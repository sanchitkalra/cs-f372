# Shared Memory
- an area of memory shared by multiple processes
- one process creates shared memory and one attaches itself to the shared memory
- shared memory is in the addr space of the producer
- syncronisation is needed as no two processes should be writing to memory at the same time

## Ways to implement shared memory

### 1. schmget()
- obtain identifier of existing shared mem segment or create a new one
- syntax: `int schmget(key_t key, size_t size, int shmflg);`
- parmas:
    - key: unique value (identifier) associated with the segment. if a segment already exists with a particular key, it will be returned to the calling process, and if it does not exist, a new one will be created. this key is generated using the function `key_t ftok(const char* path, int id)`.
        - `path`: could be the full path to a file, commonly we specify the name of the source file where this function is being called, or another text file.
        - `id`: we pass in a character value here which gets automatically typecasted to int.
        - if instead of a key, the argument `IPC_PRIVATE` is passed, a private memory segment inaccessible by other processes will be created.
    - size: size of the segment of shared memory. size that is requested here will be rounded up because memory is allocated in pages and not raw bytes, whatever is excess is wasted.
    - shmflg: specify read/write permissions with flags such as IPC_CREAT (if any). on linux systems, read - 4, write - 2, execute - 1. on linux there are 3 kinds of user - u(user), g(group), o(other). for the creator to give a particular user some permissions, set the number as the sum of permission values. for example, user has all perms (sum = 4 + 2 + 1 = 6), and only read to group and others (sum = 4 + 0 + 0 = 4), and now the value will be `0644`. the permission will now be written as `0644 | IPC_CREAT`.
         - if the last argument is passed as `<perms> | IPC_CREAT | IPC_EXCL`, and a segment does not exist with the given key, it will be created, and if it does, the shmget call will fail.
        - ?? see lec again for perms thingy and read up on how perms associated with unix shared mem work
- a successful call returns a valid shared memory identifier, else -1.
- [man page](https://man7.org/linux/man-pages/man2/shmget.2.html)

### shmat()
- used to attach to a shared memory segment. even the creator needs to explicitly attach to the shared memory segment.
- syntax: `void* shmat(int shmid, const void* shmaddr, int shmflg);`
    - shmid: identifier for the shared memory. this is the shmid that was returned by the schmget call.
    - shmaddr: the location where the shared memory segment will be attached. usually this is NOT passed, becuase we don't really know the whole address space details and we let the kernel figure out where the address space will be attached. we usually just pass NULL in here. (a void pointer is a pointer that can have any kind of data type with an appropriate cast. calloc and malloc also return void pointers because we can store any kind of data returned by these calls.)
    - shmflag: specify flags for the calling process
- if the call succeds, a void pointer to the segment's start addr will be returned, else -1 (with the typecasted equivalent: `(void*) - 1`).
- we can then access the memory with simple pointers.
- we declare the pointer as `type name*;` and use this as the return for the shmat call.
- [man page](https://man7.org/linux/man-pages/man3/shmat.3p.html)

### shmdt()
- used to detach a shared memory segment from the addr space of the calling process.
- syntax: `int shmdt(const void* shmaddr);`
    - shmaddr: the addr where the shared memory segment is located. this comes from the shmaddr in the `shmat()` call.
- on successful detach the `n_attach` counter in the `shmid_ds buf` decremented by 1.
- if successful, the segment is detached from the calling process and 0 is returned, else -1.

### shmctl()
- perform control ops on shared memory segment (commonly used to delete)
- syntax: `int shmctl(int shmid, int cmd, struct shmid_ds* buf)`
    - shmid: identifier
    - cmd: specifies control op to be performed like changing it's properties or deleting the segment. for eg, `IPC_RMID` for deleting the shared memory segment.
        - IPC_STAT: copies all data present in the kernel data structure associated with shmid. segment size: buf.shm_segsz, no of processes attached: buf.shm_nattach. see man page for more.
        - IPC_RMID: in this case, we can let the last argument (`buf`) of the shmctl can be 0.
        - ?? (see rec)
    - buf: pointer to a shmid_ds data structure. we can use the field of this data structure to perform various control functions on the shared memory segement. it is a special data structure maintained by the kernel for each shared memory segment within it's addressing space. defined in `sys/shm.h` header file.
- [man page](https://man7.org/linux/man-pages/man2/shmctl.2.html)
