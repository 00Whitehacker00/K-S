#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    // If 0 files are provided, exit with status code 1
    if (argc < 2) {
        fprintf(stderr, "my-zip: file1 [file2 ...]\n");
        exit(1);
    }

    // for-loop goes through all given files
    int i;
    for (i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");

        // Check if file opened or failed
        if (file == NULL) {
            fprintf(stderr, "my-zip: cannot open file\n");
            exit(1);
        }

        // Count characters and read first character
        int count = 0;
        int current = fgetc(file); 

        if (current == EOF) {
            fclose(file);
            continue;
        }

        int previous = current; 

        // while-loop goes through all characters in file
        while (current != EOF) {
            
            // If the same character comes up again, count will increase
            if (current == previous) {
                count++; 

            } else {

                // Write down count and character
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&previous, sizeof(char), 1, stdout);

                // Reset count for the character
                count = 1; 
            }

            // Update previous for next round and read next character
            previous = current; 
            current = fgetc(file); 
        }

        // Write down final count and character
        if (count > 0) {
            fwrite(&count, sizeof(int), 1, stdout);
            fwrite(&previous, sizeof(char), 1, stdout);
        }

        fclose(file); 
    }

    return 0; 
}
