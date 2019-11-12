#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "filehandlers.h"
#include "file_edits.h"
#include "clean-process.h"
#define FNAMESIZE 50
#define EXITTIME 3
#define LOGMSGESIZE 100


void  INThandler(int signalValue); /*prototype signal handler, implementation in the end*/
void welcome_msg();

int pid = 0;
int keepRunning = 1;
FILE *lp;  //log-file pointer
char logbuffer[100];

int main(void)
{
    signal(SIGINT, INThandler);
    welcome_msg();
    char log[] = "v.log";
    lp = fopen(log, "w");  //opens and erases log file
    if (lp == NULL) { printf("While opening log file for writing, encountered an error: \n"); perror("");}
    dup2(fileno(lp), STDERR_FILENO);  //directs error messages to log file
    while(keepRunning) {
        char *input = calloc(MAX_INPUT+1, 8);
        wlog(lp, "<parent> Asking filepath(s) from user\n");
        printf("<parent> Give filepath(s) (submit with enter, q to quit):\n");
        fgets(input, MAX_INPUT, stdin);
        char *fname;
        char delim[] = "\n ";
        fname = strtok(input, delim);
        if(strcmp(fname,"q") == 0){
            printf("Exiting\n");
            exit(0);
        }
        while(fname != NULL) {
            clean(fname, FNAMESIZE, &pid, lp, LOGMSGESIZE);
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
    //keepRunning = 0;
    printf("\nCtrl-C command detected!\n");
    wlog(lp, "Ctrl-C command detected!\n");
    kill(pid, SIGTERM);
    sleep(EXITTIME);
    int status;
    waitpid(pid, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Child exited normally with signal %d\n", WEXITSTATUS(status));
        snprintf(logbuffer, LOGMSGESIZE, "Child exited normally with signal %d\n", WEXITSTATUS(status));
        wlog(lp, logbuffer);
    }
    if (WIFSIGNALED(status)) {
        printf("Child was terminated by signal %d\n", WTERMSIG(status));
        snprintf(logbuffer, LOGMSGESIZE, "Child was terminated by signal %d\n", WTERMSIG(status));
        wlog(lp, logbuffer);
    }
    if (WIFSTOPPED(status)) {
        printf("Child was stopped by signal %d\n", WSTOPSIG(status));
        snprintf(logbuffer, LOGMSGESIZE, "Child was stopped by signal %d\n", WSTOPSIG(status));
        wlog(lp, logbuffer);
    }
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
    wlog(lp, "<parent> Exiting\n");
    printf("Exiting\n");
    exit(0);
}

void welcome_msg(){
    printf("#####################WELCOME TO CLEANF-EDITOR!#############################\n"
           "SUBMIT BELOW THE FILEPATHS TO FILES YOU WANT TO GET CLEANED OF ANY COMMENTS\n"
           "AND EMPTY LINES. LOG-FILE IS NAMED V.LOG AND SHOULD BE FOUND ON THE SAME\n"
           "PLACE AS THE SOURCE CODE. IT CONTAINS RUNTIME INFO AND POSSIBLE ERRORS.\n"
           "YOU CAN EXIT BY CTRL + c, OR BY TYPING q\n\n");
}

