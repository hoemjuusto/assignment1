#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "filehandlers.h"
#include "file_edits.h"
#include "child-process.h"
#define FNAMESIZE 50
#define EXITTIME 3
#define MAX_INPUT 50
#define MAX_LOG 100


void  INThandler(int signalValue); /*prototype signal handler, implementation in the end*/

pid_t pid;
int keepRunning = 1;
FILE *lp;  //log-file pointer
char logbuffer[100];

int main(void)
{
    signal(SIGINT, INThandler);
    char log[] = "v.log";
    lp = fopen(log, "w");  //opens and erases log file
    while(keepRunning) {
        char *input = malloc(MAX_INPUT*8);
        wlog(lp, "<parent> Asking filepath(s) from user\n");
        printf("<parent> Give filepath(s) (submit with enter):\n");
        fgets(input, MAX_INPUT, stdin);
        char *fname;
        char delim[] = "\n ";
        fname = strtok(input, delim);
        while(fname != NULL) {
            //logging
            snprintf(logbuffer, MAX_LOG, "Processing given file with name: %s\n", fname);
            wlog(lp, logbuffer);

            int fd[2];  //fd[0] for input, f[1] for output
            pipe(fd);
            pid = fork();
            if (pid == -1) {
                perror("<parent> Fork failed\n");
                exit(1);
            }
            if(pid == 0) {
                //only child would enter here
                signal(SIGINT, SIG_IGN);  //ignore CTRL + C as child and follow instructions from parent
                close(fd[1]); //closes the output of the pipe for child
                char filepath[FNAMESIZE];
                read(fd[0], filepath, FNAMESIZE);  //child doesn't proceed past this until there is something to read
                close(fd[0]);
                //logging
                snprintf(logbuffer, MAX_LOG, "<child> Got a filepath %s from parent\n", filepath);
                wlog(lp, logbuffer);

                char *original = read_file(filepath);
                char *clean = delete_comments(original);
                removeEmptyLines(clean);
                char add[] = "clean";
                char *clean_filename = edit_name(filepath, add);
                write_to_file(clean_filename, clean);
                //logging
                snprintf(logbuffer, MAX_LOG, "<child> Managed to write a clean file to %s\n", clean_filename);
                wlog(lp, logbuffer);

                free(clean_filename);
                exit(0);
            }else if(pid > 0) {
                //logging
                wlog(lp, "Child got created\n"); //logging
                snprintf(logbuffer, MAX_LOG, "Child pid is: %d\n", pid);
                wlog(lp, logbuffer);
                snprintf(logbuffer, MAX_LOG, "Parent pid is: %d\n", getppid());
                wlog(lp, logbuffer);

                close(fd[0]);  //closes the input of the pipe for parent
                wlog(lp, "<parent> Outputting filepath through pipe to child\n");
                write(fd[1], fname, FNAMESIZE);
                close(fd[1]);
                wait(NULL);  //Waits until child is done and terminated
            }
            //moves to next filename
            fname = strtok(NULL, delim);
        }
        free(input);
        free(fname);
    }
    fclose(lp);
    return 0;
}

void  INThandler(int signalValue)
{
    keepRunning = 0;
    printf("\nCtrl-C command detected!\n");
    wlog(lp, "Ctrl-C command detected!\n");
    kill(pid, SIGTERM);
    sleep(EXITTIME);
    int status;
    waitpid(pid, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Child exited normally with signal %d\n", WEXITSTATUS(status));
        snprintf(logbuffer, MAX_LOG, "Child exited normally with signal %d\n", WEXITSTATUS(status));
        wlog(lp, logbuffer);
    }
    if (WIFSIGNALED(status)) {
        printf("Child was terminated by signal %d\n", WTERMSIG(status));
        snprintf(logbuffer, MAX_LOG, "Child was terminated by signal %d\n", WTERMSIG(status));
        wlog(lp, logbuffer);
    }
    if (WIFSTOPPED(status)) {
        printf("Child was stopped by signal %d\n", WSTOPSIG(status));
        snprintf(logbuffer, MAX_LOG, "Child was stopped by signal %d\n", WSTOPSIG(status));
        wlog(lp, logbuffer);
    }
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
    exit(0);
}

