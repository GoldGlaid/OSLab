#include "../headers//allocator_list.h"

int InitializationList(size_t size) {
    if (size < sizeof(block_list)) {
        size = sizeof(block_list);
    }

    begin_list = (block_list *) malloc(size);

    if (begin_list == NULL) {
        return 0;
    }

    begin_list->size = size;
    begin_list->previous = NULL;
    begin_list->next = NULL;
    free_list = begin_list;
    size_list = size;
    return 1;
}

void Destroy() {
    free(begin_list);
}

void *BlockAlloc(block_list *block, size_t size) {
    block_list *next_block = NULL;

    if (block->size >= size + sizeof(block_list)) {
        next_block = (block_list *) ((PBYTE_LIST) block + size);
        next_block->size = block->size - size;
        next_block->previous = block->previous;
        next_block->next = block->next;
        block->size = size;

        if (block->previous != NULL) block->previous->next = next_block;

        if (block->next != NULL) block->next->previous = next_block;

        if (block == free_list) free_list = next_block;
    } else {
        if (block->previous != NULL) block->previous->next = block->next;

        if (block->next != NULL) block->next->previous = block->previous;

        if (block == free_list) free_list = block->next;
    }

    return (void *) ((PBYTE_LIST) block + sizeof(size_t));
}

void *malloc_list(size_t size) {
    size_t first_size = size_list;
    size_t old_size = size;
    block_list *first_block = free_list;
    block_list *current = free_list;

    size += sizeof(size_t);

    if (size < sizeof(block_list))
        size = sizeof(block_list);

    int flag = 0;

    while (current != NULL && flag == 0) {
        // если блок может вместить необходимую информацию, значит он подходит
        if (current->size < first_size && current->size >= size) {
            first_size = current->size;
            first_block = current;
            flag = 1;
        }

        current = current->next;
    }

    if (free_list == NULL || first_block->size < size) return NULL;

    // добавляем в счетчики
    request_list += old_size;
    total_list += size;

    // фрагментация найденного блока для использования оставшего места
    return BlockAlloc(first_block, size);
}


// освобождение места занимаемого запросом по адресу address
void FreeList(void *address) {
    // находим адрес блока
    block_list *block = (block_list *) ((PBYTE_LIST) address - sizeof(size_t));
    block_list *current = free_list;
    block_list *left_block = NULL;
    block_list *right_block = NULL;

    while (current != NULL) {
        // блок, который располагается левее освобождённого и самый близкий
        if ((block_list *) ((PBYTE_LIST) current + current->size) <= block) left_block = current;

        // аналогично правый
        if ((block_list *) ((PBYTE_LIST) block + block->size) <= current) {
            right_block = current;
            break;
        }

        current = current->next;
    }

    // добавление освобождённого блока в двусвязный список свободных блоков
    if (left_block != NULL) {
        left_block->next = block;
    } else {
        free_list = block;
    }

    if (right_block != NULL) {
        right_block->previous = block;
    }

    block->previous = left_block;
    block->next = right_block;
    current = free_list;

    // объединение двух блоков рядом в один блок большего размера
    while (current != NULL) {
        if ((block_list *) ((PBYTE_LIST) current + current->size) == current->next) {
            current->size += current->next->size;
            current->next = current->next->next;

            if (current->next != NULL) current->next->previous = current;

            continue;
        }

        current = current->next;
    }
}

// счетчики
size_t ListOfRequests() {
    return request_list;
}

size_t TotalList() {
    return total_list;
}