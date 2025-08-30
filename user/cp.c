#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/fs.h"

int
main(int argc, char *argv[])
{
  int fd1, fd2, n;
  char buf[512];

  if(argc != 3){
    fprintf(2, "Usage: cp <source> <destination>\n");
    exit(1);
  }

  fd1 = open(argv[1], 0);
  if(fd1 < 0){
    fprintf(2, "cp: cannot open %s\n", argv[1]);
    exit(1);
  }

  fd2 = open(argv[2], O_CREATE | O_WRONLY);
  if(fd2 < 0){
    fprintf(2, "cp: cannot create %s\n", argv[2]);
    close(fd1);
    exit(1);
  }

  while((n = read(fd1, buf, sizeof(buf))) > 0){
    if(write(fd2, buf, n) != n){
      fprintf(2, "cp: write error\n");
      close(fd1);
      close(fd2);
      exit(1);
    }
  }

  close(fd1);
  close(fd2);
  exit(0);
}

