#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>


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
    int fd, recieved_number, len_row;
    char status;
    fd = strtol(argv[1], &end, 10);;
    fread(&status, sizeof(char), 1, stdin);
    printf("status is %d\n", status);
    while (status != EOF) {

        fread(&recieved_number, sizeof(recieved_number), 1, stdin);
        printf("recieved_number is %d\n", recieved_number);

        char *row = (char *) malloc(sizeof(char) * recieved_number);
        if (row == NULL) {
            return 1;
        }

        if (fread(row, sizeof(char), recieved_number, stdin) == -1) {
            printf("GOOOOOOOOOOOOOOOOOOOOOOOOOL\n");
        }

        len_row = strlen(row);
        printf("row first %c\n", row[0]);


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



