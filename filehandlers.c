//
// Created by juuso on 15.10.2019.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *read_file(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("While opening file \"%s\" for reading, encountered an error: \n", filename);
        fflush(stdout);
        perror("");
    }
    char *buf = NULL;
    unsigned int count = 0;
    const unsigned int ReadBlock = 100;
    unsigned int n;
    do {
        buf = realloc(buf, count + ReadBlock + 1);
        n = fread(buf + count, 1, ReadBlock, f);
        count += n;
    } while (n == ReadBlock);
    buf[count] = 0;
    fclose(f);
    return buf;
}

int write_to_file(const char *filename, char *clean_text)
{
    printf("writing to a file %s\n", filename);
    FILE *f = fopen(filename, "w");
    int ret = 1;
    if (!f) {
        printf("While opening file \"%s\" for writing, encountered an error: \n", filename);
        fflush(stdout);
        perror("");
        ret = 0;
    }
    if (fprintf(f, "%s", clean_text) < 0) {
        ret = 0;
    }
    fclose(f);
    return ret;
}
void wlog(FILE *lp, const char *logtext){
    if (lp == NULL) { printf("While opening log file for writing, encountered an error: \n"); perror("");}
    fprintf(lp, "%s", logtext);
    fflush(lp);
}