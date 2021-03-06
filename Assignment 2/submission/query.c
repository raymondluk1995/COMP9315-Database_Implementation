/*
    COMP9315 20T1 Assignment 2
    Group Name: Tonight Fight Tiger
    Students:
        Raymond Lu z5277884
        Haowei Huang z5247672
*/

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"
#include "tsig.h"
#include "psig.h"
#include "bsig.h"

// check whether a query is valid for a relation
// e.g. same number of attributes

int checkQuery(Reln r, char *q)
{
	if (*q == '\0')
		return 0;
	char *c;
	int nattr = 1;
	for (c = q; *c != '\0'; c++)
		if (*c == ',')
			nattr++;
	return (nattr == nAttrs(r));
}

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q, char sigs)
{
	Query new = malloc(sizeof(QueryRep));
	assert(new != NULL);
	if (!checkQuery(r, q))
		return NULL;
	new->rel = r;
	new->qstring = q;
	new->nsigs = new->nsigpages = 0;
	new->ntuples = new->ntuppages = new->nfalse = 0;
	new->pages = newBits(nPages(r));
	switch (sigs)
	{
	case 't':
		findPagesUsingTupSigs(new);
		break;
	case 'p':
		findPagesUsingPageSigs(new);
		break;
	case 'b':
		findPagesUsingBitSlices(new);
		break;
	default:
		setAllBits(new->pages);
		break;
	}
	new->curpage = 0;
	return new;
}

// scan through selected pages (q->pages)
// search for matching tuples and show each
// accumulate query stats

void scanAndDisplayMatchingTuples(Query q)
{
	assert(q != NULL);
	for (q->curpage = 0; q->curpage < nPages(q->rel); q->curpage++)
	{
		if (bitIsSet(q->pages, q->curpage))
		{
			Page p = getPage(dataFile(q->rel), q->curpage); // get the current data page
			q->ntuppages++;									// one more data page read
			Status found = FALSE;
			for (q->curtup = 0; q->curtup < pageNitems((p)); q->curtup++)
			{ // iterate through all items in current data page
				Tuple s_tuple = getTupleFromPage(q->rel, p, q->curtup); // s_tuple stands for search tuple
				if(strlen(s_tuple)==0)
					break;
				q->ntuples++; //  one more data tuple read
				if (tupleMatch(q->rel, q->qstring, s_tuple))
				{
					showTuple(q->rel, s_tuple);
					found = TRUE;
				}
			}
			if (found == FALSE) // no tuples in current page are matched
				q->nfalse++;
			free(p);
		}
	}
}

// print statistics on query

void queryStats(Query q)
{
	printf("# sig pages read:    %d\n", q->nsigpages);
	printf("# signatures read:   %d\n", q->nsigs);
	printf("# data pages read:   %d\n", q->ntuppages);
	printf("# tuples examined:   %d\n", q->ntuples);
	printf("# false match pages: %d\n", q->nfalse);
}

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	free(q->pages);
	free(q);
}
