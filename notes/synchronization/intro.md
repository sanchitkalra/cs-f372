# Synchronization

- process can exec concurrently, which may be interrupted ay any time
- concurrent access to shared data may cause data inconsistency
- maintaining data consistency requires various mechanisms to ensure orderly execution of coorperating processes

## Producer Consumer Problem

- one process is the consumer, and one is a producer, and there is a shared buffer between them
- producer puts items in the buffer and consumer reads from the buffer, and we're keeping track of when the buffer is full (via a counter) (block producer because no space to insert more items) or empty (block consumer because there are no items available to process)
- whenever a variable is updated, the variable is first read into a CPU register from RAM, updated, and then written back to RAM

```
register = counter; // register populated with value from counter in RAM
register += 1 // register value incremented
counter = register // register value written back to counter in RAM
```

- unless atomicity is not there for all instructions for an update (all instructions execute together), and there is interleaved execution, it may happen that the updates don't happen correctly and an incorrect value is written to the shared variable in RAM
- this is called a **race condition** -> two or more processes are racing for access to a shared resource, and because of no control/restriction on how they were accessing and/or updating the shared resource, we get problems

## Critical Section Problem

- consider a set of n processes (p0, p1, ... p_n-1)
- each process has a critical section segment of code where the process may be trying to access (and possibly modify) a shared resource
- now when process is in the critical section, no other process should be allowed to access the critical section
- _we're not disallowing access, we're disallowing simultaneous access_
- critical section problem is to design protocols to solve this issue
- say the general structure of your program is as follows

```
do {
    entry section
        critical section
    exit section
    ...
    remainder section
} while (true);
```

- 3 essential parts of this program are the
  - entry section. this is where a process gains the permission to enter the critical section. one of the processes enters the critical section, and the rest of the processes wait in the entry section
  - exit section. once the processes' critical section execution has ended, the process will take some actions so that other processes can enter the critical section
  - remainder section. code here does not involve anything with the critical section
- our goal is to make sure only process is accesing the critical section at once, by writing the entry and exit sections
- the protocol we design will not affect the scheduling algorithm _but may affect the subsequent scheduling_
- basic requirements for an algorithm designed to solve the critical section problem

  1. mutual exclusion. if a process p_i is executing in it's critical section, no other processes should be executing in their critical sections
  2. progress. if no process is executing in the critical section, and some processes want to enter the critical section, only processes not executing in their remainder section can participate in deciding which process will enter the critical section next, and this decision cannot be postponed indefinitely. simply put, any process that doesn't need to/wish to enter the critical section should not influence the decision on which process enters the critical section next, _only the competing processes decide_
  3. bounded waiting. a bound must exist on the number of times other processes are allowed to enter the critical section after a process has made a request to enter the critical section, and before that request is granted. usually we use a linear polynomial of N (the number of process)

- Algorithms:
  1. [Peterson Solution](solutions/peterson-solution.md)
  2. [Locks](solutions/locks.md)
  3. [Mutex](solutions/mutex.md)
  4. [Semaphore](solutions/semaphore.md)

## Classical Syncronisation Problems

### Bounder Buffer Problem

- say the buffer between our producer & consumer if of size n; it cannot be of size infinity or the calls would never block.
- we'll keep track of 3 things:
  - mutex (really a semaphore) init to 1. controls access to the buffer
  - full (number of full slots) init to 0
  - empty (number of empty slots) init to n
- producer:

```c
do {
  // entry section
  wait(empty);
  wait(mutex);

  ... // critical section

  // exit section
  signal(mutex);
  signal(full);
} while (true);
```

- consumer:

```c
do {
  // entry section
  wait(full);
  wait(mutex);

  ... // critical section

  // exit section
  signal(mutex);
  signal(empty);
} while (true);
```
