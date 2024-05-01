# Deadlocks

## System model

- System has resources of kind Ri (with each having Wi instances)
- each process uses resources as:
  - request
  - use
  - release
- it is imperative to see how resource requests are approved; better to make processes wait, rather than get into a deadlock
- difference between starvation and deadlocks is that when a process starves, others can continue, but when a set of processes enter a deadlock, no one continues

## Conditions for Deadlock

1. Mutual exclusion: we're giving process access to a resource, and asking others to wait, this can trigger a deadlock
2. Hold and wait: a process holding a resouce is waiting to acquire more resources held by other processes, and in essence, what this processes is holding is blocked for access by other processes, and a dependency chain may form
3. No preemption: resource can be released only voluntarily by a process, either by itself, or when it terminates
4. Circular wait: there exists a set (P0, P1, ..., Pn) of waiting processes such that P0 is waiting for a resource that is held by P1, P1 is waiting for a resource that is held by P2, ..., Pn-1 is waiting for a resource that is held by Pn, and Pn is waiting for a resource that is held by P0. This happens in effect because of hold & wait and no preemption.

## Resource Allocation Graph

- nodes: processes represented by cicles (written inside the circle), and resources as rectangles (resource written outside the rectangle; number of instances of that resource represented by dots inside the rectangle)
- edges:
  1. request edge: directed edge from Pi to Rj (Pi requests access to Rj) (arrow head ends outside the rect.)
  2. assignment edge: directed edge from Rj to Pi (Rj allocated to Pi) (arrow tail begins from a specific instance dot, and head at process)
- assumption: all instances of a resource are considered equivalent => any instance of a resource can be allocated to a requesting process
- if a process does not have an outgoing end -> it is an active process and is not blocked (=> it is not asking for something that is not readily available) & vice versa
- if resources have only one instance -> looking for a cycle in the graph is enough to characterise a deadlock
- multiple instance => cycle does not mean a deadlock necessarily
- if a resource has multiple instances, and those instances are held by active process and there is a cycle **it does not neccessarily mean a deadlock** as at sme point the active process will end or release those resources as they are not waiting to acquire any other resources and our waiting process will get the resources they are blocked on and the cycle will go away => deadlock is not present. _deadlock -> cycle cannot be eliminated unless processes are forcefully terminated_

## Inferring Deadlock

- Look for a knot (collection of vertex and edges such that every vertex in the knot has outgoing edges that terminate at other vertices in the knot, and it is immpossible to leave the knot by following the edges of the graph)
- knot can be said as a collection of cycles

## Handling Deadlocks

- 3 strategies

  1. Proactive approach: Ensure system never enters a deadlock.

  - [Deadlock prevention](./strategies/prevention.md)
  - [Deadlock avoidance](./strategies/avoidance.md)

  2. Reactive approach: Allow system to enter a deadlock but also recover from it. [Deadlock Detection](./strategies/detection.md)
  3. Ignorance: Ignore deadlock, assume it'll never happen
