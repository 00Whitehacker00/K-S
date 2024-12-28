#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// grep() function handles grep-like functionality
void grep(const char *pattern, FILE *file) {
    char *line = NULL;
    size_t len = 0;

    // while-loop reads lines from file
    while (getline(&line, &len, file) != -1) {
        
        // Check if line has the search pattern
        if (strstr(line, pattern) != NULL) {
            printf("%s", line); 
        }
    }
    free(line);  
}

int main(int argc, char *argv[]) {

    // If 0 arguments are provided, exit with status code 1
    if (argc < 2) {
        fprintf(stderr, "my-grep: searchterm [file ...]\n");
        exit(1);
    }

    const char *pattern = argv[1];

    // If 0 files are given, read from standard input
    if (argc == 2) {
        grep(pattern, stdin);

    } else {

        // for-loop goes over given files
        int i;
        for (i = 2; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");

            // Check if file was opened or failed
            if (file == NULL) {
                fprintf(stderr, "my-grep: cannot open file\n");
                exit(1);
            }

            // Search for pattern in file using grep
            grep(pattern, file);
            fclose(file);
        }
    }

    return 0;
}
