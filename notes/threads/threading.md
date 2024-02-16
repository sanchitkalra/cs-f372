# Threads

- Thread are parallel execution paths within the context of the creator process, and the basic unit of CPU utilisation.
- *A thread is a light weight process.* => a process requires a complete address space even if it is a copy of the parent process but a thread shares the address space with the parent. secondly processes' context switch and address spaces change and that overhead is also added. on the other hand, thread context switching needs no change in address space and this consumes less resources. this makes threads more attractive. all this is valid only if threads are of the same process otherwise it is same as process contexting switching.
- Threads share the heap, the code, OS resources like the list of open files etc. but they don't share the stack of the parent process, and program counter.
- Threads are what we schedule on the CPU on a specific processor. they have a thread id, program counter, registers, and stack.
- Processes are expensive in creation and communication cost, and threads are a light weight alternative and this can be used to increase efficiency.
- Each thread can be performing their own tasks.
- Every process is by default single threaded.

## Benefits of Multithreading

- Lightweight parallelism.
- Responsiveness. allows continued execution even if a part of the process is blocked and this is useful for responsive/interactive user interfaces.
- Economic in terms of resource sharing. threads share resources of the process, and this is easier than shared memory or message passing. also economical in terms of context switching, thread context switching more economical than process context switching.
- Scalability. multithreaded process can take advantage of multiprocessor architectures.

## Concurrency vs Parallelism

- **Parallelism**: perform more than one task at the same time. cannot be achieved on a single core CPU.
- **Concurrency**: more than one task is making progress. single core systems can provide concurrency but not parallelism. in this mode the processor quickly switches between tasks very quickly allowing all of them to make progress. to the user it seems all of them are running simultaneously which is not true.
- Modern systems execute tasks in combination of both concurrency and parallelism.
- We use GAN charts to represent process execution order.

### Types of Parallelism

- Data Parallelism. distribute subsets of data across multiple cores, same operation on each subset
- Task Parallelism. distributes tasks/threads across cores, each thread performs a unique operation, and the threads may or may not be operating on the same data.


## Thread control block (TCB)

- a kernel data structure which contains threads specific information needed to manage it.
    - thread identifier
    - stack ptr
    - program counter
    - state of the thread
    - thread's register values
    - pointer to the PCB of the process that created it
    - pointers to other threads created by this threads
- threads cannot be seen in `ps -l`. Say a process creates 5 threads, we will see 5 instances of the process in `ps -l` but not with individual thread identifiers.
- threads in C run automatically as soon as the thread is created. threads in Java have a specific function that needs to be called to started the execution of that thread.
- types of threads:
    - user threads
    - kernel threads

## Threading models
