/*
    COMP9315 20T1 Assignment 2
    Group Name: Tonight Fight Tiger
    Students:
        Raymond Lu z5277884
        Haowei Huang z5247672
*/

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tuple.h"
#include "tsig.h"
#include "psig.h"
#include "bsig.h"
#include "bits.h"
#include "hash.h"
// open a file with a specified suffix
// - always open for both reading and writing

File openFile(char *name, char *suffix)
{
	char fname[MAXFILENAME];
	sprintf(fname, "%s.%s", name, suffix);
	File f = open(fname, O_RDWR | O_CREAT, 0644);
	assert(f >= 0);
	return f;
}

// create a new relation (five files)
// data file has one empty data page

Status newRelation(char *name, Count nattrs, float pF,
				   Count tk, Count tm, Count pm, Count bm)
{
	Reln r = malloc(sizeof(RelnRep));
	RelnParams *p = &(r->params);
	assert(r != NULL);
	p->nattrs = nattrs;
	p->pF = pF,
	p->tupsize = 28 + 7 * (nattrs - 2);
	Count available = (PAGESIZE - sizeof(Count));
	p->tupPP = available / p->tupsize;
	p->tk = tk;
	if (tm % 8 > 0)
		tm += 8 - (tm % 8); // round up to byte size
	p->tm = tm;
	p->tsigSize = tm / 8;
	p->tsigPP = available / (tm / 8);
	if (pm % 8 > 0)
		pm += 8 - (pm % 8); // round up to byte size
	p->pm = pm;
	p->psigSize = pm / 8;
	p->psigPP = available / (pm / 8);
	if (p->psigPP < 2)
	{
		free(r);
		return -1;
	}
	if (bm % 8 > 0)
		bm += 8 - (bm % 8); // round up to byte size
	p->bm = bm;
	p->bsigSize = bm / 8;
	p->bsigPP = available / (bm / 8);
	if (p->bsigPP < 2)
	{
		free(r);
		return -1;
	}
	r->infof = openFile(name, "info");
	r->dataf = openFile(name, "data");
	r->tsigf = openFile(name, "tsig");
	r->psigf = openFile(name, "psig");
	r->bsigf = openFile(name, "bsig");
	addPage(r->dataf);
	p->npages = 1;
	p->ntups = 0;
	addPage(r->tsigf);
	p->tsigNpages = 1;
	p->ntsigs = 0;
	addPage(r->psigf);
	p->psigNpages = 1;
	p->npsigs = 0;

	p->bsigNpages = 0;
	p->nbsigs = 0;
	// iceil(psigBits(r),maxBsigsPP(r)) is how many bSig pages we need to create
	for (PageID bsig_pid = 0; bsig_pid < iceil(psigBits(r), maxBsigsPP(r)); bsig_pid++)
	{
		Page page = newPage();
		// fill each page with all-zeros bit-strings
		for (Offset j = 0; j < maxBsigsPP((r)); j++)
		{
			Bits bSig = newBits(bsigBits(r));
			putBits(page, j, bSig);
			p->nbsigs++; // one more bit-sliced signature added
			addOneItem(page);
			freeBits(bSig);
			// When there are "pm" bit-strings already, we should stop creating new bit-strings
			if (p->nbsigs == psigBits(r))
				break;
		}
		putPage(r->bsigf, bsig_pid, page);
		p->bsigNpages++; // one more bit-slice signature page added
	}
	closeRelation(r);
	return 0;
}

// check whether a relation already exists

Bool existsRelation(char *name)
{
	char fname[MAXFILENAME];
	sprintf(fname, "%s.info", name);
	File f = open(fname, O_RDONLY);
	if (f < 0)
		return FALSE;
	else
	{
		close(f);
		return TRUE;
	}
}

// set up a relation descriptor from relation name
// open files, reads information from rel.info

Reln openRelation(char *name)
{
	Reln r = malloc(sizeof(RelnRep));
	assert(r != NULL);
	r->infof = openFile(name, "info");
	r->dataf = openFile(name, "data");
	r->tsigf = openFile(name, "tsig");
	r->psigf = openFile(name, "psig");
	r->bsigf = openFile(name, "bsig");
	read(r->infof, &(r->params), sizeof(RelnParams));
	return r;
}

// release files and descriptor for an open relation
// copy latest information to .info file
// note: we don't write ChoiceVector since it doesn't change

void closeRelation(Reln r)
{
	// make sure updated global data is put in info file
	lseek(r->infof, 0, SEEK_SET);
	int n = write(r->infof, &(r->params), sizeof(RelnParams));
	assert(n == sizeof(RelnParams));
	close(r->infof);
	close(r->dataf);
	close(r->tsigf);
	close(r->psigf);
	close(r->bsigf);
	free(r);
}

// insert a new tuple into a relation
// returns page where inserted
// returns NO_PAGE if insert fails completely

PageID addToRelation(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL && strlen(t) == tupSize(r));
	Page p;
	PageID pid;
	RelnParams *rp = &(r->params);

	// add tuple to last page
	pid = rp->npages - 1;
	p = getPage(r->dataf, pid);
	// check if room on last page; if not add new page
	if (pageNitems(p) == rp->tupPP)
	{
		addPage(r->dataf);
		rp->npages++;
		pid++;
		free(p);
		p = newPage();
		if (p == NULL)
			return NO_PAGE;
	}
	addTupleToPage(r, p, t);
	rp->ntups++; //written to disk in closeRelation()
	putPage(r->dataf, pid, p);

	// compute tuple signature and add to tsigf
	PageID tuple_pid = rp->tsigNpages - 1; // current tuple page id in tuple signature file
	Page tuple_page = getPage(tsigFile(r), tuple_pid);
	// check if room on last tuple signature page; if not add new page
	if (pageNitems(tuple_page) == maxTsigsPP(r))
	{
		addPage(tsigFile(r));
		tuple_pid++;
		free(tuple_page);
		tuple_page = newPage();
		if (tuple_page == NULL)
			return NO_PAGE;
		rp->tsigNpages++; // one more tuple signature page added
	}
	Bits tupleSig = makeTupleSig(r, t);
	putBits(tuple_page, pageNitems(tuple_page), tupleSig);
	addOneItem(tuple_page);
	rp->ntsigs++; // one more tuple signature added
	putPage(tsigFile(r), tuple_pid, tuple_page);
	freeBits(tupleSig);

	// compute page signature and add to psigf
	PageID page_pid = rp->psigNpages - 1;
	Page page_page = getPage(psigFile(r), page_pid);
	Page last_data_page = getPage(dataFile(r), pid);
	Bits pageSig = makePageSig(r, t);
	// A new page signature is added only when a new data page is added
	if (pageIsNew(last_data_page))
	{
		// check if room on last tuple signature page; if not add new page
		if (pageNitems(page_page) == maxPsigsPP(r))
		{
			addPage(psigFile(r));
			page_pid++;
			free(page_page);
			page_page = newPage();
			if (page_page == NULL)
				return NO_PAGE;
			rp->psigNpages++;
		}
		putBits(page_page, pid % maxPsigsPP(r), pageSig);
		addOneItem(page_page);
		rp->npsigs++;
	}
	else
		putBits(page_page, pid % maxPsigsPP(r), pageSig);
	putPage(psigFile(r), page_pid, page_page);
	free(last_data_page);
	
	// compute bit-sliced signature and add to bsigf
	for (Offset index = 0; index < psigBits(r); index++)
	{
		// since putPage() frees the input page, we need to read the page in each loop
		if (bitIsSet(pageSig, index))
		{
			PageID bsig_pid = index / maxBsigsPP(r);
			Page bsig_page = getPage(bsigFile(r), bsig_pid);
			Bits slice = newBits(bsigBits(r));
			getBits(bsig_page, index % maxBsigsPP(r), slice);
			setBit(slice, pid);
			putBits(bsig_page, index % maxBsigsPP(r), slice);
			putPage(bsigFile(r), bsig_pid, bsig_page);
		}
	}
	freeBits(pageSig);
	return nPages(r) - 1;
}

// displays info about open Reln (for debugging)

void relationStats(Reln r)
{
	RelnParams *p = &(r->params);
	printf("Global Info:\n");
	printf("Dynamic:\n");
	printf("  #items:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
		   p->ntups, p->ntsigs, p->npsigs, p->nbsigs);
	printf("  #pages:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
		   p->npages, p->tsigNpages, p->psigNpages, p->bsigNpages);
	printf("Static:\n");
	printf("  tups   #attrs: %d  size: %d bytes  max/page: %d\n",
		   p->nattrs, p->tupsize, p->tupPP);
	printf("  sigs   bits/attr: %d\n", p->tk);
	printf("  tsigs  size: %d bits (%d bytes)  max/page: %d\n",
		   p->tm, p->tsigSize, p->tsigPP);
	printf("  psigs  size: %d bits (%d bytes)  max/page: %d\n",
		   p->pm, p->psigSize, p->psigPP);
	printf("  bsigs  size: %d bits (%d bytes)  max/page: %d\n",
		   p->bm, p->bsigSize, p->bsigPP);
}
