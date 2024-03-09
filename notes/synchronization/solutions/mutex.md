# Mutex

- hardware solutions can be weaponised, are complicated, and inaccessible to application developers.
- we need more software based solutions
- mutex also works with the concept of acquring and releasing locks
- mutex has a variable `available` indicating if the lock is available or not
  - true => lock is available
  - false => lock already held by some other process
- calls to acquire and release must be atomic and are implemented using atomic hardware instructions
- also no two processes should be able to call acquire simultaneously
- busy waiting: whenever a process is waiting for a resource, we push it out of the ready queue and put it in the device queue for that resource. pushing a process out of the ready queue => not allowing that process to utilise CPU cycles. now busy waiting => the process is not only in the ready queue, it is running as well checking for some condition and this way it is consuming and wasting CPU cycles. neccessary evil.
- spin lock: a process waiting to acquire the lock, keeps spinning in the while condition waiting for the lock to become available.
- one mutex can ensure mututally exclusive access for only one instance of a resource. for more than one instance, we need multiple mutex locks.
- code:

```c
acquire() {
    while (!available) {
        // busy wait
    }
    available = false;
}
release() {
    available = true;
}
do {
    acquire(); // entry
        // critical section
    release(); // exit

    // remainder section
} while(true);
```

- advantages:
  - no context switch required when a process waits for a lock. otherwise 3 process state transitions would be required: 1. running to waiting when lock becomes unavailable 2. waiting to ready when lock is available 3. ready to running. also if all the processes are running, which process gets the lock can be determined algorithmically, but if they transition to waiting states, now once they're in the ready state, the decision depends on the scheduler and a lot of overhead is required.
  - useful when locks are to be held for small amount of time (especially when execution time of critical section < context switch time).
- disadvantage is that it wastes CPU cycles by busy waiting.
