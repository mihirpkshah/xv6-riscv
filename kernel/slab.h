// Simple slab (slob) allocator header - for xv6-riscv
#ifndef _SLAB_H_
#define _SLAB_H_

#include "types.h"

// Opaque slab cache type
struct slab_cache;

// Create a slab cache for objects of size `objsize`.
// Returns pointer to cache, or 0 on failure.
struct slab_cache* slab_create(int objsize);

// Allocate one object from the cache (returns pointer).
void* slab_alloc(struct slab_cache* sc);

// Free an object back to the cache.
void slab_free(struct slab_cache* sc, void* obj);

#endif // _SLAB_H_
