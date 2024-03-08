# Pthreads

- may be provided as a user level or kernel level
- a POSIX std API for thread creation and synchronization
- it is a specification, not an implementation
- common in UNIX OSes (solaris, linux, mac)
- they use data types that are themselves POSIX compliant, not general data types like int or float because they may vary across systems
- in UNIX based OSes it is called the `pthread_t` library in the `pthread.h` header file
- the thread IDs make sense only at the process level, not the system level
- thread identifiers can be recycled
- on linux, we need to pass the `-pthread` option to link the pthread library with our code. command becomes: `gcc file.c -o exec-name -pthread`
- any variables declared in the thread function are available only in the scope of the thread
- when we're creating multiple threads, we can just store tids in an array and call required functions by looping over this array

## pthread_attr_t

- defines the various attributes of a thread, we can use this to set attributes of a thread as well. attributes can be init using the function `int pthread_attr_init(pthread_attr_t* attr)` and we use this function before creating the thread because threads upon creation start running instantly. this fn takes only 1 argument, a pointer to a structure of the type pthread_attr_t. if this call succeeds, it returns 0, else an error code.

## pthread_create

- used to create threads.
- syntax: `int pthread_create(pthread_t* id, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg)`
  - id:
  - attr: pointer to a structure that holds attributes initialised via the pthread_attr_int function
  - (fn): the argument holds a ref to the function that needs to be executed. this tells the signature of the function.
    - (\*start_routine): this is a pointer to the function with the name start_routine. this name is only a placeholder value
    - the last (void\*): this is a single argument that can be passed to the thread function (start_routine). if we need to pass multiple arguments, we can pass it as a structure
    - the first void\*: this is the return type of the function (start_routine)
  - arg: the argument that is to be passed to the function start_routine. we use this incase we are specifying only the name of the function in the 3rd argument because we will specify it as `fn` and not `fn(arg)`.
- if the call succeeds, the returns 0, else a non-zero error code.

## pthread_equal

- used to compare if two thread ids are referring to the same thread.
- we use this function because pthread_t are library specific data types to ensure compatibility across POSIX systems and the typical C `==` operator should not be used, and instead this function should be used.
- syntax: `int pthread_equal(pthread_t tid1, pthread_t tid2)`
- if they're equal, 0 is returned, else a non zero value is returned

## pthread_exit

-
- syntax: `void pthread_exit(void* ptr)`

## pthread_join

- creator thread wait for threads it created to finish
- syntax: `void pthread_join(pthread_t tid, void** retval)`
  - tid: the identifier of the thread for which join is being invoked and the parent is waiting for
  - retval: the return value of the thread with identifier tid. if we don't care for this value we can just send NULL here
- the function call will return as soon as the thread with tid finishes
- if the parent waits for it's children threads it is called syncronous threading, else it is called async threading
- if the parent thread dies, irrespective of this function being used, all threads will exit because threads only live in the context of the parent thread

## pthread_self

- helps to figure of the identifier of the thread calling this function => return the caller's thread id
- syntax: `pthread_t pthread_self()`
- this function can never fail because if this function is being executed, it is in a thread
