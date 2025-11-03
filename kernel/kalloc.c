// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

// Compute maximum number of pages available in physical memory.
// PHYSTOP and KERNBASE are compile-time macros in param.h / memlayout.h.
#define MAXPAGES ((PHYSTOP - KERNBASE) / PGSIZE)

struct {
  struct spinlock lock;
  // Stack of free page physical addresses (PA).
  // We push on free and pop on alloc (LIFO).
  uint64 free_pages[MAXPAGES];
  int free_count;
} kmem;


// Initialize allocator — used early, before kvm is setup for all memory.
// This adds the pages in [vstart, vend) to the free-list.
// Initialize for the rest of physical memory after paging is enabled
void
kinit()
{
  initlock(&kmem.lock, "kmem");
  kmem.free_count = 0;
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end) {
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa.
// pa must be page-aligned, and within [KERNBASE, PHYSTOP).
void
kfree(void *pa)
{
  uint64 a = (uint64)pa;

  // sanity checks similar to original xv6
  if (a % PGSIZE || a < KERNBASE || a >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling references.
  memset((void*)a, 1, PGSIZE);

  acquire(&kmem.lock);
  if (kmem.free_count >= MAXPAGES) {
    release(&kmem.lock);
    panic("kfree: free list overflow");
  }
  // push the page physical address onto the stack
  kmem.free_pages[kmem.free_count++] = a;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer to the page interior (as void*), or 0 if none.
void*
kalloc(void)
{
  acquire(&kmem.lock);
  if (kmem.free_count <= 0) {
    release(&kmem.lock);
    return 0;
  }
  // pop from stack
  uint64 a = kmem.free_pages[--kmem.free_count];
  release(&kmem.lock);

  //fill with junk to help catch usage
  memset((void*)a, 5, PGSIZE);
  return (void*)a;
}
