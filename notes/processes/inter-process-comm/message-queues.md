# Message Queues

- async comm. messages placed on the queue are stored until recepient retrieves them.
- steps:
    1. create/connect to a message queue using `msgget()`
    2. write to the msg queue using `msgsnd()`
    3. read from the queue using `msgrcv()`. once a msg is read, it is taken out of the msg queue.
    4. perform control ops using `msgctl()`. a common op is to delete msg queue.
- functions in the `<sys/msg.h>` header file.
- all messages in linux have two parts, a msg type (strictly >= 0) and the message payload.
- msg types can be used when there are multiple consumer and the producer can choose which process processes which kinds of messages. a receiver/consumer will process messages only meant for it by using the message type. similar to multiplexing.
- messages may not be retrieved in FIFO order overall, but for each message type they will be taken out in FIFO order.

## msgget()

- create/connect to a message queue.
- syntax: `int msgget(key_t key, int msgflg)`
    - key: a unique value associated with the msg queue. if a queue with the given key does not exist, it will be created provided the IPC_CREAT flag is specified. if instead of a key, IPC_PRIVATE is passed, a private msg queue is created. ?? check if two calls to msgget are made with IPC_PRIVATE within the same process, are two msg queues created or not. at least on my machine, two calls to msgget() with IPC_PRIVATE returns two different identifiers are returned.
    - msgflg: set flags to control behaviour of the msg queue. eg. IPC_CREAT, IPC_EXCL. can also be 0. if both IPC_CREAT and IPC_EXCL flags are provided, and the queue already exists, the call will fail.
- on success, a msg queue identifier is returned, and -1 on failure.

## msgsnd()

- send messages to a msg queue. to do this, the sender must have write permissions.
- syntax: `int msgsnd(int msquid, const void* msgp, size_t msgsz, int msgflg)`
    - msquid: an identifier for the message queue.
    - msgp: points to a user defined data structure containing two fields, msg_type and the payload. this payload can be any arbitary type.
    - msgsz: the size of the message's payload in bytes. the max size of a msg on linux is 4056 bytes.
- msgsnd is a blocking system call by default. if the msg queue has no space available, and a call to msgsnd is called, the process will get blocked until sufficient space is available in the queue, unless the IPC_NOWAIT flag is passed.

## msgrcv()
- used to receive/remove a message from a specified message queue and of a specific message type.
- syntax: `ssize_t msgrcv(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg)`
    - msgtyp: used to specify the type of message to be read. specified 0 => first message on the queue is read irrespective of specified types. if the value < 0 => the first message with type <= |msgtyp| will be taken out.
    - msgsz: size of the actual payload in the message
- `ssize_t` is a signed size. even tho size will always be >= 0, to signal an error condition by way of returning a negative number, the type size_t is modified to ssize_t.
- if the call succeeds, the call will return the size of the message received else -1.
- this call is also blocking unless the IPC_NOWAIT is passed as a flag.
- if the flag MSG_EXCEPT is set with the value of msgtyp > 0, it will take the first message with message type != msgtyp.

## msgctl()
-
