#include "kernel/types.h"
#include "kernel/shm.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void readnext(int fd, char* buf) {
  int n = read(fd, (void*)buf, 100);
  if(n > 0){
    printf("read(fd, %p, 100) returned %d, not -1 or 0\n", (void*)buf, n);
  } else {
    printf("read(fd, %p, 100) failed\n", (void*)buf);
    exit(1);
  }
  printf("This is the value:\n");
  printf("%s\n", (char*)buf);
}

void performlseek(int fd, int off, int whence) {
  int res = lseek(fd, off, whence);
  if (res == 0) {
    return;
  }
  switch (res)
  {
  case ESPIPE:
    printf("lseek(fd, %d, %d) failed: ESPIPE\n", off, whence);
    break;
  case EBADF:
    printf("lseek(fd, %d, %d) failed: EBADF\n", off, whence);
    break;
  case EINVAL:
    printf("lseek(fd, %d, %d) failed: EINVAL\n", off, whence);
    break;
  case EOVERFLOW:
    printf("lseek(fd, %d, %d) failed: EOVERFLOW\n", off, whence);
    break;
  default:
    printf("lseek(fd, %d, %d) failed: Unknown response: %d\n", off, whence, res);
    break;
  }
  exit(1);
}

int main(int argc, char *argv[]) {
  shmget(0, 4096, 0);
  shmat(0, 0, 0);
  shmdt(0);
  shmctl(0, 0, 0);
  return 0;
}