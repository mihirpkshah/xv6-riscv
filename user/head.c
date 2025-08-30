#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define DEFAULT_LINES 10
#define BUFSIZE 512

void head_fd(int fd, int nlines, int nbytes) {
  char buf[BUFSIZE];
  int n, i;
  int lines = 0, bytes = 0;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (i = 0; i < n; i++) {
      if (nbytes >= 0 && bytes >= nbytes)
        return;
      write(1, &buf[i], 1);
      bytes++;
      if (buf[i] == '\n') {
        lines++;
        if (nlines >= 0 && lines >= nlines)
          return;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int nlines = DEFAULT_LINES;
  int nbytes = -1;   // -1 means unused
  int i, fd;

  // parse args like Ubuntu head
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-n") && i+1 < argc) {
      nlines = atoi(argv[++i]);
    } else if (!strcmp(argv[i], "-c") && i+1 < argc) {
      nbytes = atoi(argv[++i]);
      nlines = -1; // override
    } else if (argv[i][0] == '-') {
      fprintf(2, "head: unknown option %s\n", argv[i]);
      exit(1);
    } else break;
  }

  if (i == argc) {
    head_fd(0, nlines, nbytes); // stdin
  } else {
    for (; i < argc; i++) {
      fd = open(argv[i], 0);
      if (fd < 0) {
        fprintf(2, "head: cannot open %s\n", argv[i]);
        exit(1);
      }
      head_fd(fd, nlines, nbytes);
      close(fd);
    }
  }
  exit(0);
}

