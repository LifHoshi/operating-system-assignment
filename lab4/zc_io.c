#include "zc_io.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>

// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file {
    // Insert the fields you need here.
    void *ptr;
    int fd;
    size_t offset;
    size_t size;
    sem_t mutex;
    int readers;
    sem_t b_mutex;
    /* Some suggested fields :
        - pointer to the virtual memory space
        - offset from the start of the virtual memory
        - total size of the file
        - file descriptor to the opened file
        - mutex for access to the memory space and number of readers
    */
};

/**************
 * Exercise 1 *
 **************/

zc_file* zc_open(const char* path) {
    struct stat buf;
    zc_file *file = (zc_file*) malloc(sizeof(zc_file));
    int fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    fstat(fd, &buf);
    off_t size = buf.st_size;
    //sem_t mutex = (sem_t) malloc(sizeof(sem_t));
    //sem_t b_mutex = (sem_t) malloc(sizeof(sem_t));

    if (fd >= 0) {
        void *ptr;
        if (size == 0) {
            ptr = mmap(NULL, 1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            size = 1;
        } else {
            ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        }
        file->ptr = ptr;
        file->size = size;
        file->offset = 0;
        file->fd = fd;
        file->readers = 0;
        sem_init(&file->mutex, 0, 1);
        sem_init(&file->b_mutex, 0, 1);
        return file;
        }
    return NULL;
}

int zc_close(zc_file* file) {
    // To implement
    munmap(file->ptr, file->size);
    close(file->fd);
    free(file);
    return -1;
}

const char* zc_read_start(zc_file* file, size_t* size) {
    // To implement
    sem_wait(&file->mutex);
    file->readers++;
    if (file->readers == 1) {
        sem_wait(&file->b_mutex);
    }
    sem_post(&file->mutex);

    if (file->offset + *size > file->size) {
        *size = file->size - file->offset;
    } 
    size_t res = file->offset;
    file->offset += *size;
    return (char*) file->ptr + res;
}

void zc_read_end(zc_file* file) {
    // To implement
    sem_wait(&file->mutex);
    file->readers--;
    if (file->readers == 0) {
        sem_post(&file->b_mutex);
    }
    sem_post(&file->mutex);
}

char* zc_write_start(zc_file* file, size_t size) {
    // To implement
    sem_wait(&file->b_mutex);
    size_t prev = file->offset;
    if (file->offset + size > file->size) {
        if (file->size == 0) {
            file->ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file->fd, 0);
            file->size = size;
            ftruncate(file->fd, size);
        }
        else {
            size_t new_size = file->offset + size;
            file->ptr = mremap(file->ptr, file->size, new_size, MREMAP_MAYMOVE);
            file->size = new_size;
            ftruncate(file -> fd, new_size);
        }
    }
    file->offset += size;
    return (char*) file->ptr + prev;
}

void zc_write_end(zc_file* file) {
    msync(file->ptr, file->size, MS_SYNC);
    sem_post(&file->b_mutex);
}

/**************
 * Exercise 2 *
 **************/

off_t zc_lseek(zc_file* file, long offset, int whence) {
    // To implement
    sem_wait(&file->b_mutex);
    if (whence == SEEK_SET) {
        if (offset < 0) {
            sem_post(&file->b_mutex);
            return -1;
        }
        file->offset = 0;
    } else if (whence == SEEK_CUR) {
        file->offset = file->offset;
    } else if (whence == SEEK_END) {
        file->offset = file->size;
    }

    file->offset += offset;

    if ((long)file->offset < 0) {
        sem_post(&file->b_mutex);
        return -1;
    }
    sem_post(&file->b_mutex);
    return file->offset;
}

/**************
 * Exercise 3 *
 **************/

int zc_copyfile(const char* source, const char* dest) {
    // To implement
    zc_file *file1 = zc_open(source);
    zc_file *file2 = zc_open(dest);

    int file1_fd = file1->fd;
    int file2_fd = file2->fd;
    size_t size = file1->size;
    ssize_t res = copy_file_range(file1_fd, NULL, file2_fd, NULL, size, 0);
    ftruncate(file2->fd, file1->size);
    if (res == -1) {
        return -1;
    }
    return 0;
}

/**************
 * Bonus Exercise *
 **************/

const char* zc_read_offset(zc_file* file, size_t* size, long offset) {
    // To implement
    return NULL;
}

char* zc_write_offset(zc_file* file, size_t size, long offset) {
    // To implement
    return NULL;
}