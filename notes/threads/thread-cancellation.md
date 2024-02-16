# Thread Cancellation

- terminate a thread before it's execution has finished
- the thread to be cancelled is called the target thread
- two approaches:
    - async cancellation: one thread terminates another thread instantly
    - deferred cancellation: allows the target thread to periodically check if it should be cancelled; target thread can then terminate itself in an orderly fashion (free up allocated resources. examples: close file descriptors, network conn etc. the other thing is say the thread was updating a data structure and it couldn't complete it's update completely and we loose atomicity). the target thread keeps a track if it is receiving a termination request, it will terminate by performing housekeeping tasks and then exit gracefully
- syntax: `pthread_cancel(pthread_t tid)`
- invoking thread cancellation only **requests cancellation**, actual cancellation depends on how the target thread is setup to handle a cancellation request

## Deferred Cancellation

- cancellation occurs only when thread reaches a cancellation point (establish this by calling pthread_testcancel)
- pthread_testcancel basically checks if there are any cancellation requests and if there are any, it invokes the cleanup handler when it has reached the cancellation point
