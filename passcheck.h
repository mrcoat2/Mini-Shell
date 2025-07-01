#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEAFULT_BUFF 1024

char req_file[] = "pass_req.txt";

// Returns 1 if the password meets requirement 0 if not
int check_passwd(char *passwd){
    int pass_options[3];
    read_req(pass_options);
    if (strlen(passwd) < pass_options[0]) {
        return -1;
    } 
    return 1;
}


//checks if there is a file if not creates it
void check_file() {
    FILE* req_ptr;
    req_ptr = fopen(req_file, "r");

    int logged_in = -1;
    if (req_ptr==NULL) {
        FILE* fptr;
        fptr = fopen(req_file, "w"); // Create a file for writing
        if (fptr == NULL) {
            printf("Error! opening file");
            exit(1);
        } else {
            fclose(fptr);
        }
        
    } else {
        fclose(req_ptr);
    }
}

int starts_with (char* pattern, char* str) {
    if (strncmp(pattern,str,strlen(pattern))==0) {return 1;}

    return 0;
} 

int read_req (int option[3]) {
    check_file();

    FILE* req_ptr;
    req_ptr = fopen(req_file, "r");
    char line[DEAFULT_BUFF];

    int pass_length = 8;
    int symbols = 0;
    int upAndLow = 0;

    while(fgets(line,DEAFULT_BUFF,req_ptr)) {
        if (starts_with("LENGTH", line)) {
            strtok(line, " ");
            char *chnumber = strtok(NULL, "\n");

            pass_length = atoi(chnumber);

        }
    }

    option[0] = pass_length;
    option[1] = symbols;
    option[2] = upAndLow;
    
    return 0;
}
