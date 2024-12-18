#include "./headers/allocator_list.h"
#include "./headers/allocator_mkk.h"
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <cstdio>

// Макрос для write
#define WRITE_STDOUT(str) write(STDOUT_FILENO, str, strlen(str))

// запросы содержат адрес и размер запрашиваемого места
typedef struct request_strucе {
    void *address;
    size_t bytes;
} request;

// char* в число
size_t ToSizeT(const char *string) {
    size_t size = 0;

    while (*string != '\0') {
        if (*string < '0' || *string > '9') return 0;
        size = size * 10 + *string - '0';
        ++string;
    }

    return size;
}

// Функция для вывода числа через write
void WriteNumber(size_t number) {
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%zu", number);
    write(STDOUT_FILENO, buffer, len);
}

void WriteDouble(double number) {
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "%lf", number);
    write(STDOUT_FILENO, buffer, len);
}

void WriteLongDouble(long double number) {
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "%Lf", number);
    write(STDOUT_FILENO, buffer, len);
}

int main(int argc, char *argv[]) {
    const size_t REQUEST_QUANTITY = 1000;
    const size_t MAX_BYTES = 5000;
    clock_t first_time, second_time;
    size_t first_index, second_index, third_index;
    size_t argument;
    size_t query = 0;
    size_t total = 0;
    size_t *permute = (size_t *) malloc(sizeof(size_t) * REQUEST_QUANTITY);
    request *requests = (request *) malloc(sizeof(request) * REQUEST_QUANTITY);

    if (argc != 2) {
        WRITE_STDOUT("Error. No memory\n");
        return 0;
    }

    srand((unsigned int) time(0));

    argument = ToSizeT(argv[1]);

    if (!InitializationList(argument)) {
        WRITE_STDOUT("Error. No memory\n");
        return 0;
    }

    if (!MKKInitialization(argument)) {
        WRITE_STDOUT("Error. No memory\n");
        return 0;
    }

    for (first_index = 0; first_index < REQUEST_QUANTITY; ++first_index) {
        requests[first_index].bytes = 1 + rand() % MAX_BYTES;
        permute[first_index] = first_index;
    }

    for (first_index = 0; first_index < REQUEST_QUANTITY; ++first_index) {
        second_index = rand() % REQUEST_QUANTITY;
        third_index = rand() % REQUEST_QUANTITY;
        argument = permute[second_index];
        permute[second_index] = permute[third_index];
        permute[third_index] = argument;
    }

    WRITE_STDOUT("Количество запросов: ");
    WriteNumber(REQUEST_QUANTITY);
    WRITE_STDOUT("\nБайт: ");
    WriteNumber(MAX_BYTES);
    WRITE_STDOUT("\n\nАллокация списка свободных блоков:\n");

    first_time = clock();

    for (first_index = 0; first_index < REQUEST_QUANTITY; ++first_index) {
        requests[first_index].address = malloc_list(requests[first_index].bytes);
    }

    second_time = clock();

    WRITE_STDOUT("Заняло времени: ");
    WriteDouble((double) (second_time - first_time) / CLOCKS_PER_SEC);
    WRITE_STDOUT("\n");

    query = ListOfRequests();
    total = TotalList();

    for (first_index = 0; first_index < REQUEST_QUANTITY; ++first_index) {
        if (requests[permute[first_index]].address == NULL) continue;
        FreeList(requests[permute[first_index]].address);
    }

    first_time = clock();

    WRITE_STDOUT("Очистка заняла: ");
    WriteDouble((double) (first_time - second_time) / CLOCKS_PER_SEC);
    WRITE_STDOUT("\nКПД использованной и запрошенной памяти: ");
    WriteLongDouble((long double) query / total);
    WRITE_STDOUT("\n\nАллокация Мак-Кьюзи-Кэрлса\n");

    first_time = clock();

    for (first_index = 0; first_index < REQUEST_QUANTITY; ++first_index) {
        requests[first_index].address = MKKMalloc(requests[first_index].bytes);
    }

    second_time = clock();

    WRITE_STDOUT("Заняло времени: ");
    WriteDouble((double) (second_time - first_time) / CLOCKS_PER_SEC);
    WRITE_STDOUT("\n");

    query = MKKRequest();
    total = MKKTotal();

    for (first_index = 0; first_index < REQUEST_QUANTITY; ++first_index) {
        if (requests[permute[first_index]].address == NULL) continue;
        MKKFree(requests[permute[first_index]].address);
    }

    first_time = clock();

    WRITE_STDOUT("Очистка заняла: ");
    WriteDouble((double) (first_time - second_time) / CLOCKS_PER_SEC);
    WRITE_STDOUT("\nКПД использования и запрошенной памяти: ");
    WriteLongDouble((long double) query / total);
    WRITE_STDOUT("\n");

    Destroy();
    MKKDestroy();

    free(requests);
    free(permute);

    return 0;
}
