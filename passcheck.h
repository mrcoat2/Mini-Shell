#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"

#define DEAFULT_BUFF 1024

const char req_file[] = "pass_req.txt";
const char _symbols[] = "~!@#$^&*()_+-=[]\\{}|;':,./<>?";

const char default_req[] = "# Passwords length\nLENGTH 6\n# If the password must have symbols\nSYMBOLS yes\n# If the password must have upper and lower Case letters\nUPANDLOW yes";

// Returns 1 if the password meets requirement 0 if not
int check_passwd(char *passwd){
    int pass_options[3];
    read_req(pass_options);
    if (strlen(passwd) < pass_options[0]) {
        return -1;
    } 

    // printf("%d", pass_options[2]);
    if (pass_options[1] == 121) {
        // printf("yes");
        for(int i = 0; passwd[i]; i++){
            if (passwd[i] != tolower(passwd[i])) {
                goto symbols;
            }
        }   
        return -1;
    }
    symbols:
    if (pass_options[2] == 121) {
        // printf("yes");
        for(int i = 0; _symbols[i]; i++){
            if (strchr(passwd,_symbols[i]) != NULL) {
                goto good;
            }
        }   
        return -1;
    }
    good:

    return 1;
}


//checks if there is a file if not creates it
void check_file() {
    FILE* req_ptr;
    req_ptr = fopen(req_file, "r");

    int logged_in = -1;
    if (req_ptr==NULL) {
        FILE* fptr;
        fptr = fopen(req_file, "w+"); // Create a file for writing
        if (fptr == NULL) {
            printf("Error creating req file");
            exit(1);
        } else {
            fprintf(fptr, default_req);
            fclose(fptr);
        }
        
    } else {
        fclose(req_ptr);
    }
}

int read_req (int option[3]) {
    check_file();
    

    int pass_length = 8;
    int symbols = 0;
    int upAndLow = 0;

    pass_length = atoi(search("LENGTH",req_file));
    symbols = search("SYMBOLS",req_file)[0];
    upAndLow = search("UPANDLOW",req_file)[0];
    // printf("%c", search("SYMBOLS",req_file)[0]);
    
    option[0] = pass_length;
    option[1] = symbols;
    option[2] = upAndLow;
    
    return 0;
}
