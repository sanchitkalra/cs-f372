# Virtual Memory Management

- it is possible to exec a partially loaded prog
- less mem while running -> more programs can be run -> less need to do swaps (less I/O needed, and even when needed, smaller amounts of data will be swapped) -> each program runs faster
- virtual mem -> seperate logical mem from physical mem
- logical addr space can be larger than the physical addr space, it could be that your application has 1000 pages, but in reality is only allocated 5 frames
- virtual addr space -> logical view of how processes are stored in memory, usually as the process begins at address 0 and in memory
- in a process' virtual address space, the stack grows from top to bottom (decreasing addr values) (used for successive fn calls) and heap grows from lower address values to higher address values (dynamic memory allocation). the space between stack and heap is part of the virtual addr space _but requires actual pages **only** if the stack or heap grows_
  - addr space with holes -> sparse addr space

## Demand Paging

- bring page into memory on demand; less and no unnecessary I/O.
- if page is needed -> ref to it -> invalid ref => abort; not in memory => bring into memory
- the actual memory swapper is _lazy_, does work only when it's needed; swapping is usually swapping in/out an entire process, and a better term is pager. pager doesn't take out the entire process' pages but only take out specific pages
- pager guesses which pages will be used before swapping out again
- process exec starts with the pager guessing what pages would be required and brings in those pages in advance
- initially page faults will be high (ie, pages not found in memory) but will reduce over time. page already in memory is called a **memory resident page**.
- one instruction can cause multiple page faults as well
- pure demand paging: at start, don't load anything except the bare minimum; load only on demand
- **locality of reference**: tendency of processor to access the same set of memory locations (pages) repetitively over a short period of time
  - demand paging works _because of_ locality of reference
  - this locality is defined by prog structure and data structures
  - processes migrate from one locality to another locality, and may also contain multiple localities and the localities may also overlap

## Valid-Invalid Bit

- with each page table entry a valid-invalid bit is associated
  - v -> legal and memory resident. no addressing error or page fault
  - i -> either illegal or not in memory
- initiaally `i` for all pages, and if `i` MMU -> page fault
- servicing page fault: when page fault -> process is interrupted because interrupt is generated because the memory location it is trying to access in not in memory -> this interrupt's ISR checks if it is a valid reference or is just not in memory. if valid, then loads page into memory by finding a free frame or making one free by swapping, else abort -> modify entries of page table set valid/invalid bit to `v` -> resume program execution from the instruction that caused the page fault

## Performance of Demand Paging

- three major activities: 1. service interrupt 2. read page 3. restart process
- p -> page fault rate (0 <= p <= 1)
- Effective access time (EAT) = (1 - p) \* mem access time + p \* (page fault over head + swap out page + swap page in)
  - page fault over head + swap out page + swap page in -> page fault service time
- goal: p as small as possible

## Page replacement

- only write modified pages to disk; identify using a **modify (dirty) bit**
- when need for page replacemennt
  - if free frame, use it
  - no free frame -> use a page replacement algo to select a **victim frame**
    - write victim to disk only if dirty
- update page & frame table after bringing new page into memory
- reference string: string of page numbers in the order of them being requested
- page replacement can be done in two policies
  - global frame replacement strategy: if a process needs more frames, it can replace frames of other processes
  - local frame replacement: if a process wants more frames, it can't replace frames of another process, but only it's own frames

### FIFO Replacement Algorithm

- take out the first brought in page
- of the N frames in memory, look back and see who came in the earliest to remove it from memory and replace with newer page
- adding more pages _does not_ solve the high page fault rate -> **Belady's Anomaly**

### Optimal Replacement Algorithm

- replace page which will not be used for the longest amount of time
- trying to predict the future
- not practical and only used for benchmarking
- if there is a tie, look for a rule in the question, or randomly remove any one

### Least Recently Used (LRU) Algorithm

- can't predict the future but can look at the past
- replace page that has not been used in the most amount of time
  - associate time of last use with each page
- can be implemented using a counter or timestamp for each page that has ever been brought into memory
- can also be implemented using a Stack, since it is a LIFO data structure, the moment a page is referenced, move it to the top, and in effect the least recently used page moves to the bottom
- can be implemented in O(1) using a doubly linked list as well
- LRU and OPT do not suffer from Belady's Anomaly
- on an average LRU performs better than FIFO but worse than OPT
- a complex memory manegment scheme like LRU is still better than accessing disk

## Allocation of frames

- each process needs a min number of frames -> determined by comp arch
  - at least two pages. 1. for instructions 2. for data the instruction manipulates
- max -> total number of frames - frames allocated to OS
- two major allocation schems
  - equal allocation: allocate each process the same number of frames from the frames left after OS frame allocation. Downside is that page fault rate will be very high for programs that need a lot of frames, and very less for programs that don't need as many frames
  - proportional allocation: larger processes get more frames and smaller processes will get fewer frames. dynamic as degree of multiprogramming, process size changes. here variable number of processes can be executed unlike in equal allocation. allocation of frames for each process = floor[ ((size of process) / (sum of size of all processes)) \* (frames available after OS frame allocation) ]

## Thrashing

- process does not have enough frames -> high page fault rate
  - high paging activity
  - process spends more time in swapping than computation
  - CPU utilisation drops -> OS thinks it needs to increase the degree of multiprogramming to increase the CPU utilisation and brings new processes into main memory -> thrashing goes up
- **thrashing**: a process is busy swapping pages in and out and high paging activity
- thrashing is much worse when we are using a global frame replacement policy relative to local frame replacement policy
- thrashing occurs because the size of the locality > total memory size
  - limit effects by using local or priority page replacement
  - allocate enough frames for a single locality

## Working Set Model

- delta: working set window -> fixed number of page references (usually most recent) -> _approximation of a program's locality_
  - it is an integer value
  - can say it is the pages it is working on currently
  - if too small -> may not cover even one locality
  - if too large -> may cover several localities -> not very useful as at one point of time only one locality will be used -> more frames than required and degree of multiprogramming and throughput comes down
  - if locality tends to infinity -> encompass entire prog
- working set: set of all the pages the process references in the last delta references
- D = sum of working sets -> total demand for frames _at a point in time_
- m -> number of available frames
- if D > m => thrashing is happening -> then suspend or swap out one of the process
- when D < m -> reintroduce processes back into memory, and optimise CPU utilisation
- ideally D <= m
- challenge is to find the Delta and D value, because is v dynamic. One way to reduce thrashing is to observe the page fault rate of our target process and overall system
  - set upper and lower bound for page fault rates of system and process
  - if rate > upper bound for process -> give more frames
  - if rate < lower bound for process -> take away frames and allocate them to processes that need it more

## Allocating Kernel Memory

- treated differently from user memory; not subjected to the same constraints
- often allocated from a free-memory pool, because kernel requests memory for data structures of varying sizes, and some kernel memory needs to be contiguous
- some data structures are going to be small and allocating a whole page may cause a lot of internal fragementation
- in some cases, external hardware interacts directly with physical memory, and in these cases, large amount of memory are required and cannot be allocated using frames because of the need to be contiguous and one frame is not enough

### Buddy System

- allocate memory from fixed size segment consisting of physically contiguous blocks
- max block size -> total size of chunk available; when memory is requested, the chunks are split up recursively into smaller chunks and allocated
- define min and max (total size of the chunk) block size
  - min is required because more splits requires memory to keep track of such splits, and at some point the efficiency of not wasting space is not worth the extra overhead of maintaining information about those splits
- chunk is a multiple of 2 always
  - this causes internal fragmentation, and we do best fit
- splits are done recursively and upto the point where the chunk size is just greater than the required chunk size or until the minimum chunk size is reached
- operations
  - allocate: allocate a block of size 2^k and mark it as allocated
  - free: marks the previously allocated block as free and merge it with other neighbouring blocks to form a larger block
- algorithm: say a process has requested memory of size X
  - if 2^(K-1) < X <= 2^K -> allocate entire 2^K block
  - else recursively split and allocate when condition is reached
    - when split happens, the left most chunk is allocated
- think of this allocation in terms of binary trees
  - only adjecent nodes can be merged -> they should have a common parent
  - imp point: **don't split unless absolutely necessary**. even if a block of the correct size or one that requires fewer splits is available elsewhere apart from the leftmost subtree, use that first -> split only when no apt size blocks are unavailable
  - **for merging check tree structure, don't lean on table representation**
- _buddies are nodes with common parents_
