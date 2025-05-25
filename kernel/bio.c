// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

struct {
  struct spinlock lock;
  struct buf buf[NBUF];
  struct buf bucket[NBUCKET];
  struct spinlock bucket_locks[NBUCKET];
} bcache;

void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");

  for (int i = 0; i < NBUCKET; i++) {
    initlock(&bcache.bucket_locks[i], "bcache_bucket_lock");
  }
  struct buf *current = &bcache.bucket[0];
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    // Store all buffers into bucket 0
    initsleeplock(&b->lock, "buffer");
    current->next = b;
    current = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  int position = blockno % NBUCKET;
  acquire(&bcache.bucket_locks[position]);
  for (b = bcache.bucket[position].next; b; b = b->next) {
    if (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.bucket_locks[position]);
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  // search all buffers, based on timestamp
  uint64 latest_time = 0;
  struct buf *p, *q;
  for (p = bcache.buf; p < bcache.buf + NBUF; p++) {
    if (p->refcnt == 0) {
      if (!b || p->timestamp > latest_time) {
        b = p;
        latest_time = p->timestamp;
      }
    }
  }

  // Acquire sequence: Small bucket -> Big Bucket
  if (b) {
    int prev_pos = b->blockno % NBUCKET;
    if (prev_pos != position) {
      acquire(&bcache.bucket_locks[prev_pos]);
      // Update the Hash-table
      struct buf *prev_bkt = bcache.bucket + prev_pos;
      struct buf *current_bkt = bcache.bucket + position;
      // Delete the old buffer
      p = prev_bkt;
      q = p->next;
      while (q) {
        if (q == b) {
          p->next = q->next;
          break;
        }
        p = q;
        q = q->next;
      }
      // Allocate this buffer to a new bucket
      b->next = current_bkt->next;
      current_bkt->next = b;
      release(&bcache.bucket_locks[prev_pos]);
    }
    release(&bcache.bucket_locks[position]);
    release(&bcache.lock);
    b->dev = dev;
    b->blockno = blockno;
    b->valid = 0;
    b->refcnt = 1;
    acquiresleep(&b->lock);
    return b;
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->timestamp = ticks;
  }
  release(&bcache.lock);
}

void
bpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

void
bunpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}


