/*
    COMP9315 20T1 Assignment 2
    Group Name: Tonight Fight Tiger
    Students:
        Raymond Lu z5277884
        Haowei Huang z5247672
*/

#ifndef BITS_H
#define BITS_H 1

typedef struct _BitsRep *Bits;

#include "defs.h"
#include "page.h"

Bits newBits(int);
void freeBits(Bits);
Bool bitIsSet(Bits, int);
Bool isSubset(Bits, Bits);
void setBit(Bits, int);
void setAllBits(Bits);
void unsetBit(Bits, int);
void unsetAllBits(Bits);
void andBits(Bits, Bits);
void orBits(Bits, Bits);
void getBits(Page, Offset, Bits);
void putBits(Page, Offset, Bits);
void showBits(Bits);
Count nbits(Bits);

#endif
