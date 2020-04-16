// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"
#include "psig.h"

void findPagesUsingBitSlices(Query q) {
    assert(q != NULL);

    setAllBits(q->pages);
    // get the file for storing bigSing
    File bSigFile = bsigFile(q->rel);
    // make a bSig for the query, based on the corresponding pageSig
    Bits querybitSig = makePageSig(q->rel, q->qstring);

    // the amount of bitSig stored in file
    Count numOfbitSigPP = maxBsigsPP(q->rel);
    // width(size) of a bitSig, representing the amount of pages it contains
    Count bitSigWidth = bsigBits(q->rel);
    // width of page signature (#bits)
    Count pageSigWidth = psigBits(q->rel);
    PageID pid = -1;  // the page index of bitSig File
    PageID pos = -1;  // the current position in a bitSig
    Page cur_page;

    // to temporarily store the bitSig of from the bitSig file
    Bits bits = newBits(bitSigWidth);

    // for each position of the query descriptor
    for (Count pageIndex = 0; pageIndex < pageSigWidth; pageIndex++) {   // the number of bitSigs
        // if the bit is set, find the corresponding bit-slices
        if (bitIsSet(querybitSig, pageIndex)) {
            q->nsigs++;
            if (pid != pageIndex / numOfbitSigPP) {
                pid = pageIndex / numOfbitSigPP;
                q->nsigpages++;
                cur_page = getPage(bSigFile, pid);
            }
            pos = pageIndex % numOfbitSigPP;

            // get i'th bit slice from bsigFile
            getBits(cur_page, pos, bits);

            // zero bits in Pages which are zero in Slice
            for (Count offset = 0; offset<bitSigWidth; offset++){
                if(bitIsSet(bits, offset)){
                    unsetBit(q->pages,offset);
                }
            }
        }
    }
    // free the bit
    freeBits(bits);
}