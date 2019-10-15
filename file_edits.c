//
// Created by juuso on 15.10.2019.
//
#include <string.h>
#include <stdlib.h>


char *delete_comments(char *input)
{
    int size = strlen(input);
    char *clean = malloc(size + 100);
    int multiline = 0;
    int cleanindex = 0;
    int singleline = 0;
    for (int i = 0; i < size; i++) {
        if ((input[i] == '/') && (input[i + 1] == '*')) {
            multiline = 1;
        }
        else if ((input[i] == '*') && (input[i + 1] == '/')&&(multiline==1)) {
            multiline = 0;
            i += 1;
        }
        else if ((input[i] == '/') && (input[i + 1] == '/')) {
            singleline = 1;
        }
        else if ((input[i] == '\n')&&(singleline==1)) {
            singleline = 0;
        }
        else if ((multiline == 0)&&(singleline==0)) {
            clean[cleanindex] = input[i];
            cleanindex += 1;
        }
    }
    clean[cleanindex] = 0;
    free(input);
    return clean;

}