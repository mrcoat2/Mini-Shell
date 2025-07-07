#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEAFULT_BUFF 1024

int starts_with (char* pattern, char* str) {
    if (strnicmp(pattern,str,strlen(pattern))==0) {return 1;}

    return 0;
} 

char* search(char* pattern, const char* fname) {
    char line[DEAFULT_BUFF];

    FILE* req_ptr;
    req_ptr = fopen(fname, "r");

    while(fgets(line,DEAFULT_BUFF, req_ptr)) {
        if (starts_with(pattern, line)) {
            strtok(line, " ");
            char *found = strtok(NULL, "\n");
            fclose(req_ptr);
            return found;

        }
    }
    fclose(req_ptr);
    return NULL;
}