enum OpenFlag:
    O_RDONLY    = 0      // open for reading only
    O_WRONLY    = 1      // open for writing only
    O_RDWR      = 2      // open for reading and writing
    O_CREAT     = 64     // create file if it does not exist
    O_EXCL      = 128    // error if O_CREAT and the file exists
    O_NOCTTY    = 256    // do not assign controlling terminal
    O_TRUNC     = 512    // truncate file to zero length
    O_APPEND    = 1024   // append on each write
    O_NONBLOCK  = 2048   // non-blocking mode
    O_DSYNC     = 4096   // synchronous I/O data integrity
    O_SYNC      = 1052672 // synchronous I/O file integrity
    O_RSYNC     = 1052672 // synchronous reads
    O_DIRECTORY = 65536   // fail if not a directory
    O_NOFOLLOW  = 131072  // do not follow symbolic links
    O_CLOEXEC   = 524288  // set close-on-exec
:



native "C":
    fn open(char *name,i32 flags,i32 mode)->i32
    fn creat(char *name,i32 mode)->i32
    fn read(i32 fd,char *buf,u64 buflen)->i64
    fn write(i32 fd,char *buf,u64 buflen)->i64
    fn close(i32 fd)->i32
    fn puts(char *str)->i32
    fn malloc(u64 size)->void *
    fn free(void *ptr)->void
:


fn main()->i32:
    i32 fd = open("test.rs",OpenFlag.O_RDONLY,0)
    char *buf = malloc(1000)
    read(fd,buf,1000)
    puts(buf)
    free(buf)
    return 0
:






