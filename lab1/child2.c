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
    char *end;
    int fd = strtol(argv[1], &end, 10);
    int recieved_number, len_row;
    char status;

    fread(&status, sizeof(char), 1, stdin);
    while (status != EOF) {

        fread(&recieved_number, sizeof(recieved_number), 1, stdin);

        char *row = (char *) malloc(sizeof(char) * recieved_number);
        if (row == NULL) {
            return 1;
        }

        fread(row, sizeof(char), recieved_number, stdin);
        len_row = strlen(row);

        reverse_string(row);
        write(STDIN_FILENO, row, len_row);
        write(fd, row, len_row);

        fread(&status, sizeof(char), 1, stdin);

        free(row);
    }

    close(fd);
    close(STDIN_FILENO);
    return 0;
}
