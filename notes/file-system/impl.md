# File System Implementation

## File control block (FCB)

- aka `inode` in linux; struct containing information about a file, _not the contents_
- one per file
- file permissions, various dates, size, identifier, pointers to the actual data blocks

## Allocation Methods

### Contigous

- each file occupies set of contigous blocks
- random and sequential access easy
- need to store start block addr and the number of blocks the file is spanning in the FCB
- issue: if contigous blocks are not available, files can't be created; issue of external fragmentation (can be solved using compaction, needs to be done offline)
- head does not need to move, only need to rotate the disc for different sectors on the same track

### Linked

- blocks can be scattered, wherever blocks are available, they can be used
- each block need to have a pointer to the next block
  - last block has -1 or a null value to signify end of file
- external fragementation not a problem
- direct access not possible, only sequential access
- disk head may need to be moved accross multiple locations in worst case
- space waste due to pointers storage
- we can form chunks of contigous blocks to save some space wasted on maintaining pointer information
  - could cause internal fragmentation

### File Allocation Table (FAT)

- extension of linked allocation
- stored at beginning of a disk volume
- one entry for each disk blocks number from 0
- there is a directory entry containing the name and the starting block addr
- the number contained in a block is the number of the next block in the file
- last disk block has null/nil in it
- if block has -1 -> empty block

### Indexed Allocation

- each file has it's own index block of pointers to it's data blocks; this blocks stores no data
- FCB stores the indentifier and the associated index block
- random access is possible
- dynamic access without external fragmentation with overhead of index block
- some amount of space on index block may be lost to internal fragmentation
- better for larger files

## Free space management

- FS maintains a `free-space list` to keep track of available blocks/clusters
- bitvector/bitmap
- bit for each block, 0(MSB) -> N-1(LSB)
- position of block and index in bitmap 1:1 correspondence
  - block 0 -> bit number 0 (from MSB side, left side)
- 1 -> block is free; 0 -> block is allocated
- occupies large amt of space, and this should be in memory -> large overhead for larger systems
- free block calc: (number of bits/word) \* (number of 0-valued words) + offset of first 1 bit

### Linked List

- linked list of free blocks
- each free block has a special pointer to the next free block
- no need to traverse the list, take first free block
- difficult to allocate contigous blocks -> then it may need to traverse the list
- some space wasted in each block to hold pointer to next free block
