// Eetu Heikurinen, 424495
// 10.11.2025

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){
    FILE *in = NULL;
    FILE *out = NULL;

    // Handle the amount of arguments
    if (argc > 3){
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    if (argc == 1){
        in = stdin;
        out = stdout;
    } else if (argc == 2){
        in = fopen(argv[1], "r");
        if (!in){
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
        out = stdout;
    } else if (argc == 3){
        if (strcmp(argv[1], argv[2]) == 0){
            fprintf(stderr, "Input and output files have to be diffrent.\n");
            exit(1);
        }
        in = fopen(argv[1], "r");
         if (!in){
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
        out = fopen(argv[2], "w");
        if (!out){
            fclose(in);
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
    }

    // Dynamic array for the amount of lines
    size_t capacity = 128;
    size_t count = 0;
    char **lines = malloc(capacity * sizeof(char *));
    if (!lines) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, in)) != -1){
        if (count >= capacity) {
            capacity *= 2;
            char **new_lines = realloc(lines, capacity * sizeof(char *));
            if (!new_lines){
                fprintf(stderr, "malloc failed\n");
                exit(1);
            }
            lines = new_lines;
        }
        lines[count++] = strdup(line); // Line is copied into storage
        if (!lines[count - 1]){
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
    }
    free(line);

    // Print in reverse

    for (long i = count-1; i>=0; i--){
        fprintf(out, "%s", lines[i]);
        free(lines[i]);
    }
    free(lines);
    if (in != stdin) fclose(in);
    if (out != stdout) fclose(out);

    exit(0);
}

