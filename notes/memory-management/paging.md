# Paging

- physical addr space non contiguous -> process allocated space wherever possible
  - avoid external fragmentation
  - avoid compaction
- note that is main memory is being compacted, the backing store is also to be compacted and that is even more time consuming
- divide physical mem into fixed size blocks -> frames (size is a power of 2)
- divide logical memory into blocks of same size as frame size called pages
- logical mem -> pages; physical memory -> frames
- the frame and page size is same for all processes
- assumption: memory is byte addressable -> each byte has a different physical addr
- keep track of all unallocated (free) and allocated frames
- process size N pages -> need to find N free frames (because again frame and page sizes are the same)
  - even is byte requirement is 1 more than the size of N pages, we need to allocate N+1 pages
  - so we can decrease the page size to decrease internal fragementation but then page entries will be too many (and more memory will be required) and that isn't desirable either. we need to find a compromise
- page table -> translate logical to physical addr
  - one entry per page
- paging does not solve internal fragementation
- addr gen by CPU -> tuple <page-number(p), page-offset(d)>
  - p -> index to page table with base addr
  - if logical addr space is 2^m bytes (remember logical addr space = collection of a number of pages), and each page size is 2^n bytes, we can use the first (m - n) bytes of the address to go to the correct page, and n bytes for the offset
- don't need to do explicit bound checking on d, since if p + d > page size -> it will automatically trigger an error since we'll be going out of page boundary
- translation scheme should specify which page of the logical addr space of the process is loaded into the actual physical frame in the main memory
- physical page -> frame
- page table also lives in main memory
- size of logical addr space gives size of the logical addr
- translation: frame number \* frame size + offset = physical addr
- Frame table -> one entry for each frame indicating whether the frame is free or allocated and if allocated, to which page of which process
- page table base reg (PTBR) -> points to the page table
- page table len reg (PTLR) -> size of page table
- two mem access for each data/instruction access
  - one for page table
  - second for the data/instruction in some frame
- can be improved with hardware caches called associative memory or translation look-aside buffers (TLB)

## TLB

- v high speed cache
- small in size
- store few entries from the page table, page number and corrsp frame number/address
- entries not necessarily ordered, but allows parallel search -> O(1)
- insert most recently read page into TLB
- if page found in TLB (called TLB hit), use it to find physical addr, or else if TLB miss -> use page table in memory
- goal: % of time you find your page in TLB should be high
- say TLB lookup time is E, and hit ratio is A, and M is memory access time. so effective memory access time (EAT) becomes `EAT = A*(E + M) + (1 - A)*(E + M + M)`
  - E + M -> E required to look up the addr in the TLB, and M to actually lookup the information in the memory
  - E + M + M -> E to look up the TLB, it misses, so M to lookup page table, and then further M to look up the actual information
- initially misses will be high but with time, inserting newly accessed page into TLB, miss ratio will go down

## Memory Protection

- include extra bits with each page table entry to tell R/W/E rules.
- valid/invalid -> a bit with each entry in the page table.
  - valid -> associated with the process' address space
  - invalid -> not associated with the process' address space
- violation -> trap in the kernel
- internal fragmentation space is still technically in the process' legal address space, access attempts will not fail even though it should not be addressable by this process. but key point is even though we can access this space, we are not accessing some other process' address space and we are not causing problems becuase that extra space was allocated by the OS itself.
