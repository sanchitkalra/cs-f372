# Context Switch

- sys must save the state of the old process and load the state of the next process using a context switch.
- the process being taken off the executing state and put into the wait queue, it's context, ie, content of it's PCB needs to be stored for when the process will be resumed, it can be resumed from the point where it left off.
- context switch can be between processes already executing or between already executing and new processes.
- context switching time is overhead. each context switch costs CPU cycles which are getting wasted.
