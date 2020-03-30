// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"

Bits makePageSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	//TODO
	return NULL; // remove this
}

void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	//TODO
	setAllBits(q->pages); // remove this
}

