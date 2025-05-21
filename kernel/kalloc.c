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

// NCPU freelists
struct {
  struct spinlock lock;
  struct run *freelist;
  char lockname[10];
} kmem[NCPU];

char name[10]; // Store lock names

// NOTE: see main(). kinit() only executes if cpuid() == 0!
// So freerange() only executes once!
// This step will give all free memories to CPU0!
void
kinit()
{
  for (int i = 0; i < NCPU; i++) {
    snprintf(kmem[i].lockname, 10, "kmem_CPU%d", i);
    initlock(&kmem[i].lock, kmem[i].lockname); // Use "kmem", format later.
  }
  // Give all free memories to CPU 0
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

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  push_off();
  int id = cpuid();
  pop_off();
  acquire(&kmem[id].lock);
  r->next = kmem[id].freelist;
  kmem[id].freelist = r;
  release(&kmem[id].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
// Steal memory from other CPUs if this CPU has no free memory
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int id = cpuid();
  pop_off();
  acquire(&kmem[id].lock);
  r = kmem[id].freelist;
  if(r) {
    kmem[id].freelist = r->next;
    release(&kmem[id].lock);
  } else {
    // Steal! First release the current lock in case of deadlock
    release(&kmem[id].lock);
    for (int i = 0; i < NCPU; i++) {
      if (i == id) continue;
      acquire(&kmem[i].lock);
      // The following commented code is correct, but it further exacerbates the contention!
      // Need a better way.
      // r = kmem[i].freelist;
      // if (r) {
      //   kmem[i].freelist = r->next;
      //   release(&kmem[i].lock);
      //   break;
      // }
      r = kmem[i].freelist;
      if (r) {
        // Use Fast-Slow Pointers, to give half of the free memory of this CPU to the current one!
        struct run *slow = r, *fast = r, *prev = r;
        while (fast && fast->next) {
          prev = slow;
          slow = slow->next;
          fast = fast->next->next;
        }
        // Slow points to the middle of the freelist.
        // Need to handle the corner case, where another CPU only has one free page.
        if (slow == prev) {
          // No free pages for CPU i
          kmem[i].freelist = 0;
        } else {
          prev->next = 0;
        }
        release(&kmem[i].lock);
        // Update current CPU's free memory;
        r = slow;
        acquire(&kmem[id].lock);
        kmem[id].freelist = slow->next;
        release(&kmem[id].lock);
        break;
      }
      release(&kmem[i].lock);
    }
  }

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
