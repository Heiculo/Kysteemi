// Eetu Heikurinen, 424495
// 17.11.2025

#include <stdio.h>
#include <stdlib.h>

void compress_file(FILE *fp){
    int count;
    int c, prev;

    prev = fgetc(fp);
    if (prev == EOF) return;

    count = 1;
    while ((c = fgetc(fp)) != EOF){
        if (c == prev){
            count++;
        } else {
            fwrite(&count, sizeof(int), 1, stdout); // Gives the 4-byte binary integer
            fputc(prev, stdout);

            prev = c;
            count = 1;
        }
    }

    fwrite(&count, sizeof(int), 1, stdout);
    fputc(prev, stdout);
}

int main(int argc, char *argv[]){
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
