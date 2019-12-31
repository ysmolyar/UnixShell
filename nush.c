#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <assert.h>
#include "svec.h"
#include "tokens.h"

void parse_boolean(svec* first, svec* second, int boolean);
void parse_background(svec* command);
void parse_redir_output(svec* command, char* dest_file);
void parse_redir_input(svec* command, char* src_file);
void parse_pipe(svec* first, svec* second);
void execute(svec* cmd);
void execute_toks(svec* toks);
int main(int argc, char* argv[]);

void
check_rv(int rv) {
    if (rv == -1) {
        perror("fail");
        exit(1);
    }
}

void parse_cd(char* new_dir) {
    chdir(new_dir);
}


//int boolean = 0 for OR (||) operator
//int boolean = 1 for AND (&&) operator
void
parse_boolean(svec* first, svec* second, int boolean) {
    
    int cpid;

    if ((cpid = fork())) {
        // parent process
        // Child may still be running until we wait.

        int status;
        waitpid(cpid, &status, 0);
        
        //child exited and succeeded with zero exit code
        if (WIFEXITED(status)) {
           if (boolean) { //if AND operator is being parsed
               if (WEXITSTATUS(status) == 0) {
                   execute_toks(second);
               }
           }
           else { //if OR operator is being parsed
               if (WEXITSTATUS(status) != 0) {
                   execute_toks(second);
               }
           }
           
        }

    }
    else {
        // child process
        
        // The argv array for the child.
        // Terminated by a null pointer.
        char* argv[first->size];
        argv[first->size] = 0; //null pointer

        for (int ii = 0; ii < first->size; ++ii) {
            argv[ii] = first->data[ii];
        }

        char* cmd_name = argv[0];
        execvp(cmd_name, argv);
        exit(0);
    }
}


void 
parse_background(svec* cmd) {
    
    int cpid;
    //don't want to wait for child process
    //so it can run in the background
    if ((cpid = fork())) {
        //do nothing
    } else {
        char* argv[cmd->size];
        argv[cmd->size] = 0; //null pointer

        for (int ii = 0; ii < cmd->size; ++ii) {
            argv[ii] = cmd->data[ii];
        }

        char* cmd_name = argv[0];

        execvp(cmd_name, argv);
        exit(0);
    }
}

void 
parse_redir_output(svec* command, char* dest) {
    
    int cpid;
    
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
        
    } else {
        
        FILE* dest_file = fopen(dest, "w");
        if (!dest_file) {
            printf("Couldn't find file at path %s\n", dest);
        }
        int fd = fileno(dest_file);
        dup2(fd, 1);
        
        fclose(dest_file);
        execute_toks(command);
    }
}

void 
parse_redir_input(svec* command, char* src) {
    
    int cpid;
    
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
        
    } else {
        
        FILE* in_file = fopen(src, "r");
        
        if (!in_file) {
            printf("Couldn't find file at path %s\n", src);
        }
        int fd = fileno(in_file);
        dup2(fd, 0);
        
        fclose(in_file);
        execute_toks(command);
    }
}

void
parse_pipe(svec* first, svec* second) {
    
    int cpid;
    int pipe_fds[2]; //read is pipe_fds[0], write is pipe_fds[1]
    
    if ((cpid = fork())) {
        // parent process
        // Child may still be running until we wait.
        int status;
        waitpid(cpid, &status, 0); //waiting for child process
        
    }
    else {
        // child process
        int rv = pipe(pipe_fds);
        check_rv(rv);
        
        int ccpid;
        if ((ccpid = fork())) {
            //in child-parent
            int status2;
            dup2(pipe_fds[0], 0); //hook pipe to stdin
            close(pipe_fds[1]); //close write end of pipe
            execute_toks(second); //execute second command
            waitpid(ccpid, &status2, 0); //waiting for child-child
        }
        else {
            //in child-child
            dup2(pipe_fds[1], 1); //hook pipe to stdout
            close(pipe_fds[0]); //close read end of pipe
            execute_toks(first); //execute first command
        }
    }
}

void
execute(svec* cmd)
{
    
    int cpid;
    
    if (strcmp(cmd->data[0], "cd") == 0) {
        parse_cd(cmd->data[1]);
        return;
    }
    
    if (strcmp(cmd->data[0], "exit") == 0) {
        exit(0);
    }

    if ((cpid = fork())) {
        // parent process
        // Child may still be running until we wait.

        int status;
        waitpid(cpid, &status, 0);

    }
    else {
        // child process
        
        // The argv array for the child.
        // Terminated by a null pointer.
        char* argv[cmd->size];
        argv[cmd->size] = 0; //null pointer

        for (int ii = 0; ii < cmd->size; ++ii) {
            argv[ii] = cmd->data[ii];
        }

        char* cmd_name = argv[0];

        execvp(cmd_name, argv);
        exit(0);
    }
}

void
execute_toks(svec* toks)
{
    int idx;
    if (svec_contains(toks, ";")) {
        idx = svec_first_index_of(toks, ";");
        
        svec* first_half = subvec(toks, 0, idx);
        svec* second_half = subvec(toks, idx + 1, toks->size);

        execute_toks(first_half);
        execute_toks(second_half);
        
        free_svec(first_half);
        free_svec(second_half);
        
    } else if (svec_contains(toks, "|")) {
        idx = svec_first_index_of(toks, "|");
        
        svec* first_half = subvec(toks, 0, idx);
        svec* second_half = subvec(toks, idx + 1, toks->size);
        
        parse_pipe(first_half, second_half);
        
        free_svec(first_half);
        free_svec(second_half);
        
        
    } else if (svec_contains(toks, "||") || svec_contains(toks, "&&")) {
        int boolean = 0;
        idx = 0; //intializing values to make clang-check happy
        if (svec_contains(toks, "||")) {
            boolean = 0;
            idx = svec_first_index_of(toks, "||");
        }
        else if (svec_contains(toks, "&&")) {
            boolean = 1;
            idx = svec_first_index_of(toks, "&&");
        }
        
        
        svec* first_half = subvec(toks, 0, idx);
        svec* second_half = subvec(toks, idx + 1, toks->size);
        
        parse_boolean(first_half, second_half, boolean);
        
        free_svec(first_half);
        free_svec(second_half);
    } else if (svec_contains(toks, "&")) {
        idx = svec_first_index_of(toks, "&");
        
        svec* command = subvec(toks, 0, idx);
        
        parse_background(command);
        
        free_svec(command);
    } else if (svec_contains(toks, ">")) {
        idx = svec_first_index_of(toks, ">");
        
        svec* command = subvec(toks, 0, idx);
        
        char* dest_file = toks->data[idx + 1];
        
        parse_redir_output(command, dest_file);
        
        free_svec(command);
    } else if (svec_contains(toks, "<")) {
        idx = svec_first_index_of(toks, "<");
        
        svec* command = subvec(toks, 0, idx);
        
        char* in_file = toks->data[idx + 1];
        
        parse_redir_input(command, in_file);
        
        free_svec(command);
    }
    
    else {
        
        execute(toks);

    }
}

int
main(int argc, char* argv[])
{
    svec* toks;
    char cmd[256];
    
    if (argc == 1) {
        //loop for user input
        while(1) {
            printf("nush$ ");
            fflush(stdout);
            char* status = fgets(cmd, 256, stdin);
            
            //exit program if EOF is read
            if (status == 0) {
                printf("\n");
                exit(0);
            }
            
            // function from tokens.c to parse input command into svec* 
            toks = tokenize(cmd); 
            
            if (toks->size != 0) {
                //if exit command is read, exit program
                if (strcmp(toks->data[0], "exit") == 0) {
                    free_svec(toks);
                    exit(0);
                } else if (strcmp(toks->data[0], "cd") == 0) {
                    parse_cd(toks->data[1]);
                    break;
                }
                execute_toks(toks);
            }
            
            // free toks from memory before next iteration
            free_svec(toks);
        }
    }
    else {
        //if path to script is program argument
        FILE* file = fopen(argv[1], "r");
        
        if (!file) {
            printf("Couldn't find file at path %s\n", argv[1]);
        }
        while (fgets(cmd,256,file) != NULL) {
            
            toks = tokenize(cmd);
            execute_toks(toks);
            free_svec(toks);
        }
        fclose(file);
    }

    return 0;
}
