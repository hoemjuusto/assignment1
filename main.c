#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include "filehandlers.c"
#include "file_edits.c"

int main(void)
{
    int fd[2];
    pipe(fd);
    pid_t pid;
    pid = fork();  //creation of child-process
    if (pid == -1) {
        perror("<parent> Fork failed");
        return -1;
    }
    if (pid > 0) {
        // Only parent process would come here
        char *filepath = malloc(50);
        printf("filepath (submit with enter): ");
        fflush(stdout);
        scanf("%s", filepath);
        fflush(stdout);
        write(fd[1], filepath, (strlen(filepath)+1));
        wait(NULL);  //Waits until child is done and terminated
        printf("<parent> exiting\n");
        fflush(stdout);
    } else {
        // Only child process would come here
        if (fd[0]!=0)  //if parent has outputted filepath
        {
            char *fpath = malloc(100);
            read(fd[0], fpath, 100);
            fd[0]=0;  //empties the
            char *original = read_file(fpath);   //"/home/juuso/CLionProjects/assignment1/alkuperanen.txt"
            char *clean = delete_comments(original);
            write_to_file("/home/juuso/CLionProjects/assignment1/alkuperanenclean.txt", clean);
        }
        exit(0);
    }
    return 0;
}
