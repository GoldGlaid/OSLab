#include <string.h>

#include <stdlib.h>
#include <unistd.h>
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
    int recieved_number;
    char status;

    fread(&status, sizeof(char), 1, stdin);
    while (status != EOF) {
        fread(&recieved_number, sizeof(recieved_number), 1, stdin);
        char *row = (char *) malloc(sizeof(char) * recieved_number);
        row[0] = '\0';

        fread(row, sizeof(char), recieved_number, stdin);

        row[recieved_number] = '\0';
        reverse_string(row);

        char space = '\n';
        write(STDOUT_FILENO, row, recieved_number);
        write(STDOUT_FILENO, &space, 1);

        fread(&status, sizeof(char), 1, stdin);
        free(row);
    }

    close(STDIN_FILENO);
    return 0;
}