#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "svec.h"
#include "tokens.h"

/*
 ATTRIBUTION: svec.c and svec.h are taken from Nat Tuck's Lecture 9      notes
 */

//method to read a single line of input into a char buffer
char* read_line() {
    char* line = malloc(256 * sizeof(char));
    fgets(line, 256, stdin);
    return line;
    
}

//method to determine if a character is a supported 1-character operator
bool is_operator(char c) {
    return (c == '&' || c == '|' || c == '<' || c == '>' || c == ';');
}

//method to determine if the two characters starting at index ii
//in a character buffer are either '&&' or '||'
bool is_and_or_operator(int ii, char* buf) {
    return ((buf[ii] == '&' && buf[ii+1] == '&') || 
            (buf[ii] == '|' && buf[ii+1] == '|')); 
}

//method to read and return the next valid token from char buffer line,
//starting at index ii
char* read_token(int ii, char* line) {
    char* token = malloc(strlen(line) * sizeof(char));
    memset(token, 0, strlen(line));
    int tok_idx = 0;
    //first checking if this token is an operator
    if (is_operator(line[ii])) {
        if (is_and_or_operator(ii, line)) {
            token[1] = line[ii + 1];
        }
       token[0] = line[ii];
       return token;
    }
    //then reading through line and storing chars until token is over
    for (int jj = ii; jj < strlen(line); jj++) {
        if (isspace(line[jj]) || is_operator(line[jj])) {
            return token;
        }
        token[tok_idx] = line[jj];
        ++tok_idx;
    }
    return token;
}

//method to reverse an svec and return it, done by creating a new svec and copying values
svec* reverse_vec(svec* vec) {
    svec* rev = make_svec();
    
    for (int ii = vec->size - 1; ii >= 0; --ii) {
        svec_push_back(rev, vec->data[ii]);
    }
    return rev;
}

//method to turn a char buffer line of input into an svec of tokens
svec* tokenize(char* line) {
    svec* tokens = make_svec();
    int nn = strlen(line);
    //loop through line, pushing each new token into svec and updating counter
    for (int ii = 0; ii < nn; ++ii) {
        if (isspace(line[ii])) {
            continue;
        }
        if (line[ii] == ')' || line[ii] == '(') {
            continue;
        }
        char* tok = read_token(ii, line);
        svec_push_back(tokens, tok);
        ii = (ii + strlen(tok) - 1);
        free(tok);
        
    }
    return tokens;
}




