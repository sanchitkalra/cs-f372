# Scheduling

- multiprogamming => max CPU utilisation
- CPU-I/O burst cycles: process exec => cycle of CPU exec and I/O wait
    - CPU burst time: the time a process actually uses the CPU for performing some computation
    - I/O burst time: the time a process waits for an I/O device to perform some kind of action
- if computation > I/O => computationally intensive process, else an I/O intensive process
- too many of either of them is bad. too many computationally intensive processes => device queue will be relatively empty. too many I/O intensive processes => ready queue will be relatively empty
- frequency of processes with short CPU and I/O burst cycles > frequncy of processes with long CPU-I/O cycles

## CPU Scheduler

- selects from among the processes in ready queue for CPU allocation
- state transitions based on which decisions are made:
    - running to waiting => CPU is now free, the CPU can be allocated to another process. process may moved to the device queue. this is voluntary, by for example, calling an I/O function
    - running to ready => when a proces' CPU time has finished and it is preemptive (it has been taken forcibly from the CPU in the interest of fairness)
    - waiting to ready => the I/O operation is over and it is now ready for execution. device queue -> ready queue
    - termination =>
- preemptive in case 2 & 3.
- non preemitve => once a process is allocated the CPU, it retains the CPU until termination or switching or waiting state


## Dispatcher

- gives control of the CPU to the process selected by the CPU (short-term) scheduler. this involves:
    - switching the context
    - switching to user mode
    - jumping to the proper location in the user program to restart that program. if the process never ran, start it from the beginning
- dispatch latency: time for the dispatcher to stop one process and start another runnning

## Scheduling Criterion

1. CPU utilisation: keep the CPU as busy as possible
2. Throughput: number of process that complete their execution per unit of time
