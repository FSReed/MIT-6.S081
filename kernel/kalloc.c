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

// There are 128MB free memory in xv6, defined in memlayout.h:39-40
// As there are at most 64 processes in xv6,
// 8 bits would be sufficient to store the reference count of a physical page,
// so I choose a char array.
struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  char ref[(PHYSTOP - KERNBASE) >> 12]; // physical page ref count
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  memset(kmem.ref, 1, (PHYSTOP - KERNBASE) >> 12);  // Set the reference count to 0
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  uint64 position = (uint64)(pa - KERNBASE) >> 12;
  acquire(&kmem.lock);
  if (kmem.ref[position] == 1) {
    // If ref count of this page is more than 1, don't actually free it.

    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);
    r = (struct run*)pa;

    r->next = kmem.freelist;
    kmem.freelist = r;
  }
  kmem.ref[position] -= 1;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    kmem.freelist = r->next;
    uint64 position = ((uint64)r - KERNBASE) >> 12;
    if (kmem.ref[position] != 0) {
      panic("kalloc: page already allocated");
    }
    kmem.ref[position] = 1; // This is a newly allocated page, its ref_count would be 1.
  }
  release(&kmem.lock);

  if(r) {
    memset((char*)r, 5, PGSIZE); // fill with junk
  }
  return (void*)r;
}

// New function, for COW
void
kmeminc(void* pa) {
  uint64 position = (uint64)(pa - KERNBASE) >> 12;
  acquire(&kmem.lock);
  kmem.ref[position] += 1;
  release(&kmem.lock);
}
