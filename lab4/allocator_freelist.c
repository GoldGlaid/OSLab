#include <string.h>
#include <sys/mman.h>
#include <limits.h>


typedef struct Block {
    struct Block *next;
    size_t size;
} Block;

typedef struct Allocator {
    Block *head;
    void *memory;
    size_t size;
} Allocator;

Allocator *allocator_create(void *const memory, const size_t size) {
    if (!memory || size <= sizeof(Block)) return NULL;

    Allocator *allocator = (Allocator *) mmap(NULL, sizeof(Allocator), PROT_READ | PROT_WRITE,
                                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (!allocator) return NULL;

    allocator->memory = memory;
    allocator->size = size;
    allocator->head = (Block *) memory;

    allocator->head->size = size - sizeof(Block);
    allocator->head->next = NULL;

    return allocator;
}

void allocator_destroy(Allocator *const allocator) {
    if (allocator) {
        munmap(allocator, sizeof(Allocator));
    }
}

void *allocator_alloc(Allocator *const allocator, const size_t size) {
    if (!allocator || size == 0) return NULL;

    Block *current = allocator->head;
    Block *prev = NULL;
    Block *best_fit = NULL;
    Block *best_fit_prev = NULL;

    size_t best_fit_size = ULONG_MAX;

    // Найти лучший блок
    while (current) {
        if (current->size >= size && current->size < best_fit_size) {
            best_fit = current;
            best_fit_prev = prev;
            best_fit_size = current->size;
        }
        prev = current;
        current = current->next;
    }

    if (!best_fit) return NULL;

    size_t remaining_size = best_fit->size - size - sizeof(Block);

    if (remaining_size >= sizeof(Block)) {
        Block *new_block = (Block *) ((char *) best_fit + sizeof(Block) + size);
        new_block->size = remaining_size;
        new_block->next = best_fit->next;

        if (best_fit_prev) {
            best_fit_prev->next = new_block;
        } else {
            allocator->head = new_block;
        }
        best_fit->size = size;
    } else {
        if (best_fit_prev) {
            best_fit_prev->next = best_fit->next;
        } else {
            allocator->head = best_fit->next;
        }
    }

    return (void *) ((char *) best_fit + sizeof(Block));
}

void allocator_free(Allocator *const allocator, void *const memory) {
    if (!allocator || !memory) return;

    Block *header = (Block *) ((char *) memory - sizeof(Block));
    Block *current = allocator->head;
    Block *prev = NULL;

    // Найти позицию для вставки
    while (current && current < header) {
        prev = current;
        current = current->next;
    }

    header->next = current;

    if (prev) {
        prev->next = header;
        if ((char *) prev + sizeof(Block) + prev->size == (char *) header) {
            prev->size += sizeof(Block) + header->size;
            prev->next = header->next;
            header = prev;
        }
    } else {
        allocator->head = header;
    }

    if (current && (char *) header + sizeof(Block) + header->size == (char *) current) {
        header->size += sizeof(Block) + current->size;
        header->next = current->next;
    }
}
