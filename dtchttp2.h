
struct ssl_info_t {
      char event;
      int pid;
      int uid;
      unsigned int fd;
      unsigned long size;
      unsigned char buf[64];
};