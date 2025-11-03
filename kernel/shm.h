#include "types.h"

struct ipc_perm {
  int          mode;  /* r/w permission (see chmod(2)) */
  unsigned short  _seq;  /* Reserved for internal use */
  int           _key;  /* Reserved for internal use */
};

shm_table_entry;

struct shmid_ds {
  struct ipc_perm  shm_perm;     /* operation permissions */
  uint64           shm_segsz;    /* size of segment in bytes */
  int            shm_lpid;     /* pid of last shm op */
  int            shm_cpid;     /* pid of creator */
  short            shm_nattch;   /* # of current attaches */
  uint64           shm_atime;    /* last shmat() time*/
  uint64           shm_dtime;    /* last shmdt() time */
  uint64           shm_ctime;    /* last change by shmctl() */
  void            *shm_internal; /* sysv stupidity */
};

#define MAX_SHM 64

struct shmid_ds shmids[MAX_SHM];