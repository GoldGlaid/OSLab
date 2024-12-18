#ifndef ALLOCATOR_MKK_H
#define ALLOCATOR_MKK_H

#include <cstdio>
#include <cstdlib>

typedef unsigned char *PBYTE_MKK;

typedef enum memory_structure {
    free_state = 0
} memory_state;


// каждая из страниц может находиться в одном из 3 состояний:
// 1. свободная -> 
// -> соответствующий элемент массива содержит указатель на элемент, описывающий следующую свободную страницу
// 2. разбитая на буферы определённого размера (некоторая степень 2) -> элемент массива содержит размер буфера
// 3. частью буфера, объединяющего сразу несколько страниц ->
// -> элемент массива указывает на первую страницу буфера, в котором находятся данные о его длине


// односвязный список для блоков
typedef struct block_mkk_structure {
    struct block_mkk_structure *next;
} block_mkk;


// размер одной страницы
static const size_t PAGE_SIZE_MKK = 4096;

// стартовый адрес области
static void *heap_mkk = NULL;

// массив для управления страницами
static size_t *memory_size_mkk = NULL;

//Массив, содержащий заголовки всех буферов, имеющих размер меньше одной страницы
static block_mkk **list_mkk = NULL;

//Общее количество страниц
static size_t pages_mkk = 0;

// Степень двойки
static size_t pow_mkk = 0;

//Минимальный размер необходимый для хранения указателя на элемент
static size_t pow_index_minimum = 0;

//Счётчик количества запрашиваемой информации
static size_t request_mkk = 0;

//Счётчик количества использованной информации
static size_t total_mkk = 0;

int MKKInitialization(size_t size);

void MKKDestroy();

void *MKKMalloc(size_t size);

void MKKFree(void *address);

block_mkk *MKKAllocPage(size_t size);

void MKKFreePage(block_mkk *block);

void MKKSplitPage(block_mkk *block, size_t powIndex);

size_t PowOfTwo(size_t size);

size_t MKKPageCounter(size_t size);

size_t MKKPageIndex(block_mkk *block);

size_t MKKRequest();

size_t MKKTotal();

#endif