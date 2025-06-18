#include <stdlib.h>
#include <string.h>

int* encrypt(const char* str) {
    int* enc_str = (int*)calloc(16, sizeof(int));

    int length = strlen(str);
    int loop = 0;
    int chr = 0;

    if (length >=16) { //Repeat letters on top
        for (int i = 0; i < length; i++) {
            if (loop >= 16) {
                loop = 0;
                chr++;
            }
            
            enc_str[loop] = enc_str[loop] + str[i] + chr;
            loop++;
        } 
    } else {
        for (int i = 0; i < 16; i++) {

            if (loop >= length) {
                loop = 0;
                chr++;
            }
            enc_str[i] = str[loop] + chr;
            loop++;
        } 


    }
    

    for (int i = 0; i < 16; i++) {
        enc_str[i] = enc_str[i] & 117;
    }

    for (int i = 0; i < 16; i++) {
        for (int x = 0; x < 16; x++) {
            enc_str[i] = enc_str[x] ^ enc_str[i];
            //enc_str[x] = (enc_str[x]/10) * (enc_str[x]/10);
        }
    }
    
    for (int i = 0; i < 16; i++) {
        for (int x = 0; x < 16; x++) {
            enc_str[i] = enc_str[x] + enc_str[i];
            //enc_str[x] = (enc_str[x]/10) * (enc_str[x]/10);
        }
    }

    for (int x = 0; x < 16; x++) {
        enc_str[x] = (enc_str[x] + 2) * (enc_str[x] + 2);
        if (enc_str[x] < 0) enc_str[x] = enc_str[x] * -1;
        while (enc_str[x] > 255) {
            enc_str[x] = enc_str[x]>>2;
        }

    }
    
    return enc_str;
}