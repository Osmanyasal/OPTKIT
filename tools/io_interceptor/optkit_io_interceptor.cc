#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

// Function pointers to hold the original functions
static int (*real_open)(const char *pathname, int flags, ...) = NULL;
static int (*real_open64)(const char *pathname, int flags, ...) = NULL;
static int (*real_openat)(int dirfd, const char *pathname, int flags, ...) = NULL;
static int (*real_close)(int fd) = NULL;
static FILE *(*real_fopen)(const char *pathname, const char *mode) = NULL;
static FILE *(*real_fopen64)(const char *pathname, const char *mode) = NULL;
static int (*real_fclose)(FILE *stream) = NULL;

// Intercepted open function
int open(const char *pathname, int flags, ...)
{
    if (!real_open)
    {
        real_open = (int (*)(const char *, int, ...))dlsym(RTLD_NEXT, "open");
        if (!real_open)
        {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
            return -1;
        }
    }

    printf("File open detected: %s\n", pathname);
    // Perform your operations here

    va_list args;
    va_start(args, flags);
    int mode = 0;
    if (flags & O_CREAT)
    {
        mode = va_arg(args, int);
    }
    va_end(args);

    return real_open(pathname, flags, mode);
}

// Intercepted open64 function
int open64(const char *pathname, int flags, ...)
{
    if (!real_open64)
    {
        real_open64 = (int (*)(const char *, int, ...))dlsym(RTLD_NEXT, "open64");
        if (!real_open64)
        {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
            return -1;
        }
    }

    printf("File open64 detected: %s\n", pathname);
    // Perform your operations here

    va_list args;
    va_start(args, flags);
    int mode = 0;
    if (flags & O_CREAT)
    {
        mode = va_arg(args, int);
    }
    va_end(args);

    return real_open64(pathname, flags, mode);
}

// Intercepted openat function
int openat(int dirfd, const char *pathname, int flags, ...)
{
    if (!real_openat)
    {
        real_openat = (int (*)(int, const char *, int, ...))dlsym(RTLD_NEXT, "openat");
        if (!real_openat)
        {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
            return -1;
        }
    }

    printf("File openat detected: %s\n", pathname);
    // Perform your operations here

    va_list args;
    va_start(args, flags);
    int mode = 0;
    if (flags & O_CREAT)
    {
        mode = va_arg(args, int);
    }
    va_end(args);

    return real_openat(dirfd, pathname, flags, mode);
}

// Intercepted close function
int close(int fd)
{
    if (!real_close)
    {
        real_close = (int (*)(int))dlsym(RTLD_NEXT, "close");
        if (!real_close)
        {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
            return -1;
        }
    }

    printf("File close detected: %d\n", fd);
    // Perform your operations here

    return real_close(fd);
}

// Intercepted fopen function
FILE *fopen(const char *pathname, const char *mode)
{
    if (!real_fopen)
    {
        real_fopen = (FILE * (*)(const char *, const char *)) dlsym(RTLD_NEXT, "fopen");
        if (!real_fopen)
        {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
            return NULL;
        }
    }

    printf("File fopen detected: %s\n", pathname);
    // Perform your operations here

    return real_fopen(pathname, mode);
}

// Intercepted fopen64 function
FILE *fopen64(const char *pathname, const char *mode)
{
    if (!real_fopen64)
    {
        real_fopen64 = (FILE * (*)(const char *, const char *)) dlsym(RTLD_NEXT, "fopen64");
        if (!real_fopen64)
        {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
            return NULL;
        }
    }

    printf("File fopen64 detected: %s\n", pathname);
    // Perform your operations here

    return real_fopen64(pathname, mode);
}

// Intercepted fclose function
int fclose(FILE *stream)
{
    if (!real_fclose)
    {
        real_fclose = (int (*)(FILE *))dlsym(RTLD_NEXT, "fclose");
        if (!real_fclose)
        {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
            return EOF;
        }
    }

    printf("File fclose detected\n");
    // Perform your operations here

    return real_fclose(stream);
}
