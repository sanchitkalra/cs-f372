# Mass Storage

- disc -> platters made of metal or glass
- disc -> two surfaces covered in a magenetic material
- rotating spindle in the middle
- disc divided into tracks (concentric circles)
  - each surface has same number of tracks
  - tracks at the same distance from the center for all the platters constituite a cylinder
- tracks are divided into sectors (usually 256/512 bytes)
- one arm (aka read-write head) for each upper and lower surface of each platter
  - makes sure data is read/written from/to the disk
  - it should only glide, not touch the disk
  - arm can go forward and backward to select the track
  - arms move in the same direction
- only one I/O track can be read/written at a time, because the arm cannot be in two places at once, but if the sector to be read are in the same track, then it can be done, otherwise it will be queued
- sector -> smallest unit of logical data transfer
- transfer rate: data flow rate between disk and computer
- positioning time (random access time): time to move disk arm to desired cylinder (seek time) and time for desired sector to rotate under the disk head (rotational latency)
- head crash: when disk head makes contact with disk surface -> that location and the sectors in those locations _may_ go bad and R/w _may_ not be possible anymore

## Disk Structure

- sectors are numbered using tracks
  - outermost cylinder is track 0
  - any one sector on track 0 is numbered 0, and then number all sectors in that track
  - then consecutively the sectors in the next platter on the same track are numbered
- addr of a sector: <cylinder-number, track-number, sector-number>
- logically for us we assume it is a 1-d array of logical blocks
  - logical block size is defined by the file system we use
  - storage is allocated in units of blocks
  - block size >= sector size
  - one block = one or more sectors
  - blocks -> logical domain, smallest unit during creation of files
  - sector -> smallest physically addressable unit

## Disk Scheduling

- goal: minimise seek time
  - optimise the order in which the disk head moves
- disk bandwidth: total number of bytes transferred divided by the total amount of time between the first request for service and the completion of the last transfer
- OS maintains a queue of I/O requests

### FCFS

- assumption: the seek time for a track in all cylinders is the same
- given will be the cylinder number to be accessed
- total head movement -> number of cylinders accross which the head moved
- for given set of I/O requests -> the more head moves -> more time required -> time to service a request depends on the time of head movement -> time of head movement depends on the order in which requests are serviced

### SSTF (Shortest Seek Time First)

- select request with minimum seek time from current head position
- may cause starvation of certain requests

### Scan/Elevator Algorithm

- initial direction will be given in question
- it goes till the extreme even if there is no request in that direction anymore (ie, it goes to 0 and N-1 even if there is no request at 0 or N-1)
- keep moving in the direction you are already moving till boundary is hit. if still more requests -> switch the direction
- if requests over -> no need to go to boundary
- drawback: even if a request comes in early, but is in the opposite direction to the head movement, they have to wait for a long time

### C-Scan Algorithm

- need current head direction and initial head position
- keep moving in the direction it is moving already till you hit the extreme in that direction and then swing to the other exteme end and move in the initial direction again

## RAID

- Redundant array of independent disks
- more disks for backup/redundancy
- backup and some data can be reconstructed and reliability increases
- redundancy => reliability
- reliability measure
  - MTTF (mean time to failure): the amount of time a component works properly before failing
  - MTTR (mean time to repair): avg amount of time to replace/repair a failed component to it's working condition (downtime)
- best redundancy -> mirrored disks/volumes -> for every disk there is a backup disk
  - also the most expensive solution

### Data Stripping

- splitting our data accross multiple disks
- bit level stripping: each bit of a byte is stored on a different disk
- byte level stripping: each byte of some data is stored on different disk
- block level stripping: each block of some data is stored on different disks
- splitting imporves read performance -> can read in parallel
- bit and byte level stripping don't help with multi-process parallelism

### RAID levels

#### RAID 0

- no redundancy
- only parallelism using block stripping
- best write performance -> no redundant information to write/update

#### RAID 1 (Mirrored)

- two copies of each data
- each strip on physically different disks
- storage capacity halved
- good read performance using parallelism
- write/update costly -> have to write/update twice

#### RAID 2 (Error correcting codes)

- bit level stripping
- even number of 1's -> even parity (p = 0); odd number of 1's -> odd parity (p = 1)
- maintain 1 extra bit of data for every 8 bits in a seperate disk for parity bits
- m data disks, k error code disks -> (m + k) <= 2^k - 1
- not used in mordern systems; used in old drives which did not have ECC or sector error detection

#### RAID 3 (Bit-Interleaved parity)

- single check disk/parity disk with parity bit for error detection and correction
- byte level data stripping
- parity disk fails -> all parity info gone

#### RAID 4 (Block-Interleaved parity)

- single check disk/parity disk with parity bit for error detection and correction
- block level data stripping
- independent data access -> high read performance
- parity disk fails -> all parity info gone

#### RAID 5 (Block-Interleaved distributed parity)

- each disk stores some blocks of data and some blocks of parity information
- even if one disk fails -> all parity information is not lost
- parity of set of blocks is not stored on the disks where either of those blocks are stored
- parallel read parallel
- not resilient to multi-disk failures

#### RAID 6 (P + Q redundancy)

- parity blocks are also repicated and stored in different disks
- both parity and it's replica are stored on different blocks and neither of them on the blocks for whose parity they are maintaining
