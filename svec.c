/* This file is lecture notes from CS 3650, Fall 2018 */
/* Author: Nat Tuck */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "svec.h"

svec*
make_svec()
{
    svec* sv = malloc(sizeof(svec));
    sv->size = 0;
    sv->cap  = 4;
    sv->data = malloc(4 * sizeof(char*));
    memset(sv->data, 0, 4 * sizeof(char*));
    return sv;
}

void
free_svec(svec* sv)
{
    for (int ii = 0; ii < sv->size; ++ii) {
        if (sv->data[ii] != 0) {
            free(sv->data[ii]);
        }
    }
    free(sv->data);
    free(sv);
}

char*
svec_get(svec* sv, int ii)
{
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}

void
svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    sv->data[ii] = strdup(item);

}

void svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;

    if (ii >= sv->cap) {
        sv->cap *= 2;
        sv->data = (char**) realloc(sv->data, sv->cap * sizeof(char*));
    }

    sv->size = ii + 1;
    svec_put(sv, ii, item);
}

//returns first index of given item in svec
//if not found, return -1
int
svec_first_index_of(svec* vec, char* item)
{
    for (int i = 0; i < vec->size; i++) {
        if (strcmp(vec->data[i], item) == 0) {
            return i;
        }
    }
    return -1;
}

bool 
svec_contains(svec* vec, char* item)
{
    for (int i = 0; i < vec->size; i++) {
        if (strcmp(vec->data[i], item) == 0) {
            return true;
        }
    }
    return false;
}

svec*
subvec(svec* vec, int start, int end) 
{
    svec* result = make_svec();
    for (int i = start; i < end; i++) {
        svec_push_back(result, vec->data[i]);
    }
    return result;
}

