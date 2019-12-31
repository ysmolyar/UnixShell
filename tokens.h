
#ifndef TOKENS_H
#define TOKENS_H

char* read_line();

bool is_operator(char c);

bool is_and_or_operator(int ii, char* buf);

char* read_token(int ii, char* line);

svec* tokenize(char* line);

#endif
