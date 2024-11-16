#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    //файл только на запись был создан в parent, но как его сюда передать?

    // char *path_file = argv[1];
    // int32_t file = open(path_file, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    // if (file == -1) {
    //     const char msg[] = "error: failed to open requested file\n";
    //     write(STDERR_FILENO, msg, sizeof(msg));
    //     exit(EXIT_FAILURE);
    // }

    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        buffer[bytes_read] = '\0';
        reverse_string(buffer);

        // file?
        write(STDIN_FILENO, buffer, strlen(buffer));
    }
    close(STDIN_FILENO);
    return 0;
}
