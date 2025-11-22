// Eetu Heikurinen, 424495
// 17.11.2025
// Program that mimics Unix 'zip' command

#include <stdio.h>
#include <stdlib.h>

void compress_file(FILE *fp){
    int count;
    int c, prev;

    // Initialize the counting by reading the first character
    prev = fgetc(fp);
    if (prev == EOF) return;

    count = 1;
    while ((c = fgetc(fp)) != EOF){
        if (c == prev){
            // If the character matches the previous one --> increase the count
            count++;
        } else {
            fwrite(&count, sizeof(int), 1, stdout); // Gives the 4-byte binary integer if the character changes
            fputc(prev, stdout);

            // reset
            prev = c;
            count = 1;
        }
    }

    fwrite(&count, sizeof(int), 1, stdout);
    fputc(prev, stdout);
}

int main(int argc, char *argv[]){
    // Atleast one file is provided
    if (argc < 2) {
        printf("my-zip: file1 [file2 ...]\n");
        return 1;
    }

    // Going thorugh all the files
    for (int i = 1; i < argc; i++){
        FILE *fp = fopen(argv[i], "r");
            if (fp == NULL){
                printf("my-zip: cannot open file\n");
                return 1;
            }
            
        compress_file(fp);
        fclose(fp);
    }
    return 0;
}
