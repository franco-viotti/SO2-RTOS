#include <sys/stat.h>
#include <errno.h>

// Implementaciones mínimas necesarias para que funcione stdlib
void *_sbrk(int incr) { return (void *)-1; }
int _write(int file, char *ptr, int len) { return 0; }
int _close(int file) { return -1; }
int _fstat(int file, struct stat *st) { return -1; }
int _isatty(int file) { return 1; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _read(int file, char *ptr, int len) { return 0; }
int _kill(int pid, int sig) { errno = EINVAL; return -1; }
int _getpid(void) { return 1; }
int _exit(int status) { while(1); return 0; }