//
// Created by Minrui Lu on 5/4/20.
//

#include <assert.h>
#include <stdbool.h>
#include "defs.h"
#include "bits.h"
#include "page.h"

typedef struct _BitsRep {
    Count nbits;          // how many bits
    Count nbytes;          // how many bytes in array
    Byte bitstring[1];  // array of bytes to hold bits
    // actual array size is nbytes
} BitsRep;

Bits newBits(int nbits) {
    Count nbytes = iceil(nbits, 8);
    Bits new = malloc(2 * sizeof(Count) + nbytes);
    new->nbits = nbits;
    new->nbytes = nbytes;
    memset(&(new->bitstring[0]), 0, nbytes);
    return new;
}



int main(){
    Byte bitstring[9] = "00110000";
    int position = 3;
    Byte chr = bitstring[position/8];
    printf("%s\n",chr);
    printf("%s",chr>>(position%8));
    return 0;


}