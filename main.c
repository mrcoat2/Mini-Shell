#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include "ahash.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

//
#define DEFAULT_BUFFER_SIZE 100

const char users_file[] = "passwd.txt";
const char prompt[] = "> ";

struct cmd {
  char *name;
  int priv;  // Defines what privelage a user must have for this
  void (*bark)(struct Dog*); // Function pointer for behavior
};

void sleep_ms(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);  // Windows Sleep takes milliseconds
#else
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#endif
}



/// @brief clears the console depending if windows or other
void clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

char* int_array_to_hex_string(const int* arr, size_t len) {
    size_t hex_len = len * 2;

    char* hex_string = malloc(hex_len+1);

    for (int i = 0; i!=len; i++) {
        sprintf(&hex_string[i*2], "%02x", arr[i]);
    } 
    hex_string[hex_len] = '\0';


    return hex_string;
}

char* get_passwd() {
    char* input = malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
    int i = 0;
    char ch;

    while ((ch = getch()) != '\r') { // '\r' is Enter key
        if (ch == '\b' && i > 0) {   // Handle backspace
            printf("\b \b");
            i--;
        } else if (ch != '\b') {
            input[i++] = ch;
            printf("*"); // Display asterisks for feedback
        }
    }
    input[i] = '\0';

    return input;
}

int countLines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1; 
    }

    int lines = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }
    fclose(file);
    return lines;
}

void create_account() {
    char no_spaces = ' ';

    FILE* users_ptr;
    users_ptr = fopen(users_file, "a");

    printf("Please enter what you would like your username and password to be.\n");
    
    new_user:
        printf("Username: ");
        char* username = malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
        scanf("%99s", username);

        if (strchr(username, no_spaces)) { //checks if the username has a space, and them tells them they can't
            printf("Username cannot contain spaces.\n");
            goto new_user; // I know I shouldn't use goto, but it seemed like the best solution.
        }
    
    new_pass:
        printf("Password: ");
        
        char* passwd = get_passwd();

        printf("\nConfirm password: ");
        char* check_pass = get_passwd();

        printf("\n");

        if (!(strcmp(passwd,check_pass)==0)) {
            printf("Passwords do not match, please try again\n");
            goto new_pass;
        }

    
    int* strin = encrypt(passwd);
    char* crypted = malloc(32 * sizeof(char) + 1);
    crypted = int_array_to_hex_string(strin, 16);
    
    int lineCount = countLines(users_file);
    if (lineCount == -1) {
        printf("Trouble reading file...");
        exit(1);
    }
    sprintf(username + strlen(username), " %s %d\n", crypted, lineCount);

    size_t length = strlen(username) / sizeof(char);

    fwrite(username, sizeof(char), length, users_ptr);   
    fclose(users_ptr);
    free(username);
    free(passwd);
    free(check_pass);
    free(strin);
    free(crypted);   

}

// Takes in the user and passwd char to return -1 if that user doesn't exist and the user id if it does.
int check_cred(const char* user, const char* passwd){
    int user_id = -1;
    FILE* users_ptr = fopen(users_file, "r");
    if (users_ptr==NULL) {
        printf("Error reading file data");
        exit(1);
    }

    char* line = malloc(DEFAULT_BUFFER_SIZE * sizeof(char));
    while (fgets(line,DEFAULT_BUFFER_SIZE,users_ptr)) {
        char *f_user = strtok(line, " ");
        if (strcmp(f_user,user)==0) {
            char *pass_hash = strtok(NULL, " ");

            int* strin = encrypt(passwd);
            char* crypted = malloc(32 * sizeof(char) + 1);
            crypted = int_array_to_hex_string(strin, 16);   

            if (strcmp(pass_hash,crypted) == 0) {
                free(line);
                return atoi(strtok(NULL, " "));
            }
        }


    }

    free(line);
    
    
    return -1;
    
}

int confirm(char* msg, char _default) {
    _default = tolower(_default);

    printf("%s ", msg);
    if (_default == 'y') {
        printf("(Y/n): ");
    } else {
        printf("(y/N): ");
    }
    
    char choice = getchar();
    
    printf("",choice);

    switch (towlower(choice)) {
        case 'y':
            return 1;
            
        case 'n':
            return 0;
        
        default:
            if (_default=='y' && choice=='\n') {
                return 1;
            } else {
                return  0;
            }
    }
}

// Function understand the command and do a function
void handle_cmd(char* input, char* username, int logged_in, char* output) {
    char* cmd = malloc(DEFAULT_BUFFER_SIZE * sizeof(char));
    strcpy(cmd,input);

    char* delim_pos = strchr(input, ' ');

    strtok(cmd, " ");
    
    for(int i = 0; cmd[i]; i++){
        cmd[i] = tolower(cmd[i]);
    }

    if (strcmp(cmd,"hello")==0) {
        strcpy(output, "Hello to you too good sir\n");
    }

    if (strcmp(cmd,"mkuser")==0) {
        strcpy(output, "Running create_account...\n");
        
        create_account();
    }

    if (strcmp(cmd,"ahash")==0) {
        
    
        if (delim_pos) {
            char* second_half = delim_pos + 1;
            int* strin = encrypt(second_half);
            char* crypted = malloc(32 * sizeof(char) + 1);
            crypted = int_array_to_hex_string(strin, 16);
            crypted[32] = '\n';
            crypted[33] = '\0';

            strcpy(output, crypted);
            
        } else {
            strcpy(output, "Nothing found to hash\n");
        }

        
    } 

    if (strcmp(cmd,"echo")==0) {
        
    
        if (delim_pos) {
            char* second_half = delim_pos + 1;


            strcpy(output, second_half);
            output[strlen(second_half)] = '\n';    
            output[strlen(second_half)+1] = '\0';

        } else {
            strcpy(output, "\n");
        }
    }
    
    if (strcmp(cmd,"whoami")==0) {
        strcpy(cmd,username);
        strcat(output," with an id of ");

        output[strlen(username)] = '\n';
        output[strlen(username)+1] = '\0';
        
    }

    if (strcmp(cmd,"exit")==0) {
        free(cmd);
        exit(0);
    }

    free(cmd);

}

int main() {
    printf("Welcomem to Amon's login service.\n");
    FILE* users_ptr;
    users_ptr = fopen(users_file, "r");

    int logged_in = -1;
    if (users_ptr==NULL) {
        printf("No userdata found, creating new file...\n");

        FILE* fptr;
        fptr = fopen(users_file, "w"); // Create a file for writing
        if (fptr == NULL) {
            printf("Error! opening file");
            exit(1);
        } else {
            fclose(fptr);
            create_account();
        }
        
    } else {
        fclose(users_ptr);
    }
    


    printf("\nPlease enter username and password\n");

    char* username = malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
    char* passwd;

    while (logged_in==-1) {
        printf("Username: ");
        
        fgets(username, DEFAULT_BUFFER_SIZE, stdin);
        username[strlen(username) - 1] = '\0';

        printf("Password: ");
        passwd = get_passwd();
        printf("\n");

        logged_in = check_cred(username,passwd);
        if (logged_in==-1) printf("Wrong username or password, please try again\n");
    }
    

    clear();
    printf("Logged in as %s\n", username);

    //free(username);
    free(passwd);

    

    char* input = malloc(DEFAULT_BUFFER_SIZE * sizeof(char));
    char* output = malloc(DEFAULT_BUFFER_SIZE * sizeof(char));
    strcpy(output, "");
    // Command loop
    while (1) {
        printf("%s", prompt);
        if (fgets(input, DEFAULT_BUFFER_SIZE, stdin)) {
            // Remove trailing newline if present
            input[strcspn(input, "\n")] = '\0';
            
            handle_cmd(input, username, logged_in, output);
            printf("%s", output);
            strcpy(output, "");
        }
    }

    free(input);
    free(output);
    return(0);
}