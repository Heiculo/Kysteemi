// Eetu Heikurinen, 424495
// 17.11.2025

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Searching for the term
int search_file(FILE *fp, const char *searcterm){
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1){
        if(strstr(line, searcterm) != NULL){  // strstr() finds the substring in the line
            printf("%s", line);
        }
    }

    free(line);
    return 0;
}

int main (int argc, char *argv[]) {
    // Exit if not enough inputs
    if (argc < 2){
        printf("my-grep: searcterm [file...]\n");
        return 1;
    }

    // Term that is looked up
    char *searcterm = argv[1];


    if (argc == 2){
        // If no file is provided, search from stdin
        search_file(stdin, searcterm);
    } else { 
        for (int i = 2; i < argc; i++){ // Loop through all the given files
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL){
                printf("my-grep: cannot open file\n");
                return 1;
            }

            search_file(fp, searcterm);
            printf("\n");
            fclose(fp);
        }
    }
    return 0;
}
