/*
    COMP9315 20T1 Assignment 2
    Group Name: Tonight Fight Tiger
    Students:
        Raymond Lu z5277884
        Haowei Huang z5247672
*/

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "tsig.h"

Status pageIsNew(Page p)
{
	return (pageNitems(p) == 1);
}

// /* When we make the page signature, we need to check the last data page is a new page or not (
//  * i.e. a new page is a page with just one tuple).
//  * */
Bits makePageSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	Bits querySig = newBits(psigBits(r));
	char **attributes = tupleVals(r, t);
	for (Count i = 0; i < nAttrs(r); i++)
		orBits(querySig, codeWord(attributes[i], psigBits(r), codeBits(r)));
	free(attributes);
	return (querySig);
}

// find "matching" pages using page signatures
void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	Bits querySig = makePageSig(q->rel, q->qstring);
	unsetAllBits(q->pages);
	// psig_pid stands for the page id in the page signature file
	for (PageID psig_pid = 0; psig_pid < nPsigPages(q->rel); psig_pid++)
	{
		q->nsigpages++; // one more signature page read
		Page current_psig_page = getPage(psigFile(q->rel), psig_pid);
		// tid stands for tuple id in the current page-signature page
		for (Offset tid = 0; tid < pageNitems(current_psig_page); tid++)
		{
			q->nsigs++; // one more signature read
			Bits pageSig = newBits(psigBits(q->rel));
			getBits(current_psig_page, tid, pageSig);
			if (isSubset(querySig, pageSig))
				setBit(q->pages, psig_pid * maxPsigsPP(q->rel) + tid);
			freeBits(pageSig);
		}
		free(current_psig_page);
	}
	freeBits(querySig);
}
