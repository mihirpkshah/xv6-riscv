// Simple slab (slob) allocator for xv6-riscv
// Minimal implementation: one cache per object-type; allocates pages via kalloc()

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "slab.h"

// A node representing a physical page used by the slab cache.
struct slab_page {
  void *page;                // starting address (virtual) of the page
  struct slab_page *next;
  int free_count;            // count of free objects on this page (unused in minimal impl)
};

struct freeobj {
  struct freeobj *next;
};

struct slab_cache {
  struct spinlock lock;
  int objsize;               // size of each object
  struct freeobj *freelist;  // singly-linked free objects
  struct slab_page *pages;   // list of pages allocated for this cache
};

// Round up object size to a pointer-aligned size (for free list pointer)
static int
round_obj_size(int s)
{
  // ensure space for freeobj->next pointer
  int sz = (s + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
  if (sz < (int)sizeof(struct freeobj)) sz = sizeof(struct freeobj);
  return sz;
}

struct slab_cache*
slab_create(int objsize)
{
  struct slab_cache *sc = (struct slab_cache*)kalloc();
  if (!sc) return 0;
  sc->objsize = round_obj_size(objsize);
  initlock(&sc->lock, "slab");
  sc->freelist = 0;
  sc->pages = 0;
  return sc;
}

// allocate a new page and carve it into objects, push them onto free list
static int
slab_grow(struct slab_cache *sc)
{
  void *p = kalloc();
  if (!p) return -1;

  // track this page
  struct slab_page *sp = (struct slab_page*)kalloc();
  if (!sp) {
    // cannot allocate tracking node; free page and fail
    kfree(p);
    return -1;
  }
  sp->page = p;
  sp->next = sc->pages;
  sc->pages = sp;

  // carve the page into objects
  int objsz = sc->objsize;
  char *base = (char*)p;
  int n = PGSIZE / objsz;
  for (int i = 0; i < n; i++) {
    struct freeobj *fo = (struct freeobj*)(base + i * objsz);
    fo->next = sc->freelist;
    sc->freelist = fo;
  }
  sp->free_count = n;
  return 0;
}

void*
slab_alloc(struct slab_cache* sc)
{
  if (!sc) return 0;
  acquire(&sc->lock);
  if (!sc->freelist) {
    // grow (release lock while doing kalloc is possible but simpler keep lock)
    if (slab_grow(sc) < 0) {
      release(&sc->lock);
      return 0;
    }
  }
  struct freeobj *o = sc->freelist;
  sc->freelist = o->next;
  release(&sc->lock);
  // zero the object for safety
  memset(o, 0, sc->objsize);
  return (void*)o;
}

void
slab_free(struct slab_cache* sc, void* obj)
{
  if (!sc || !obj) return;
  acquire(&sc->lock);
  struct freeobj *fo = (struct freeobj*)obj;
  fo->next = sc->freelist;
  sc->freelist = fo;
  release(&sc->lock);
}