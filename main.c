#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include "filehandlers.h"
#include "file_edits.h"
#include <signal.h>
#define FNAMESIZE 50

void signalHandler( int signalValue ); /* prototype */
void  INThandler(int signalValue);

pid_t pid;

int main(void)
{
    char log[] = "v.log";
    fopen(log, "w");  //opens and erases log file
    while(1) {
        int fd[2];  //fd[0] for input, f[1] for output
        pipe(fd);
        pid = fork();  //creation of child-process
        if (pid == -1) {
            perror("<parent> Fork failed\n");
            exit(1);
        }
        if (pid > 0) {
            // Only parent process would come here
            signal(SIGINT, INThandler);
            close(fd[0]);  //closes the input of the pipe for parent
            char filepath[FNAMESIZE];
            wlog(log, "<parent> Asking filepath from user\n");
            printf("<parent> Give filepath (submit with enter): ");
            fflush(stdout);
            scanf("%s", filepath);
            fflush(stdout);
            wlog(log, "<parent> Outputting filepath through pipe to child\n");
            write(fd[1], filepath, FNAMESIZE);
            close(fd[1]);
            wait(NULL);  //Waits until child is done and terminated
        } else {
            // Only child process would come here
            close(fd[1]); //closes the output of the pipe for child
            wlog(log, "<child> I'm alive\n");
            char filepath[FNAMESIZE];
            read(fd[0], filepath, FNAMESIZE);  //child doesn't proceed past this until there is something to read
            close(fd[0]);
            printf("<child> Got a filepath %s from parent\n", filepath);
            wlog(log, "<child> Received filepath through pipe\n");
            char *original = read_file(filepath);
            char *clean = delete_comments(original);
            char add[] = "clean";
            write_to_file(edit_name(filepath, add), clean);
            exit(0);
        }
    }
    return 0;
}

void signalHandler( int signalValue )
{
    int response;

    printf( "%s%d%s\n%s","\nInterrupt signal ( ", signalValue, " ) received.",
            "Do you wish to continue ( 1 = yes or 2 = no )? \n" );

    scanf("%d", &response);
    fflush(stdout);
    if ( response == 1 ) {
        signal( SIGINT, signalHandler );
    }
    else {
        signal(SIGINT, INThandler);
    }

}


void  INThandler(int signalValue)
{
    printf("\nCtrl-C command detected!\n");
    signal(signalValue, SIG_IGN);
    kill(pid, SIGTERM);
    sleep(3);
    int status;
    waitpid(pid, &status, WNOHANG);
    if(WIFEXITED(status)){
        printf("child exited normally\n");
        fflush(stdout);
    } else{
        printf("erroneous exit of child\n");
        fflush(stdout);
    }
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
    exit(0);
}
