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

## Mutex POSIX API

- if unlocked -> acquire changes the state to locked & returns immedietly
- if locked -> acquire blocks until a call to release in another thread changes the mutex's state to unlocked, and now the acquire call sets it to locked and returns
- should not attempt to lock mutex in a thread that already holds the mutex lock -> undefined behaviour
- release unlocked mutex -> undefined behaviour. only call release when locked
- only the thread that current has the lock on the mutex (called the owner of the mutex) can release the lock
- syntax: `int pthread_mutex_init(pthread_mutex_t* mutex, pthread_mutexattr_t* attr)`
  - init mutex ref by mutex
  - init with the attributes specified by attr. if `NULL` -> default properties are used
    - kind: imp property is the kind of mutex (these different kinds are defined as macros)
      - default: defined as `PTHEAD_MUTEX_DEFAULT`. no recursive locking. does not tolerate any erroneous behaviour.
      - normal: defined as `PTHREAD_MUTEX_NORMAL`. this kind is **not** deadlock-safe (for example, if T1 has a lock & tries to acquire a lock again, it will be deadlocked along with all other threads, logic being that for this new lock to be acquired, the old lock must be released but before the lock can be released, a new request for a lock has been made by the same thread). if a mutex is unlocked, and unlock is called, there is undefined behaviour
      - error checking: defined as `PTHREAD_MUTEX_ERRORCHECK`. this kind _is_ deadlock-safe. there is no undefined behaviour. for the kind of situations defined above, errors will be returned, no deadlocks will happen & no undefined behaviour.
      - recursive: defined as `PTHREAD_MUTEX_RECURSIVE`. this time if, say, T1 calls lock after it has already acquired a lock on the mutex, there will be no deadlocks or undefined behaviour, instead a counter for the number of times lock has been acquired is associated with the mutex. the mutex will be unlocked and available to other threads only after exactly these many number of unlock calls are made. if another process attempts to acquire the lock or unlock is called on an unlocked mutex, it will return with an error.
    - pshared: tells if the mutex is sharable between the parent & child processes
- successful call -> 0 returned (+ state init & unlocked); failed call -> error code returned
- to destroy the mutex we can call `pthread_mutex_destroy(pthread_mutex_t* mutex)`
  - essentially mutex becomes `un-init`, and cannot be locked or unlocked
  - safe to destroy unlocked mutex; destroying locked mutex -> undefined behaviour
  - successful call -> 0 returned; failed call -> error code returned
- lock using `pthread_mutex_lock(pthread_mutex_t* mutex)`
  - mutex object locked
  - if mutex already locked -> calling thread blocks until mutex becomes available
  - successful call -> 0 returned; failed call -> error code returned
- unlock using `pthread_mutex_unlock(pthread_mutex_t* mutex)`
  - mutex object unlocked
  - if there are threads blocked on the mutex obj -> scheduling policy decides which thread will acquire the mutex
  - successful call -> 0 returned; failed call -> error code returned
- we can also use the function `pthread_mutex_trylock` to acquire lock on a mutex. in this case is the mutex is locked, the calling thread is not blocked, instead, the call returns immediately with an error code (`EBUSY`).
- `pthread_mutex_gettype(pthread_mutex_t* mutex)` and `pthread_mutex_settype(pthread_mutex_t* mutex)` can be used to get and set the types of the mutex resp.
