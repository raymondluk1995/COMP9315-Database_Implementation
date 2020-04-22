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
#include "bits.h"
#include "bsig.h"
#include "tsig.h"
#include "psig.h"

// check whether the query is selecting all
Bool checkAllQuestionMark(char *qstring)
{
    char *c;
    for (c = qstring; *c != '\0'; c++)
        if (*c == ',')
            continue;
        else if (*c != '?')
            return FALSE;
    return TRUE;
}

// find "matching" pages using bit-sliced signatures
void findPagesUsingBitSlices(Query q)
{
    assert(q != NULL);
    // In bit-sliced signature, the query signature is the same as the page signature one,
    // but just considering it gets transposed
    Bits querySig = makePageSig(q->rel, q->qstring);
    setAllBits(q->pages);
    // In bit-sliced signature, we don't need to read a page in each for loop
    if (checkAllQuestionMark(q->qstring))
        return;
    PageID bsig_pid = -1;
    Page bsig_page;
    for (Offset index = 0; index < psigBits(q->rel); index++)
    {
        if (bitIsSet(querySig, index))
        {
            if (bsig_pid != index / maxBsigsPP(q->rel))
            { // A new bit-sliced signature page should be read
                bsig_pid = index / maxBsigsPP(q->rel);
                bsig_page = getPage(bsigFile(q->rel), bsig_pid);
                q->nsigpages++; // one more signature page read
            }
            Bits slice = newBits(bsigBits(q->rel));
            // index%maxBsigsPP(q->rel) is the offset in current bsig_page
            getBits(bsig_page, index % maxBsigsPP(q->rel), slice);
            q->nsigs++; // one more signature read
            //zero bits in Pages which are zero in Slice
            for (Count j = 0; j < nbits(q->pages); j++)
            {
                if (bitIsSet(q->pages, j))
                {
                    if (!bitIsSet(slice, j))
                        unsetBit(q->pages, j);
                }
            }
            freeBits(slice);
        }
    }
    freeBits(querySig);
    free(bsig_page);
}
