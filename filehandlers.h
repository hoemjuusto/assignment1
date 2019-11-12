//
// Created by juuso on 18.10.2019.
//

#ifndef ASSIGNMENT1_FILEHANDLERS_H
#define ASSIGNMENT1_FILEHANDLERS_H
int write_to_file(const char *filename, char *clean_text);

char *read_file(const char *filename);

void wlog(FILE *lp, const char *logtext);
#endif //ASSIGNMENT1_FILEHANDLERS_H

