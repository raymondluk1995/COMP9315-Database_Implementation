// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"

void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);
	//TODO
	setAllBits(q->pages); // remove this
}

