// Eetu Heikurinen, 424495
// 17.11.2025

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    // If there isn't any files, exit

    if (argc == 1){
        return 0;
    }

    // Going through the file names
    for (int i = 1; i < argc; i++){
        FILE *fp = fopen(argv[i], "r");
        // If there is an error we exit
        if (fp == NULL){
            printf("my-cat: cannot open file\n");
            return 1;
        }

        char buffer[4096]; // temporary storage for reading the data
        while (fgets(buffer, sizeof(buffer), fp) != NULL){
            printf("%s", buffer);
        }

        fclose(fp);
    }

    return 0;
}
