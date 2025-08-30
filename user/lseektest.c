#include "kernel/types.h"
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
  char* addr = (char*) malloc(101);
  addr[100] = '\0';
  int fd = open("README", 0);
    if(fd < 0){
      printf("open(README) failed\n");
      exit(1);
    }

    printf("File README opened successfully, fd = %d\n", fd);
    printf("Initial (Sample) read 100 bytes:\n");
    readnext(fd, (void*)addr);
    
    printf("Test 1: SEEK_SET to 200, read 100 bytes:\n");
    performlseek(fd, 200, SEEK_SET);
    readnext(fd, (void*)addr);

    printf("Test 2: SEEK_CUR to 100 {i.e. cur=300+100=400}, read 100 bytes:\n");
    performlseek(fd, 100, SEEK_CUR);
    readnext(fd, (void*)addr);

    printf("Test 3: SEEK_END to -100, read last 100 bytes:\n");
    performlseek(fd, -100, SEEK_END);
    readnext(fd, (void*)addr);

    close(fd);
    return 0;
}