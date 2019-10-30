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
#define MAX_INPUT_FILES 10


/*void  INThandler(int signalValue);

pid_t pid;
int keepRunning = 1;

int main(void)
{
    FILE *lp;
    char log[] = "v.log";
    lp = fopen(log, "w");  //opens and erases log file
    while(keepRunning) {
        signal(SIGINT, SIG_IGN);  //for the child process to ignore normal exit with CTRL + C and exit more gracefully with parent instructions
        int fd[2];  //fd[0] for input, f[1] for output
        pipe(fd);
        create_child(pid, fd, FNAMESIZE, lp);
        if (pid > 0) {
            // Only parent process would come here
            signal(SIGINT, INThandler);
            close(fd[0]);  //closes the input of the pipe for parent
            char filepath[FNAMESIZE];
            wlog(lp, "<parent> Asking filepath from user\n");
            printf("<parent> Give filepath(s) (submit with enter): ");
            fflush(stdout);
            scanf("%s", filepath);
            fflush(stdout);
            wlog(lp, "<parent> Outputting filepath through pipe to child\n");
            write(fd[1], filepath, FNAMESIZE);
            close(fd[1]);
            wait(NULL);  //Waits until child is done and terminated
        } else {
            // Only child process would come here
            close(fd[1]); //closes the output of the pipe for child
            wlog(lp, "<child> I'm alive\n");
            char filepath[FNAMESIZE];
            read(fd[0], filepath, FNAMESIZE);  //child doesn't proceed past this until there is something to read
            close(fd[0]);
            printf("<child> Got a filepath %s from parent\n", filepath);
            wlog(lp, "<child> Received filepath through pipe\n");
            char *original = read_file(filepath);
            char *clean = delete_comments(original);
            char add[] = "clean";
            write_to_file(edit_name(filepath, add), clean);
            exit(0);
        }
    }
}

void  INThandler(int signalValue)
{
    printf("\nCtrl-C command detected!\n");
    keepRunning = 0;
    kill(pid, SIGTERM);
    sleep(EXITTIME);
    if(kill(pid, SIGKILL)==-1){
        printf("Child did not exit in set %d s exit time, so the parent killed it", EXITTIME);
    }
    int status;
    waitpid(pid, &status, WNOHANG);
    if (WIFEXITED(status))
        printf("Child exited normally with %d\n", WEXITSTATUS(status));

    if (WIFSIGNALED(status))
        printf("Child was terminated by signal %d\n", WTERMSIG(status));

    if (WIFSTOPPED(status))
        printf("Child was stopped by signal %d\n", WSTOPSIG(status));

    waitpid(pid, &status, 0);
    exit(0);
}

char **separate_filenames(char *input){
    char *fname = malloc(FNAMESIZE*8);
    const char *delim = "_";
    fname = strtok(input, delim);
    while(fname) {
        fname = strtok(input, delim);
    }
}*/