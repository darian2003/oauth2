#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define TOKEN_LEN 15

/**
 * generate alpha-numeric string based on random char*
 * 
 * INPUT: fixed length of 16
 * OUTPUT: rotated string
 * */
char* generate_access_token(char* clientIdToken) {
    char *token = malloc(TOKEN_LEN * sizeof(char*));
    int i, key, used[TOKEN_LEN];
    int rotationIndex = TOKEN_LEN;

    memset(used, 0, TOKEN_LEN * sizeof(int));
    for (i = 0; i < TOKEN_LEN; i++) {
        do {
            key = rand() % rotationIndex;
        } while (used[key] == 1);
        token[i] = clientIdToken[key];
        used[key] = 1;
    }
    token[TOKEN_LEN] = '\0';
    return token;
}


#define SHIFT 5       // Caesar cipher shift value

char* generate_signature_token(const char* clientIdToken) {
    char *token = malloc(TOKEN_LEN + 1);  
    int i;

    for (i = 0; i < TOKEN_LEN; i++) {
        char c = clientIdToken[i];
        
        if (isalpha(c)) {  // If the character is a letter
            if (isupper(c)) {
                token[i] = 'A' + ((c - 'A' + SHIFT) % 26);  // Shift and wrap for uppercase
            } else {
                token[i] = 'a' + ((c - 'a' + SHIFT) % 26);  // Shift and wrap for lowercase
            }
        } else if (isdigit(c)) {  // If the character is a digit
            token[i] = '0' + ((c - '0' + SHIFT) % 10);  // Shift and wrap for numbers
        } else {
            token[i] = c;  // If it's neither, leave it unchanged (shouldn't happen per your constraints)
        }
    }
    token[TOKEN_LEN] = '\0';  // Null-terminate the string
    return token;
}

