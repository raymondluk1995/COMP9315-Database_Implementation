/*
    COMP9315 20T1 Assignment 2
    Group Name: Tonight Fight Tiger
    Students:
        Raymond Lu z5277884
        Haowei Huang z5247672
*/

#ifndef TSIG_H
#define TSIG_H 1

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "bits.h"

Bits makeTupleSig(Reln, Tuple);
void findPagesUsingTupSigs(Query);
Bits codeWord(char *, int, int);

#endif
