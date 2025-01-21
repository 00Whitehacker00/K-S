#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_PATHS 10
#define MAX_ARGS 10
#define MAX_COMMANDS 10

void execute(char* command, char** args, char* output_file, char** paths, int path_count) {

    // If redirection is specified, handle output file redirection
    if (output_file) {
        
        // Check if file opened or failed
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            char error_message[100] = "An error has occurred while opening the output file\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }

        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

    // for-loop goes through each directory specified in the paths to search for the command
    for (int i = 0; i < path_count; i++) {
        
        // Create path to executable
        char executable_path[256];
        snprintf(executable_path, sizeof(executable_path), "%s/%s", paths[i], command);
        
        // Check if command is executable and run command with arguments
        if (access(executable_path, X_OK) == 0) {
            execv(executable_path, args);
        }
    }

    char error_message[100] = "An error has occurred while executing the command\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
}

void run(char* input, char** paths, int* path_count) {
    char* commands[MAX_COMMANDS];       // Array of parallel commands
    int command_count = 0;              // Num of parallel commands

    // Split input into seperate commands
    char* command = strtok(input, "&");
    while (command && command_count < MAX_COMMANDS) {
        commands[command_count++] = command;
        command = strtok(NULL, "&");
    }

    pid_t pids[MAX_COMMANDS];

    for (int i = 0; i < command_count; i++) {
        char* args[MAX_ARGS];           // Array of arguments for command
        char* output_file = NULL;
        int arg_count = 0;

        // Make commands into tokens and handle arguments
        char* token = strtok(commands[i], " \t");
        while (token) {
            
            // File redirection, if '>' is there
            if (strcmp(token, ">") == 0) {
                token = strtok(NULL, " \t");
            
                // Give redirection file
                if (token && !output_file) {
                    output_file = token;
                } else {
                    char error_message[100] = "An error has occurred while handling redirection\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    return;
                }

            } else {
                args[arg_count++] = token;
            }
            token = strtok(NULL, " \t");
        }
        args[arg_count] = NULL;
        if (arg_count == 0) continue;

        // Handle built-in commands
        if (strcmp(args[0], "exit") == 0) {
            
            // Handle extra arguments
            if (arg_count > 1) {
                char error_message[100] = "An error has occurred: 'exit' doesn't take arguments\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            } else {
                exit(0);
            }

        } else if (strcmp(args[0], "cd") == 0) {

            // Handle cd
            if (arg_count != 2 || chdir(args[1]) != 0) {
                char error_message[100] = "An error has occurred while changing directory\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                
            } 
            return;
            
        } else if (strcmp(args[0], "path") == 0) {

            if (args[1] == NULL) {
                for (int j = 1; j < arg_count; j++) {

                    // Free dynamically allocated memory for each path
                    free(paths[j]); 
                    paths[j] = NULL;
                }
                *path_count = 0;
                return;
            }
            
            // Handle path
            *path_count = 0;
            for (int j = 1; j < arg_count; j++) {

                // Dynamically allocate memory for paths
                paths[*path_count] = strdup(args[j]); 
                (*path_count)++;
            }
            return;
        } 

        // Fork new process for other commands
        pids[i] = fork();
        if (pids[i] == 0) {
            
            // Child process execute command
            execute(args[0], args, output_file, paths, *path_count);

        } else if (pids[i] < 0) {
            char error_message[100] = "An error has occurred while forking a process\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        
    }

    // Parent process waits for child processes to be finished
    for (int i = 0; i < command_count; i++) {
        if (pids[i] > 0) {
            waitpid(pids[i], NULL, 0);
        }
    }
}

int main(int argc, char* argv[]) {
    char* paths[MAX_PATHS];             // Array of paths
    int path_count = 1;                 // Num of current paths in array
    paths[0] = "/bin";                  // Default path

    FILE* input = stdin;
    int interactive_mode = 1;

    // Open file if batch file is specified
    if (argc == 2) {
        input = fopen(argv[1], "r");
        interactive_mode = 0;

        if (!input) {
            char error_message[100] = "An error has occurred while opening the batch file\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }

    } else if (argc > 2) {
        char error_message[100] = "An error has occurred: Too many arguments\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    char* line = NULL;
    size_t len = 0;

    // while-loop reads input and process commands
    while (1) {
        if (interactive_mode) {
            printf("wish> ");
            fflush(stdout);
        }

        if (getline(&line, &len, input) == -1) {
            break;
        }

        line[strcspn(line, "\n")] = '\0';
        run(line, paths, &path_count);
    }
    free(line);
    if (input != stdin) fclose(input);
    return 0;
}
