#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    // Открываем семафоры
    sem_t *sem1 = sem_open("/sem1", 0);
    sem_t *sem2 = sem_open("/sem2", 0);
    if (sem1 == SEM_FAILED || sem2 == SEM_FAILED) {
        const char *msg_error = "[CHILD] ERROR: SEMAPHORE_ERROR.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(EXIT_FAILURE);
    }

    // Создаем область общей памяти
    const size_t SHM_SIZE = 4096;
    char *shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shm == MAP_FAILED) {
        const char *msg_error = "[CHILD] ERROR: MEMORY_ERROR.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(EXIT_FAILURE);
    }

    char status;
    while (1) {
        // Ожидаем сигнал от родителя
        sem_wait(sem1); // Для дочернего процесса 1
        sem_wait(sem2); // Для дочернего процесса 2

        // Читаем статус
        status = shm[0];
        if (status == EOF) {
            break; // Выход из цикла, если получен EOF
        }

        // Получаем строку
        char *row = &shm[1]; // Строка начинается с индекса 1
        int received_number = strlen(row); // Длина строки

        // Инвертируем строку
        reverse_string(row);

        // Записываем результат обратно в stdout
        char newline = '\n';
        write(STDOUT_FILENO, row, received_number);
        write(STDOUT_FILENO, &newline, 1);
    }

    // Освобождаем ресурсы
    munmap(shm, SHM_SIZE);
    sem_close(sem1);
    sem_close(sem2);
    sem_unlink("/sem1");
    sem_unlink("/sem2");

    return 0;
}
