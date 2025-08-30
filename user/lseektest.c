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

int main(int argc, char *argv[]) {
  char* addr = (char*) malloc(101);
  addr[100] = '\0';
  int fd = open("README", 0);
    if(fd < 0){
      printf("open(README) failed\n");
      exit(1);
    }
    readnext(fd, (void*)addr);
    
    printf("Test 1: SEEK_SET to 200, read 100 bytes:\n");
    if (lseek(fd, 200, SEEK_SET) < 0) {
      printf("lseek(fd, 200, SEEK_SET) failed\n");
      exit(1);
    }
    readnext(fd, (void*)addr);

    printf("Test 2: SEEK_CUR to 100 {i.e. cur=300+100=400}, read 100 bytes:\n");
    if (lseek(fd, 100, SEEK_CUR) < 0) {
      printf("lseek(fd, 100, SEEK_CUR) failed\n");
      exit(1);
    }
    readnext(fd, (void*)addr);

    printf("Test 3: SEEK_END to 100, read 100 bytes:\n");
    if (lseek(fd, 100, SEEK_END) < 0) {
      printf("lseek(fd, 100, SEEK_END) failed\n");
      exit(1);
    }
    readnext(fd, (void*)addr);
    close(fd);
  return 0;
}