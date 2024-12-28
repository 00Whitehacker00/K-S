#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    // If 0 files are provided, exit with status code 1
    if (argc < 2) {
        fprintf(stderr, "my-unzip: file1 [file2 ...]\n");
        exit(1);
    }

    // for-loop goes through all given files
    int i;
    for (i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");
        
        // Check if file opened or failed
        if (file == NULL) {
            fprintf(stderr, "my-unzip: cannot open file\n");
            exit(1);
        }

        // Store run length and character
        int count; 
        char character;   

        // while-loop reads compressed data in small parts
        while (fread(&count, sizeof(int), 1, file) == 1) {  
            
            if (fread(&character, sizeof(char), 1, file) == 1) {
                
                // for-loop prints character count
                int j;
                for (j = 0; j < count; j++) {
                    printf("%c", character);
                }
            }
            
        }

        fclose(file); 
    }

    return 0; 
}
