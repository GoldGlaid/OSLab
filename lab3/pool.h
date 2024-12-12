#ifndef POOL_H

#define POOL_H


#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/stat.h>

#define SHARED_MEMORY_NAME "/shared_memory"
#define BUFSIZE 4096

#define SEM_NAME1 "/sem1"
#define SEM_NAME2 "/sem2"

typedef enum {
    OK,
    INVALID_INPUT,
    INVALID_FILES,
    MEMORY_ERROR,
    ERROR_FORK,
    ERROR_EXECV,
} state;


#endif //POOL_H
