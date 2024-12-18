#ifndef ALLOCATOR_LIST_H
#define ALLOCATOR_LIST_H

#include <cstdio>
#include <cstdlib>

typedef unsigned char *PBYTE_LIST;

// Структура двусвязного списка для свободных блоков
typedef struct block_list {
    size_t size;
    struct block_list *previous;
    struct block_list *next;
} block_list;

//Стартовый адрес области
static block_list *begin_list;

//Адрес первого свободного блока
static block_list *free_list;

//Общий размер выделенной памяти
static size_t size_list;

//Счётчик количества запрашиваемой информации
static size_t request_list = 0;

//Счётчик количества используемой информации
static size_t total_list = 0;

int InitializationList(size_t size);

void Destroy();

void *BlockAlloc(block_list *block, size_t size);

void *malloc_list(size_t size);

void FreeList(void *address);

size_t ListOfRequests();

size_t TotalList();

#endif