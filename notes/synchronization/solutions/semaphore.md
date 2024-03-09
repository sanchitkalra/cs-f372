# Semaphore

- uses an integer variable S
- apart from init, S can only be accessed via two functions `wait()` (orginally called `P()`, p => test the value of something) and `signal()` (originally called `V()`, v => increment the value of something)
- wait -> entry section, signal -> exit section
- wait and signal should not be executed by two processes simultaneously and they are themselves atomic operations

```
// semaphore implementation with busy waiting
wait (S) {
    while (S <= 0) {
        // busy wait
        // all processes wait until S > 0 and as soon as a process releases the lock by calling signal, a process may break out of this loop and acquire the lock
    }
    S--; // acquire the lock by decrementing S, so that the condition S > 0 is not fulfilled for other processes
}
signal(S) {
    S++; // release the lock by incrementing S, and making the condition S >= 0 false, and letting a process break out of the wait's while loop
}
```

- **example:** let's say S is init to 1. P1 and P2 are two processes that want to access the critical section. Say P1 calls the function wait. the loop sees the value of S > 0, and the loop breaks and P1 acquires the lock by decrementing S. In the meantime before P1 calls signal, if P2 calls wait, it will see that the value of S is 0 and the loop condition is satisfied and P2 is stuck in this loop. When P1 releases the lock by calling signal and incrementing S, P2's loop condition becomes false and P2 can now acquire the lock

- types of semaphore:
  1. binary semaphore. only two values for S, 0 & 1. it is like a mutex lock.
  2. countin semaphore. integer value can range over an unrestricted domain. (0 -> N). this is useful when we have multiple copies of the same resource, and we need to ensure mutual exclusion for each of those copies. for example, if we have 5 copies of the same file, 5 processes can access them at the same time => 5 processes can be executing in the critical section at the same time.

## Implementation without busy waiting

- each semaphore has an integer value, and a list of processes represented as a list

```c
typedef struct {
    int value; // magnitude of -ve value of this value => number of processes blocked on the semaphore
    struct processes *list; // usually implemented as a FIFO queue
} semaphore;
```

- we will implement two functions:
  - we will now try to instead of making a process busy wait, we will **block** the process (make it transition to waiting queue) (the process list in the struct is like a device queue, all these processes are waiting for the semaphore)
  - when a process has executed a signal call => somebody who has been blocked needs to be woken up => **wakeup** removes a process from the waiting queue and places it in the ready queue. _exactly in which order they will be scheduled depends on the scheduler and is not in our hands_

```c
// implementation without busy waiting
wait(semaphore *S) {
    S -> value--;
    if (S -> value < 0) { // -ve semaphore value => count of blocked processe
        // add this process to S -> list;
        block();
    }
}

signal(S) {
    S -> value++;
    if (S -> value <= 0) {
        // remove a process P from S -> list;
        wakeup(P);
    }
}
```

## Deadlock & Starvation

- incorrect usage of semaphores leads to deadlock and starvation problems
- **Deadlock**: two or more processes are waiting indefinitely for an event that can be caused by only one of the waiting processes. essentially, a loop kind of condition occurs, say P1 is waiting for P2 to do something (say x) which depends on P1 itself doing something (say y) which depends on P2 doing x, and a cycle is formed. Now because both are blocked, they are in a deadlock situation.
- Incorrect code -> deadlock will always occur, all it says is that a deadlock _may_ occur but nonetheless the code is considered unsafe.
- **Starvation**: Starvation issues can be encountered with semaphores if we use a LIFO queue.
