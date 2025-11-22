// Eetu Heikurinen, 424495
// 17.11.2025
// Program that mimics Unix 'unzip' command

#include <stdio.h>
#include <stdlib.h>

void decompress_file(FILE *fp){
    int count;
    int c;

    while (fread(&count, sizeof(int), 1, fp) == 1){
        c = fgetc(fp); // read the character from the 4-byte integer we get from fread()
        if (c == EOF) break;

        for (int i = 0; i<count; i++){
            fputc(c, stdout); // Write repeated characters
        }
    }
}

int main(int argc, char *argv[]){
    // Atleast one file is provided
    if (argc < 2) {
        printf("my-unzip: file1 [file2 ...]\n");
        return 1;
    }

    // Going thorugh all the files
    for (int i = 1; i < argc; i++){
        FILE *fp = fopen(argv[i], "rb"); // Open in binary read
            if (fp == NULL){
                printf("my-unzip: cannot open file\n");
                return 1;
            }
            
        decompress_file(fp);
        fclose(fp);
    }
    return 0;
}
