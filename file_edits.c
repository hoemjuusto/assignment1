//
// Created by juuso on 15.10.2019.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define BUFFER_SIZE 150


char *delete_comments(char *input)
{
    int size = strlen(input);
    char *clean = malloc(size + 100);
    int multiline = 0;
    int singleline = 0;
    int cleanindex = 0;

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

int isEmpty(const char *str)
{
    char ch;

    do
    {
        ch = *(str++);

        // Check non whitespace character
        if(ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '\0')
            return 0;

    } while (ch != '\0');

    return 1;
}


void removeEmptyLines(char *src)
{
    char buffer[BUFFER_SIZE];
    char ret[strlen(src)];
    for(int i = 0; i < strlen(src); i++){
        if(src[i] == '\n'){
            if(!isEmpty(buffer)){
                strcat(ret, buffer);
                strncat(ret, "\n", 1);
            }
            strcpy(buffer, "");
        }else{
            strncat(buffer, &src[i], 1);
        }
    }
    strcpy(src, ret);
}

char *edit_name(char *filename, char *add){
    char *original = filename;
    char *result = malloc(strlen(filename) + strlen(add) + 1);
    char *ret = result;
    int i = 0;
    while(filename[i] != '.' && filename[i] != '\0'){
        *result = filename[i];
        result++;
        *result = '\0';
        original++;
        i++;
    }
    *result = '_'; result++; *result = '\0';
    strcat(ret, add);
    strcat(ret, original);
    return ret;
}
