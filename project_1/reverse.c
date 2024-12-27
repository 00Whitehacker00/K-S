#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lines {
    char *string;       
    struct lines *prev;     // Previous node
    struct lines *next;     // Next node
};

// read_lines() function reads the lines from the input stream and stores them in a double-linked list
int read_lines(struct lines **line, FILE *stream) {

    char *tmp = NULL;
    size_t len = 0;
    ssize_t read;
    struct lines *lp;

    // while loop reads every line from the input stream
    while ((read = getline(&tmp, &len, stream)) != -1) {
        
        if (*line == NULL) {

            // If list is empty, allocate memory + error handling
            if ((*line = lp = malloc(sizeof(struct lines))) == NULL) {
                fprintf(stderr, "malloc failed\n");
                free(tmp);
                return -1;
            }

            lp->prev = lp->next = NULL;

        } else {
 
            // Otherwise, add new node to the end of list + error handling
            if ((lp->next = malloc(sizeof(struct lines))) == NULL) {
                fprintf(stderr, "malloc failed\n");
                free(tmp);
                return -1;
            }

            // Link new node to current node and initialize next node
            lp->next->prev = lp;    
            lp = lp->next;          
            lp->next = NULL;        
        }

        // Allocate memory for the line and copy it into new node + error handling
        if ((lp->string = malloc(strlen(tmp) + 1)) == NULL) {
            fprintf(stderr, "malloc failed\n");
            free(tmp);
            return -1;
        }

        // Copy line into node
        strcpy(lp->string, tmp);    
    }

    free(tmp);
    return 0;
}

// print_lines() function prints the lines in reverse order
void print_lines(struct lines *line, FILE *stream) {
    struct lines *lp = line;

    while (lp != NULL && lp->next != NULL) {
        lp = lp->next;
    }

    while (lp != NULL) {
        fprintf(stream, "%s", lp->string);
        lp = lp->prev;
    }
}

// delete_lines() function frees memory allocated to the list
void delete_lines(struct lines *line) {
    struct lines *lp;

    lp = line;
    while (lp != NULL) {
        line = lp->next;
        free(lp->string);
        free(lp);
        lp = line;
    }
}

int main(int argc, char *argv[]) {
    struct lines *line = NULL;      // Top of double-linked list
    FILE *input = NULL; 
    FILE *output = NULL;

    // Handle wrong number of given arguments
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // Open input file if specified, use stdin if no file is specified
    if (argc >= 2) {
        input = fopen(argv[1], "r");
        
        if (input == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }

    } else {
        input = stdin;
    }

    // Open output file if specified, use stdout if no file is specified
    if (argc == 3) {

        // Check if input and output files are the same
        if (strcmp(argv[1], argv[2]) == 0) {
            fprintf(stderr, "Input and output file must differ\n");
            if (input != stdin) fclose(input);
            exit(1);
        }

        output = fopen(argv[2], "w");
        if (output == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            if (input != stdin) fclose(input);
            exit(1);
        }

    } else {
        output = stdout;
    }

    // Read lines from input and store them in the list
    if (read_lines(&line, input) == -1) {
        if (input != stdin) fclose(input);
        if (output != stdout) fclose(output);
        exit(1);
    }

    // Print lines in reverse order to output and free allocated memory
    print_lines(line, output);
    delete_lines(line);

    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);

    return 0;
}










