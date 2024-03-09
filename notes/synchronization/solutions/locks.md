# Locking

- Sync hardware. many systems provide hardware level support for critical section problem. all these solutions are based on the idea of protecting critical section by locking. once one entity or process has a lock, others cannot acquire the lock unless the entity that holds the lock releases it. modern machines provide atomic (non-interruptable) hardware instructions. basically the high level instructions (translated as a set of machine level instructions) should be executed as an atomic uninterruptable unit. the process cannot be preempted when it is executing that set of atomic instructions.

- basic code:

```c++
do {
    accquire lock // entry
        critical section
    release lock // exit

    ...

    remainder section
} while (true);
```

- to use this idea, we'll implement two instructions (these are machine level instructions represented as high level code)

## Instruction 1: test_and_set

- we store the old value, setting new value as true, and return the new value.

```java
boolean test_and_set(boolean* target) {
    boolean rv = *target; // test part
    *target = true; // set part
    return rv;
}
```

### Solution using test_and_set

```java
// shared variable lock, init to false
do {
    while (test_and_set(&lock)) { // entry section
        // this loop will be stuck as long as lock is true
        // do nothing
    }

    // critical section

    lock = false; // exit section

    // remainder section

} while (true);
```

- **example:** say there are two processes P1 and P2, lock is initally false and P1 wants to enter the critical section. in P1's execution, in the test_and_set function, rv = false, and target is set to true, and false is returned and subsequently, the while loop in P1 is exited and P1 enters the critical section. now P2 also wants to enter the critical section. for P2's call to test_and_set, rv = true, and target is also set to true, and true is returned and P2 will be stuck in the while loop. after P1 exits the critical section, lock is set to false. now in the next iteration of the while loop of P2, rv is false, and the while loop breaks and P2 enters the critical section, and now in next iteration of P1, it waits for the lock to be released by P2.

- this approach does not follow the bounded execution requirement because P1 may keep acquiring locks over and over again if it gets to call test_and_set before P2, and P1 will again break out of the loop and acquire it again.

## Instruction 2: compare_and_swap

- we store the given value, compare the given value with the expected value, and if it is true, we store the new value in the original value.

```java
int compare_and_swap(int *value, int expected, int new_value) {
    int temp = *value;
    if (*value == expected) {
        *value = new_value;
    }
    return temp;
}
```

### Solution using compare_and_swap

```java
// shared global integer lock init to 0
do {
    while (compare_and_swap(&lock, 0, 1) != 0) { // entry section
        // do nothing
    }

    // critical section

    lock = 0; // exit section

    // remainder section
} while (true);
```

- **example:** suppose lock is init to 0 and P1 and P2 are the two processes trying to access the critical section. when P1 calls compare_and_swap, it returns 0, and sets the value to 1, and breaks out of the while loop because the condition != 0 is not satisfied and acquires the lock. now in the meantime if P2 tries to acquire the lock by calling compare_and_swap, it returns 1, and does not set value to anything because value != expected value (1 != 0), and the loop condition is satisfied and P2 is stuck in the while loop until P1 reaches the exit section, and releases the lock by setting the lock value to 0. Now another call to compare_and_swap by P2 will return 0 and it can break out of the loop, and acquire the lock.

- this solution also does not satisfy the bounded waiting requirement because if P1's next call to compare_and_swap (after P1 releases the lock) happens before P2 has a chance to call compare_and_swap, it can acquire the lock again, and this can happen over and over again, potentially resulting in P2 never acquiring the lock.
