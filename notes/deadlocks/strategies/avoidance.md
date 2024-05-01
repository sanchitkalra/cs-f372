# Deadlock Avoidance

- system needs information a-priori.
- process declares max number of resources of each kind it may need => essentially declaring an upper bound on the number of resources that will ever be required. cannot request more than this declared upper bound.
- Resource Allocation State: number of available and allocated resources, and max demand of processes. total number of resources of each kind = available + allocated = fixed const.
- deadlock avoidance algo -> examine resource allocation state, and ensure no circular wait. if request is ok (no chance of deadlock happening) -> allocate, else (there is a chance, deadlock can happen) ask process to wait.

## Safe state

- safe state => there is no chance of deadlock in the system.
- safe state => sequence of all processes in the system such that for each process, it's resource demands can be met with the resources available + the resource held by the prev process.
  - technically demands for Pi can be met with available + resources held by Pj (j < i)
- if resources are not available, Pi waits. if it executes, it can obtain the resources it needs and return them on termination, and they are made available to the process Pi+1.
- avoidance => never let system go into an unsafe state.

## Avoidance Algorithms

- since instance resources -> resource allocation graphs
- multiple instances of resources -> Banker's algorithm

### Avoidance - Resource Allocation Graphs Algorithm

- claim edge: this is a new kind of edge from a process to a resource (a dashed edge), in the sense that the said process _may_ claim the resource in the future.
- it turns into a request edge when _the actual request happens_
- it then further turns into an assignment edge (provided the resource is available) when the _request is granted_
- when the resource is released, the edge turns into a claim edge again
- resource claims must be declared a-priori
- request will only be granted if changing the request edge to a assignment edge does not produce a cycle in the graph. if it does, the process will wait

### Avoidance - Banker's algorithm

- when there are multiple instances of a resources
- demands declared a-priori
- process _may_ have to wait
- process must return the resources in a finite amount of time
- _see datastructure details from slides_
- _see lec once again -> imp intricacies topic_

#### Safety Algo

- _ref to slides, only writing extras here_
- if finish[i] = true => process i incorporated in safe sequence, and vice versa. finish is of size N, the number of processes
- 2b -> i have enough resources to satisfy process i's needs, and move on
- if condition of step 2 holds -> we can do step 3
  - => process can be included in safe sequence
  - => process will give back everything when done
  - currently has -> allocation[i], needs -> need[i], and need[i] + allocation[i] = max[i]
  - logic behind doing work[i] = work[i] + allocation[i] is as follows:
    - when resources are allocated -> work[i] = work[i] - need[i], and allocation[i] = allocation[i] + need[i]
    - and when process returns the resources, it returns max[i]
    - so we can write in one step that work[i] = work[i] + allocation[i] (took need, returned max, positive sum difference = max - need = allocation)
  - loop back to step 2
- if no process satisfies both conditions of 2 for one of two reasons
  - each process has been included in the safe sequence (step 4)
  - a certain process' needs in the future can't be satisfied -> here we are in an unsafe state, but **not** necessarily deadlocked

#### Resource Request Algorithm

- decide whether to grant the resource request now, or delay it for later so as to not get into problems
- _see slides, algorithm steps_
- key point: when a process requests resources, do the following ops
  - available = available - request
  - alloc = alloc + request
  - need = need - request
  - and then apply banker's algo and check if a safe sequence is possible, if so, then it is okay to grant that request, else we deny that request
