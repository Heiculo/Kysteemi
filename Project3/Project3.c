// Eetu Heikurinen, 424495
// 17.11.2025

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_ARGS 100
#define MAX_PARALLEL 100
#define MAX_PATH 10


char error_message[30] = "An error has occurred\n";

// Path array
char *path[MAX_PATH];
int path_count = 0;

// Function for printing the error
void print_error(){
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// Executing external commands
pid_t execute_command(char ** args, char *redirect_file){
    pid_t pid = fork();
    if (pid < 0){
        print_error();
        return -1;
        
    } if (pid == 0){
        // Child process
        if (redirect_file != NULL){
            int fd = open(redirect_file, O_CREAT | O_WRONLY | O_TRUNC, 0666); // Give right access to the file
            if (fd < 0){
                print_error();
                exit(1);
            }

            if(dup2(fd, STDOUT_FILENO) < 0) {print_error(); exit(1);}
            if(dup2(fd, STDERR_FILENO) < 0) {print_error(); exit(1);}
            close(fd);
        }

        // Try each of directories in path
        for (int i = 0; i < path_count; i++){
            char cmd_path[512];
            snprintf(cmd_path, sizeof(cmd_path), "%s/%s", path[i], args[0]);
            if (access(cmd_path, X_OK) == 0){
                execv(cmd_path, args);
                break;
            }
        }

        print_error();
        exit(1);
    } 

    return pid;
}

// Helper for trimming

char *trim(char *str){
    while (str != NULL && (*str == ' ' || *str == '\t')) str++;
    if (str == NULL || *str == '\0') return str;

    char *end = str + strlen(str) -1;
    while (end > str && (*end == ' ' || *end =='\t')) end--;
    *(end + 1) = '\0';

    return str;
}

int main(int argc, char *argv[]){
    // Initialize default path
    path[0] = strdup("/bin");
    path_count = 1;

    char *line = NULL;
    size_t len = 0;

    while(1) {
        printf("wish> ");
        fflush(stdout);

        if (getline(&line, &len, stdin) == -1){
            free(line);
            exit(0); // The end of the command prompt
        }

        // Removing the newline
        line[strcspn(line, "\n")] = '\0';

        // Skip possible empty lines
        char *tline = trim(line);
        if(tline == NULL || *tline == '\0') continue;

        // Parallel command handling

        char temp[1024];
        if (strlen(tline) >= sizeof(temp)){
            // IN case the line is too long for the buffer
            print_error();
            continue;
        }
        strcpy(temp,line);

        char *parallel_cmds[MAX_PARALLEL];
        int pc = 0;

        char *part = strtok(temp, "&");
        while (part != NULL && pc < MAX_PARALLEL){
            parallel_cmds[pc++] = trim(part);
            part = strtok(NULL,"&");
        }

        pid_t pids[MAX_PARALLEL];
        int pid_count = 0;
        
        // Processing each command individually

        for (int c = 0; c < pc; c++){

            if (parallel_cmds[c] == NULL) continue;
            
            char cmd_copy[1024];
            if (strlen(parallel_cmds[c]) >= sizeof(cmd_copy)) {
                print_error();
                continue;
            }
            strcpy(cmd_copy, parallel_cmds[c]);
            trim(cmd_copy);

            // Check for redirection
            char *redirect_file = NULL;
            char *redirect_pos = strchr(cmd_copy, '>');

            if(redirect_pos != NULL){
                
                // Making sure there is only one '>'
                char *second = strchr(redirect_pos + 1, '>');
                if (second != NULL) {print_error(); continue; }

                // Split around the >
                *redirect_pos = '\0'; // left side stays as the command
                redirect_pos++;
                
                redirect_pos = trim(redirect_pos);

                if (redirect_pos == NULL || *redirect_pos == '\0') {print_error(); continue; }


            // Only one filename
            char *rf = strtok(redirect_pos, " \t");
            if (!rf || strtok(NULL, " \t") != NULL) {
                print_error();
                continue;
            }

            redirect_file = rf;

            trim(cmd_copy);
        }

            // Tokenize inputs
            char *args[MAX_ARGS];
            int arg_count = 0;
            char *token = strtok(cmd_copy, " \t");
            while(token != NULL && arg_count < MAX_ARGS-1){
                args[arg_count++] = token;
                token = strtok(NULL, " \t");
            }

            args[arg_count] = NULL;

            if(arg_count == 0) continue; // No command


            // BUILT-IN COMMANDS
            // exit:
            if (strcmp(args[0], "exit")==0){
                if (arg_count != 1){
                    print_error();
                    continue;
                }
                free(line);
                for (int i = 0; i < path_count; ++i) free(path[i]); // Free path entries
                exit(0);
            }
            
            // cd:
            if (strcmp(args[0], "cd") == 0){
                if (arg_count != 2 || chdir(args[1]) != 0){
                    print_error();
                }
                continue;
            }

            // path:
            if (strcmp(args[0], "path") == 0){
                // Clear old path
                for (int i = 0; i < path_count; i++){
                    free(path[i]);
                    path[i] = NULL;
                }
                path_count = 0;

                for (int i = 1; i<arg_count && path_count < MAX_PATH; i++){
                    path[path_count++] = strdup(args[i]);
                }
                continue;
            }

            pid_t child = execute_command(args, redirect_file);
            if (child > 0 && pid_count < MAX_PARALLEL){
                pids[pid_count++] = child;
            } else if ( child == -1){
                continue;
            }
        }

        // Wait for all the parallel processes
        for (int i=0; i < pid_count; i++){
            waitpid(pids[i],NULL,0);
        }
    }
    free(line);
    for (int i = 0; i < path_count; i++) free(path[i]);
    return 0;
}
