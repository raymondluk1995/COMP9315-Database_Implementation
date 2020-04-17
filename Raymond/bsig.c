// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"
#include "tsig.h"
#include "psig.h"

void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);
	//TODO
	Bits querySig = makePageSig(q->rel,q->qstring);
    setAllBits(q->pages);
    PageID bsig_pid = -1;
    Page bsig_page;
    for (Offset index=0;index<psigBits(q->rel);index++){
        if(bitIsSet(querySig,index)){
            if(bsig_pid!=index/maxBsigsPP(q->rel)){ // A new bit-sliced signature page should be read
                bsig_pid = index/maxBsigsPP(q->rel);
                bsig_page = getPage(bsigFile(q->rel),bsig_pid);
                q->nsigpages++;
            }
            Bits slice = newBits(bsigBits(q->rel));
            getBits(bsig_page,index%maxBsigsPP(q->rel),slice);
            q->nsigs++;
            andBits(q->pages,slice);
            freeBits(slice);
        }
    }
    freeBits(querySig);
    free(bsig_page);
}

