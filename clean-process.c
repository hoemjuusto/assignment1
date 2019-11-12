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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
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

        //MEMORY-MAPPING, O_RDONLY, S_IRUSR | S_IWUSR : open for read only,
        int f = open(filepath, O_RDONLY, S_IRUSR | S_IWUSR);
        struct stat sb;

        if(fstat(f, &sb) == -1){
            perror("While doing memory mapping couldn't get the file size.\n");
        }
        printf("file size is %ld\n", sb.st_size);

        //PROT_READ ONLY GIVES US PERMISSION TO READ, AS IT SHOULD BE
        char *file_in_memory = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, f, 0);
        close(f); //file pointer can be closed immediately after mapping

        if(file_in_memory==MAP_FAILED)
        {
            printf("memory mapping failed\n ");
        }
        char *temp = delete_comments(file_in_memory);
        char *clean = removeEmptyLines(temp);
        //ADDING .CLEAN EXTENSION
        char add[] = "clean";
        char *clean_filename = edit_name(filepath, add);
        //WRITING THE CLEAN FILE
        if(write_to_file(clean_filename, clean) != 0){
            printf("Writing to file failed\n");
        }
        //logging
        snprintf(logbuffer, LOGMSGESIZE, "<child> Managed to write a clean file to %s\n", clean_filename);
        wlog(lp, logbuffer);

        free(clean_filename);
        //UNMAPPING THE FILE IN MEMORY
        if ( file_in_memory != MAP_FAILED ) {
            if ( munmap( file_in_memory, sb.st_size )  == -1) {
                perror("Munmap failed with error: ");
            }
        }
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
        wait(NULL);  //Waits until child is done and terminated
    }
    return 1;
}

