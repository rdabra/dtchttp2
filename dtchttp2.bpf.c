#include "dtchttp2.h"
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>

struct {
      __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);
      __uint(key_size, sizeof(unsigned int));
      __uint(value_size, sizeof(unsigned int));
} output SEC(".maps");

SEC("tp/syscalls/sys_enter_read")
int peek_sys_read(struct trace_event_raw_sys_enter *ctx) {
   int key = 0;
   struct ssl_info_t data;
   data.pid = bpf_get_current_pid_tgid() >> 32;
   data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

   bpf_probe_read_kernel(&data.size, sizeof(data.size), &ctx->args[2]);

   bpf_probe_read_kernel(&data.fd, sizeof(data.fd), &ctx->args[0]);
   unsigned char *buf = (unsigned char *)ctx->args[1];
   data.event = 'r';

   data.size = data.size > sizeof(data.buf) ? sizeof(data.buf) : data.size;
   bpf_probe_read_user(&data.buf, data.size, buf);

   bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data));
   return 0;
}

SEC("tp/syscalls/sys_enter_write")
int peek_sys_write(struct trace_event_raw_sys_enter *ctx) {
   int key = 0;
   struct ssl_info_t data;
   data.pid = bpf_get_current_pid_tgid() >> 32;
   data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

   bpf_probe_read_kernel(&data.size, sizeof(data.size), &ctx->args[2]);

   bpf_probe_read_kernel(&data.fd, sizeof(data.fd), &ctx->args[0]);
   unsigned char *buf = (unsigned char *)ctx->args[1];
   data.event = 'w';

   data.size = data.size > sizeof(data.buf) ? sizeof(data.buf) : data.size;
   bpf_probe_read_user(&data.buf, data.size, buf);

   bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data));
   return 0;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";