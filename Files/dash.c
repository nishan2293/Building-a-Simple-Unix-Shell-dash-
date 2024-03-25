#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Function to display a standard error message.
void display_error() {
    const char error_message[] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, sizeof(error_message) - 1);
}

// Function to prepend "/bin/" to a command, which helps in executing it.
char* prepend_bin_to_cmd(const char *cmd) {
    const char *prefix = "/bin/";
    char *fullpath = malloc(strlen(prefix) + strlen(cmd) + 1);
    if (!fullpath) {
        display_error();
        exit(1);
    }
    strcpy(fullpath, prefix);
    strcat(fullpath, cmd);
    return fullpath;
}

// Function to process each command.
void process_cmd(char *cmd_line) {
    char *cmd_args[100];
    int idx = 0;
    int k;
    pid_t pid;
    int file_descriptor = -1;

    // Tokenize commands split by '&'.
    char *command = strtok(cmd_line, "&");
    while (command != NULL) {
        idx = 0;
        
        // Tokenize arguments for each command.
        cmd_args[idx] = strtok(command, " ");
        while (cmd_args[idx]) {
            idx++;
            cmd_args[idx] = strtok(NULL, " ");
        }

        // Check for output redirection using ">".
        for (k = 0; k < idx; k++) {
            if (strcmp(cmd_args[k], ">") == 0) {
                if (cmd_args[k+1]) {
                    // Open or create the file for writing.
                    file_descriptor = open(cmd_args[k+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (file_descriptor == -1) {
                        display_error();
                        return;
                    }
                    cmd_args[k] = NULL;
                    break;
                } else {
                    display_error();
                    return;
                }
            }
        }

        // Fork a child process to execute the command.
        pid = fork();
        if (pid == 0) { // Child process
            if (file_descriptor != -1) {
                dup2(file_descriptor, STDOUT_FILENO);
                close(file_descriptor);
            }
            char *full_cmd_path = prepend_bin_to_cmd(cmd_args[0]);
            if (execv(full_cmd_path, cmd_args) == -1) {
                free(full_cmd_path);
                display_error();
                exit(1);
            }
        } else { // Parent process
            if (command[strspn(command, " \t")] != '\0') { 
                // Wait for the child process if the command isn't just whitespace.
                waitpid(pid, NULL, 0);
            }
        }
        command = strtok(NULL, "&");
    }
}

// Main function where the execution begins.
int main(int argc, char **argv) {
    char *originalPath = getenv("PATH");
    char newPath[2048];

    // Modify PATH environment variable.
    if (originalPath != NULL && strlen(originalPath) > 0) {
        snprintf(newPath, sizeof(newPath), "/bin:%s", originalPath);
    } else {
        strncpy(newPath, "/bin", sizeof(newPath));
    }

    setenv("PATH", newPath, 1);

    // Check for argument count. Only one additional argument is allowed (a file to read commands from).
    if (argc > 2) {
        display_error();
        exit(1);
    }

    char *input = NULL;
    size_t len = 0;
    ssize_t read_chars;
    FILE *input_source = stdin;

    // If there's an input file, open it.
    if (argc == 2) {
        input_source = fopen(argv[1], "r");
        if (!input_source) {
            display_error();
            exit(1);
        }
    }

    // Main loop to process commands.
    while (1) {
        if (input_source == stdin) {
            printf("dash> ");
        }

        read_chars = getline(&input, &len, input_source);
        if (read_chars == -1) {
            if (input_source == stdin) {
                display_error();
            }
            break;
        }

        // Remove newline character from the input.
        if (input[read_chars - 1] == '\n') {
            input[read_chars - 1] = '\0';
        }

        // Exit command to break out of the loop.
        if (strcmp(input, "exit") == 0) {
            free(input);
            exit(0);
        }

        // Special handling for 'cd' command.
        char *temp = strdup(input);
        char *first_cmd = strtok(temp, " ");
        if (first_cmd && strcmp(first_cmd, "cd") == 0) {
            char *path = strtok(NULL, " ");
            if (!path || strtok(NULL, " ")) {
                display_error();
                continue;
            }
            if (chdir(path) != 0) {
                display_error();
                continue;
            }
            free(temp);
            continue;
        }

        free(temp);
        process_cmd(input);
    }

    free(input);
    exit(0);
}

