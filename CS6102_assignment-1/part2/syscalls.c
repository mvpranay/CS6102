// syscalls.c - Minimal syscalls stubs for newlib
#include <sys/stat.h>
#include <errno.h>

#undef errno
extern int errno;

// Increase heap pointer
char *heap_end = 0;
extern unsigned long _heap_start;
extern unsigned long _heap_end;

void *_sbrk(int incr) {
    char *prev_heap_end;
    
    if (heap_end == 0) {
        heap_end = (char *)&_heap_start;
    }
    
    prev_heap_end = heap_end;
    
    if (heap_end + incr > (char *)&_heap_end) {
        errno = ENOMEM;
        return (void *)-1;
    }
    
    heap_end += incr;
    return (void *)prev_heap_end;
}

int _close(int file) {
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _read(int file, char *ptr, int len) {
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

// UART output for QEMU
#define UART0_BASE 0x4000C000
#define UART0_DR   (*(volatile unsigned int*)(UART0_BASE + 0x000))
#define UART0_FR   (*(volatile unsigned int*)(UART0_BASE + 0x018))

int _write(int file, char *ptr, int len) {
    int i;
    (void)file;
    
    for (i = 0; i < len; i++) {
        // Wait until TX FIFO not full
        while (UART0_FR & (1 << 5));
        
        // Write character
        if (ptr[i] == '\n') {
            UART0_DR = '\r';
            while (UART0_FR & (1 << 5));
        }
        UART0_DR = ptr[i];
    }
    
    return len;
}

void _exit(int status) {
    (void)status;
    while (1);
}

int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

int _getpid(void) {
    return 1;
}