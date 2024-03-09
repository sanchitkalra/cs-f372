# Peterson Solution

- two processes only
- software based solution, requires no hardware support
- assumption: load & store machine level instructions are atomic and cannot be interrupted
- the two processes share two variables

```java
int turn; // represents whose turn it is to enter the critical section
boolean flag[2]; // bool for each process; indicate if a process is ready to enter the critical section
```

_turn is what we get, flag is what we want_

- pseudo code

```java
// process p_i
do {
    flag[i] = true; // p_i shows it's inclination to enter the critical section
    turn  = j; // p_i gives j a fair chance to enter the critical section
    while (flag[j] && turn == j); // p_i waits for j's critical section to get over

    // now p_i is free to enter the critical section and do what it wants
    {
        // critical section
    }

    flag[i] = false; // finally p_i shows that for the time being it is done with the critical section

    // remainder section
} while (true);
```

_similar code for process pj as well, replace i with j and j with i_

- the execution happens as follows:

  - say p_i executes first, it sets flag[i] = true, and turn = j
  - now the condition `(flag[j] && turn == j)` evaluates to false as flag[j] is false and turn = j, and p_i breaks out of the while loop
  - p_i enters the critical section
  - now while p_i is executing in the critical section, p_j will set flag[j] = true and turn = i
  - now the loop condition `(flag[i] && turn == i)` evaluates to true and p_j is stuck in the loop and keeps waiting => p_j is prevented from entering the critical section
  - now once p_i exits the critical section, p_i sets flag[i] = false and the loop condition inside p_j evaluates to false and p_j breaks out of the while loop and enters the critical section

- checking 3 requirements
  1. mutual exclusion. the execution pattern shows us that both will not be accessing the critical section simultaneously and mutual exclusion is followed
  2. progress. say p_i wants to enter the critical section & p_j doesn't. since p_j doesn't want to enter the critical section, flag[j] would be false. p_i will set turn = j. now the while loop condition in p_i is not satisfied, and the p_i breaks out of the while loop and p_i is able to enter the critical section
  3. bounded waiting. if process preemption is there and this particular process execution order happens: p_i sets flag[i] to false -> p_j sets flag[j] = true and turn = i -> (p_j preempts) -> p_i sets flag[i] = true and turn = j. p_i is now stuck in the while loop -> (p_i preempts) -> while loop condition in p_j is now false and p_j gets a chance to enter the critical section again.
