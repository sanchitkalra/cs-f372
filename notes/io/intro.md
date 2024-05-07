# I/O

- IO devices controlled by device controllers
- device drivers -> not part of kernel -> used to interface with the device -> encapsulate device level details
- device communnicate with computer communicate by passing signals over a set of wires called the bus -> it needs a std protocol. Eg PCIe, Expansion Bus
- port -> the physical connection point for the device
  - _connect via the port and communicate via the bus_
- controller -> electronics that operate the port, bus, device, and manage the device. has some memory, buffer, and processor

## I/O Hardware

- there are special instructions to control IO hardware
- have special registers -> hold command which CPU gives to the controller via these registers
- data-in register -> actual input gets stored here, the CPU can read this to get the data/input it needs
- data-out register -> written by CPU to this register and susequently is pushed to an output register
- status register -> commands given by setting/unsetting bits -> CPU can read the bits and decide next step
- control register -> written by CPU to start a command

## Polling

- if busy bit in controller is 1 -> controller busy exec a previously given command; 0 -> controller idle
- CPU constantly checks if busy bit is 0
  - this step is the busy wait step
  - CPU cycles wasted _because I/O devices are significantly slower_
    - CPU can't wait too much (buffer may overflow)
    - solution is interrupts
- once 0, CPU sets R/w bit in command register _tell the controller what kinda op to be done from the perspective of CPU_
  - if write -> the data is but one byte at a time in the data-out register
  - then set command-ready bit in command register
  - controller now sets busy bit
  - controller see write command and reads byte from data out register and complete IO
  - controller clears busy bit, error bit (status reg), and clear command ready bit when transfer done

## Interrupts

- CPU does the tasks for setting an IO output command
- CPU has interrupt request line, and keeps checking it after every instruction if there is a signal there
- if there an interrupt => controller ready for next step of IO -> handles the interrupt via the ISR and continues the execution
- bad for very big files

## DMA

- used for large data movement
- uses a DMA controller (special purpose processor)
- Bypasses CPU to transfer data directly between I/O device and memory
- CPU gives the command and the DMA controller takes over; CPU interrupted only after the IO op is complete
- issue: system bus is one -> CPU and DMA both have access to it -> DMA sort of steals the bus but the advantage is that CPU is not interrupted for each byte

## IO interface

- Device types

  - Character-stream(transfer 1 byte of data at a time like monitor) or block(hard disk)
  - Sequential(modem) or random-access(cd,bluray disk)
  - Synchronous (response time is predictable; application blocks until IO completes and returns) or asynchronous (response time is unpredicatable; program and IO both continue and the entire data is given to the program at once via an interrupt) (or both)
  - Sharable or dedicated
  - Speed of operation
  - read-write, read only, or write only

- Types of I/O
  - Blocking
  - Nonblocking – input from keyboard and mouse
  - Asynchronous – disk and n/w I/O

## Error handling

- two kinds of errors: transitent or permanent
- OS can recover from transient failures (like disk read, device unavailable)
  - error usually have error codes associated with them

## IO protection

- no illegal IO ops
- IO op is priviledged, must be performed by sys calls
