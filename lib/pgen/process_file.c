
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define DELIMITER ","

char** split_line(char* line, int* token_count) {
    char** tokens = NULL;
    int count = 0;

    // Use strtok to split the string by commas
    char* token = strtok(line, DELIMITER);
    while (token != NULL) {
        tokens = realloc(tokens, sizeof(char*) * (count + 1));
        tokens[count] = strdup(token);
        count++;

        token = strtok(NULL, DELIMITER);
    }

    *token_count = count;
    return tokens;
}

void process_file(const char* filename, void (*process_line)(char**, int)) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        // Remove newline character at the end of the line
        line[strcspn(line, "\n")] = 0;

        int token_count = 0;
        char** tokens = split_line(line, &token_count);
        process_line(tokens, token_count);
        // Process tokens
    }

    fclose(file);
}
