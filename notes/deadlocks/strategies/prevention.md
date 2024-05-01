# Deadlock Prevention

- goal: make sure at least one of the four conditions for a deadlock never holds in the way resources are allocated
- mutual exclusion: not for sharable resources, only for non-sharable resources
- hold & wait: when a process asks for resources -> it should not hold anything else
  - acquire everything before starting execution
  - acquire only when process holds anything
  - low resources utilisation, starvation possible
- no preemption: release all old resources when asking for new ones that cannot be immediately allocated, and add released resources to list of requested resources -> start only once the entire list can be allocated together
- circular wait: total ordering on resource types. each process should request resources in increasing order of enumeration. define 1:1 fn, f:R->N. If process already holds resources of Ri, it can only acquire Rj type resource if f(Rj) > f(Ri). if a process acquires some resource with f-value > a resource it needs now, the process must release all it's resources with f-value > f-value of required resource and re-acquire them in the correct order.
