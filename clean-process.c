//
// Created by juuso on 31.10.2019.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "filehandlers.h"
#include "file_edits.h"




int clean(char *fname, size_t FNAMESIZE, pid_t *pid, FILE *lp, size_t LOGMSGESIZE){
    //logging
    char logbuffer[LOGMSGESIZE];
    snprintf(logbuffer, LOGMSGESIZE, "Processing given file with name: %s\n", fname);
    wlog(lp, logbuffer);

    int fd[2];  //fd[0] for input, f[1] for output
    pipe(fd);
    *pid = fork();
    if (*pid == -1) {
        perror("<parent> Fork failed\n");
        return 0;
    }
    if(*pid == 0) {
        //only child would enter here
        signal(SIGINT, SIG_IGN);  //ignore CTRL + C as child and follow instructions from parent
        close(fd[1]); //closes the output of the pipe for child
        char filepath[FNAMESIZE];
        read(fd[0], filepath, FNAMESIZE);  //child doesn't proceed past this until there is something to read
        close(fd[0]);
        //logging
        snprintf(logbuffer, LOGMSGESIZE, "<child> Got a filepath %s from parent\n", filepath);
        wlog(lp, logbuffer);

        char *original = read_file(filepath);
        if(original==NULL){exit(0);}
        char *clean = delete_comments(original);
        removeEmptyLines(clean);
        char add[] = "clean";
        char *clean_filename = edit_name(filepath, add);
        if(write_to_file(clean_filename, clean)==0){
            free(clean_filename); exit(0);
        }
        //logging
        snprintf(logbuffer, LOGMSGESIZE, "<child> Managed to write a clean file to %s\n", clean_filename);
        wlog(lp, logbuffer);

        free(clean_filename);
        wlog(lp, "<child> Exiting\n");
        exit(0);
    }else if(*pid > 0) {
        /*Only parent would enter here
         * logging*/
        wlog(lp, "Child got created\n"); //logging
        snprintf(logbuffer, LOGMSGESIZE, "Child pid is: %d\n", *pid);
        wlog(lp, logbuffer);
        snprintf(logbuffer, LOGMSGESIZE, "Parent pid is: %d\n", getppid());
        wlog(lp, logbuffer);

        close(fd[0]);  //closes the input of the pipe for parent
        wlog(lp, "<parent> Outputting filepath through pipe to child\n");
        if(write(fd[1], fname, FNAMESIZE) < 0){ perror("<parent> writing to pipe failed"); return 0;}
        close(fd[1]);
        wait(pid);  //Waits until child is done and terminated
    }
    return 1;
}
