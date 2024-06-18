#include "dtchttp2.h"
#include "dtchttp2.skel.h"
#include <bpf/libbpf.h>
#include <iostream>
#include <stdio.h>
#include <sys/resource.h>

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args) {
   if (level >= LIBBPF_DEBUG)
      return 0;

   return vfprintf(stderr, format, args);
}

void handle_event(void *ctx, int cpu, void *data, unsigned int data_sz) {
   struct ssl_info_t *m = (struct ssl_info_t *)data;

   /**
    * @brief The following condition observers the structure of HTTP2 client and server Hello
    * Messages according to https://tls12.xargs.org
    *
    * @details HTTP2 Client and Server Hello headers always have at least 9 bytes
    *          Beginning of Client Hello request: 0X16 0X03 0X01[0X03] 0x?? 0x?? 0X01
    *          Beginning of Server Hello response: 0X16 0X03 0X01[0X03] 0x?? 0x?? 0X02
    *          For better performance, the condition bellow should be implemented in the kernel
    *          space program
    */
   bool is_http2 =
       m->size >= 9 && m->buf[0] == 0X16 && m->buf[1] == 0X03 &&
       (m->buf[2] == 0X01 || (m->buf[2] == 0X03) && (m->buf[5] == 0X01 || (m->buf[5] == 0X02)));

   if (is_http2) {
      std::string what = m->buf[5] == 0X01 ? "ClientHello" : "ServerHello";
      std::string event = m->event == 'r' ? "Received" : m->buf[5] == 0X01 ? "Sent" : "Responded";

      std::cout << "PID: " << m->pid << " UID: " << m->uid << " FD: " << m->fd << " -> "
                << event + " an HTTP/2 " + what << " message" << std::endl;
   }
}

void lost_event(void *ctx, int cpu, long long unsigned int data_sz) {
   printf("lost event\n");
}

int main() {

   struct rlimit r = {RLIM_INFINITY, RLIM_INFINITY};
   if (setrlimit(RLIMIT_MEMLOCK, &r)) {
      perror("setrlimit(RLIMIT_MEMLOCK)");
      return 1;
   }

   struct dtchttp2_bpf *skel;
   int err;
   struct perf_buffer *pb = NULL;

   libbpf_set_print(libbpf_print_fn);

   skel = dtchttp2_bpf__open_and_load();
   if (!skel) {
      printf("Failed to open BPF object\n");
      return 1;
   }
   err = dtchttp2_bpf__attach(skel);
   if (err) {
      fprintf(stderr, "Failed to attach BPF skeleton: %d\n", err);
      dtchttp2_bpf__destroy(skel);
      return 1;
   }

   pb = perf_buffer__new(bpf_map__fd(skel->maps.output), 8, handle_event, lost_event, NULL, NULL);
   if (!pb) {
      err = -1;
      fprintf(stderr, "Failed to create ring buffer\n");
      dtchttp2_bpf__destroy(skel);
      return 1;
   }

   printf("HTTP/2 detection started sucessfully...\n");
   while (true) {
      err = perf_buffer__poll(pb, 100 /* timeout, ms */);

      // Ctrl-C gives -EINTR
      if (err == -EINTR) {
         err = 0;
         break;
      }
      if (err < 0) {
         printf("Error polling perf buffer: %d\n", err);
         break;
      }
   }

   perf_buffer__free(pb);
   dtchttp2_bpf__destroy(skel);
   return -err;
}
