#include "../headers//allocator_mkk.h"

int MKKInitialization(size_t size) {
    size_t index;
    block_mkk *block = NULL;

    // память разбита на набор последовательных страниц
    pages_mkk = MKKPageCounter(size);
    // степень двойки для размера одной страницы
    pow_mkk = PowOfTwo(PAGE_SIZE_MKK);
    // степень двойки для структуры блока
    pow_index_minimum = PowOfTwo(sizeof(block_mkk));
    // стартовый адрес области
    heap_mkk = malloc(pages_mkk * PAGE_SIZE_MKK);
    // массив управления страницами
    memory_size_mkk = (size_t *) malloc(sizeof(size_t) * pages_mkk);
    // заголовки буферов размер которых меньше одной страницы для объединения
    list_mkk = (block_mkk **) malloc(sizeof(block_mkk *) * pow_mkk);

    if (heap_mkk == NULL || memory_size_mkk == NULL || list_mkk == NULL) return 0;

    // определение первой страницы и остальных
    memory_size_mkk[free_state] = free_state;
    list_mkk[free_state] = (block_mkk *) heap_mkk;
    block = list_mkk[free_state];

    for (index = 1; index < pages_mkk; ++index) {
        memory_size_mkk[index] = free_state;
        block->next = (block_mkk *) ((PBYTE_MKK) block + PAGE_SIZE_MKK);
        block = block->next;
    }

    block->next = NULL;

    // заголовков пока нет
    for (index = 1; index < pow_mkk; ++index) {
        list_mkk[index] = NULL;
    }

    return 1;
}

void MKKDestroy() {
    free(heap_mkk);
    free(memory_size_mkk);
    free(list_mkk);
}

void *MKKMalloc(size_t size) {
    size_t pow_index = PowOfTwo(size);
    size_t old_size = size;
    block_mkk *block = NULL;

    if (pow_index < pow_index_minimum) pow_index = pow_index_minimum;

    size = 1 << pow_index;

    // если размер меньше размера страницы
    if (size < PAGE_SIZE_MKK) {
        if (list_mkk[pow_index] == NULL) {
            // если нет ни одного буфера размером в 2^pow_index, то выделим страницу для этого
            block = MKKAllocPage(size);

            if (block == NULL) return NULL;

            //Разделим страницу которую мы выделили под буфер размером 2^pow_index
            // на максимальное количество буферов размером 2^pow_index
            MKKSplitPage(block, pow_index);
        }

        // первый свободный буфер
        block = list_mkk[pow_index];
        list_mkk[pow_index] = block->next;

        // снова счётчики
        request_mkk += old_size;
        total_mkk += size;

        return (void *) block;
    } else {
        // снова счётчики
        request_mkk += old_size;
        total_mkk += size;

        return MKKAllocPage(size);
    }
}

void MKKFree(void *address) {
    size_t page_index = MKKPageIndex((block_mkk *) address);
    size_t pow_index = PowOfTwo(memory_size_mkk[page_index]);
    block_mkk *block = (block_mkk *) address;

    if (memory_size_mkk[page_index] < PAGE_SIZE_MKK) {
        block->next = list_mkk[pow_index];
        list_mkk[pow_index] = block;
    } else MKKFreePage(block);
}

block_mkk *MKKAllocPage(size_t size) {
    size_t count = 0;
    size_t page_index = 0;
    // находим где есть свободное место
    size_t previous_index = MKKPageIndex(list_mkk[free_state]);
    size_t pages = MKKPageCounter(size);
    block_mkk *current = list_mkk[free_state];
    block_mkk *previous = NULL;
    block_mkk *page = NULL;

    while (current != NULL) {
        // определяем номер страницы, на котором находится буффер current
        page_index = MKKPageIndex(current);

        if (page_index - previous_index <= 1) {
            if (page == NULL) page = current;

            ++count;
        } else {
            // если current занимает больше 1 страницы, то мы сбрасываем счётчик до 1, 
            // а в страницу добавляем current, чтобы при повторном прохождении цикла не зайти в (if(page == NULL))
            page = current;
            count = 1;
        }

        if (count == pages) break;

        // берем следующий буфер и обрабатываем текущий
        previous = current;
        current = current->next;
        previous_index = page_index;
    }

    // не вместится на одну страницу
    if (count < pages) page = NULL;

    if (page != NULL) {
        page_index = MKKPageIndex(page);
        // страница page_index уже разделена на буферы размером с size
        memory_size_mkk[page_index] = size;
        // адрес текущего блока
        current = (block_mkk *) ((PBYTE_MKK) page + (pages - 1) * PAGE_SIZE_MKK);

        if (previous != NULL) previous->next = current->next;
            // вносим адрес свободного блока
        else list_mkk[free_state] = current->next;
    }

    return page;
}

void MKKFreePage(block_mkk *block) {
    size_t index;
    size_t page_index = MKKPageIndex(block);
    size_t block_count = memory_size_mkk[page_index] / PAGE_SIZE_MKK;
    block_mkk *left = NULL;
    block_mkk *right = NULL;
    block_mkk *current = block;

    while (current != NULL) {
        // самый близкий левый буффер
        if (current < block) left = current;
            // аналогично правый
        else {
            if (current > block) {
                right = current;
                break;
            }
        }

        current = current->next;
    }

    for (index = 1; index < block_count; ++index) {
        // отделяем целые страницы от буфера
        block->next = (block_mkk *) ((PBYTE_MKK) block + PAGE_SIZE_MKK);
        block = block->next;
    }

    block->next = right;

    // освобождаем целые страницы
    if (left != NULL) left->next = block;
    else list_mkk[free_state] = block;
}

void MKKSplitPage(block_mkk *block, size_t pow_index) {
    size_t index;
    size_t page_index = MKKPageIndex(block);
    size_t block_size = 1 << pow_index;
    size_t block_count = PAGE_SIZE_MKK / block_size;

    list_mkk[pow_index] = block;
    memory_size_mkk[page_index] = block_size;

    // определение связи буферов на странице
    for (index = 1; index < block_count; ++index) {
        block->next = (block_mkk *) ((PBYTE_MKK) block + block_size);
        block = block->next;
    }

    block->next = NULL;
}

// степень двойки
size_t PowOfTwo(size_t size) {
    size_t pow = 0;

    while (size > ((size_t) 1 << pow)) {
        ++pow;
    }

    return pow;
}

// необходимое количество страниц
size_t MKKPageCounter(size_t size) {
    return size / PAGE_SIZE_MKK + (size_t) (size % PAGE_SIZE_MKK != 0);
}

// определение номера страницы, где block
size_t MKKPageIndex(block_mkk *block) {
    return (size_t) ((PBYTE_MKK) block - (PBYTE_MKK) heap_mkk) / PAGE_SIZE_MKK;
}

// снова счетчики
size_t MKKRequest() {
    return request_mkk;
}

size_t MKKTotal() {
    return total_mkk;
}