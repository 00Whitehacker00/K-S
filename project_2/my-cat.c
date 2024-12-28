#include <stdio.h>
#include <stdlib.h>

#define MAXLEN 1024

int main(int argc, char * argv[]) {
    
    // If 0 files are provided, exit with status code 0
    if (argc < 2) {
        return 0; 
    }

    // for-loop goes through all given files
    int i;
    for (i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");

        // Check if file was opened or failed
        if (file == NULL) {
            fprintf(stderr, "my-cat: cannot open file\n");
            exit(1);
        }

        // Read files and print their lines
        char buffer[MAXLEN];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s", buffer);
        }

        fclose(file);
    }

    return 0;
}


