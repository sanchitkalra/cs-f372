# Deadlock Detection

- allow a sys to enter a deadlock state
- goal: detect & resolve

## Single instance resource graphs

- construct a new graph called a wait-for graph from the existing resource allocation graph. this is a simplified version of the resource allocation graph.
- say P1 is requesting R1 which is held by P2, then connect P1 to P2.
- now if there are cycles => deadlock is present.
- if a process is waiting for a deadlocked process, in effect, it is also deadlocked

## Recovery

### Process termination

- abort all process altogether or abort one at a time till deadlock is removed

### Resource Preemption

- Preempt some resources from process, and give them to other processes until deadlock is eliminated
