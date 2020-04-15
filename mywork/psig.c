// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "tsig.h"

Bool pageIsNew(Page p){
    return (pageNitems(p)==1);
}

/* When we make the page signature, we need to check the last data page is a new page or not (
 * i.e. a new page is a page with just one tuple).
 * */

Bits makePageSig(Reln r, Tuple t)
{
	//?
    assert(r != NULL && t != NULL);
	//TODO
    Page last_data_page = getPage(dataFile(r),nPages(r)-1);
    Page last_sig_page = getPage(psigFile(r),nPsigPages(r)-1);
    Bits pageSig = newBits(psigBits(r));
    if(!pageIsNew((last_data_page)))
        getBits(last_sig_page,pageNitems(last_sig_page),pageSig);

    // Since Tuple is a pointer, we need to make a copy string for strtok() later
    char * temp = malloc(sizeof(char)*tupSize(r));
    strcpy(temp,t);
    char * token = strtok(temp,",");

    while(token!=NULL){
        orBits(pageSig,codeWord(token,psigBits(r),codeBits(r)));
        token = strtok(NULL,",");
    }
    free(temp);
    return (pageSig);
}

void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	//TODO
	setAllBits(q->pages); // remove this
}

