// xv6 holds copied of blocks that it has read into memory,
// using struct buf.
// The data stored here is sometimes out of sync,
// e.g., have not yet been read in from the disk,
// or have been updated from the software but not yet been written to the disk
struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?
  uint dev;
  uint blockno;
  struct sleeplock lock;
  uint refcnt;
  struct buf *prev; // LRU cache list
  struct buf *next;
  uchar data[BSIZE];
};

