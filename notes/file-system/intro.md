# File Systems

- attributes: name, identifier (non human readable), type (pdf, docx, etc), location (ptr to location), size, protection (read, write, execution), time date user identification
- information about files stored in kept in directory structure maintained on disk
- directory itself -> name and an identifier
- all file ops are essentially sys calls
- write -> delete prev content and write new content; append -> add to _already existing_ content of a file
- OS maintains an open file table; each file op assumes that the file is open and an entry is made in this table which is used an index to reference to the file
  - fclose -> remove entry from open file table
- file access methods
  - sequential access: access different blocks of file sequentially; can't go back. `read_next()` (go from x to x + 1 block), `write_next()`, `reset()` (go back to first block)
  - direct/relative access: fixed length logical record (record lengths can vary by files). n is relative block number `read(n)`, `write(n)`, `position_file(n)` (position the file pointer at a certain location)

## Directory

- organise files in with some semantic relationship; logical grouping of files
- let's us easily name files
- each file in a directory should have a unique name
- directory is handled as a _special file_
  - tag: tells us if a file is a directory or a normal file

### Single level directory

- single directory for all users
- file names should be unique
- not scalable
- entry for each file present; entry -> pointer to the actual file location

### Two level directory

- each user has their own directory but no further directories for each user
- master file directory (MFD) -> track all users in multiuser env; each user has an entry in the MFD. login -> search for that user's file directory and load it
- each user has their own user file directory
- creation and deletion of user directory requires admin priviledges
- no concept of sharing of files

### Tree structure directory

- any level of nesting of directories
- even better logical grouping of files
- there is always a present working directory
- deleting files is easy
- using permissions, some sharing is possible
- absolute path: path from the root dir
- relative path: path relative to the PWD

### Acyclic Graph Directory

- sharing: multiple people work on the same copy of the file
- file shared among two dir
- file can now have two seperate absolute paths
  - each user sees the path relative to their directory
- symbolic link: the file physically exists in one memory location only, but for one path it goes to the actual file, for the other it goes through a link
- accessing the file using the symbolic link is called resolving the link
- when the actual source file is deleted, and it is attempted to be accessed by the symbolic link, it is an illegal operation and the pointer is essentially now a dangling pointer. they are deleted lazyly, only when it is attempted to be accessed, it is deleted
- OS needs to keep track of the number of links associated with a file, and it regains the disk space when the number of links are removed
- file system should not have a cycling, else we will keep traversing a portion of the file system again and again
