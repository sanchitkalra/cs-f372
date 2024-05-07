# Page Table Structure

- page table size if v large -> difficult to store in contigous blocks

## Heirarical

- _page the page table_
- prev logical addr -> bits for page number + bits for offset
- now breakup page number bits into as many levels needed
- eg. for 2 levels logical addr -> p1 + p2 + offset
  - p1 -> index to outermost page table -> index in outermost page table gives base addr of inner page table
  - then p2 -> index to inner page table -> index in inner page table gives the base addr of the frame to be accessed
  - offset -> offset in a particular frame

## Inverted

- tracks physical pages (frames)
- one entry for each frame
- page table has a <pid, page-number, i> triple
  - basically search the table via this tuple of pid and page-number now. prev we used to store a page table for each process
  - now there is sort of a grand page table that contains an entry for each frame and it is indexed by this tuple
  - once you get the `i` which is the index of the frame, and to this add the offset and you have your physical addr
- not efficient -> need to do sequential search but could be much smaller than keeping a track of a page table for each process
