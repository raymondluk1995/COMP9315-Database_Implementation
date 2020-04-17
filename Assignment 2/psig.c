// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "tsig.h"

Status pageIsNew(Page p){
    return (pageNitems(p)==1);
}

Bits makePageQuerySig(Reln r, Tuple t){
	assert(r!=NULL && t!= NULL);
	Bits querySig = newBits(psigBits(r));
	char ** attributes = tupleVals(r,t);
	for (Count i =0;i<nAttrs(r);i++){
        orBits(querySig,codeWord(attributes[i],psigBits(r),codeBits(r)));
    }
	free(attributes);
	return (querySig);
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
    // if a data page is not new, that means this data page should have already got
    // a Page Signature
    if(!pageIsNew((last_data_page)))
        getBits(last_sig_page,pageNitems(last_sig_page)-1,pageSig);
    char ** attributes = tupleVals(r,t);
    for (Count i =0;i<nAttrs(r);i++){
        orBits(pageSig,codeWord(attributes[i],psigBits(r),codeBits(r)));
    }
    free(attributes);
    free(last_data_page);
    free(last_sig_page);
    return (pageSig);
}

void findPagesUsingPageSigs(Query q)
{
  assert(q != NULL);
	//TODO
  Bits querySig = makePageQuerySig(q->rel,q->qstring);
	unsetAllBits(q->pages);
	// psig_pid stands for the page id in the page signature file
	for (PageID psig_pid=0;psig_pid<nPsigPages(q->rel);psig_pid++){
	    q->nsigpages++;
	    Page current_psig_page = getPage(psigFile(q->rel),psig_pid);
        // tid stands for tuple id in the current page-signature page
	    for (Offset tid=0;tid<pageNitems(current_psig_page);tid++){
	        q->nsigs++;
	        Bits pageSig = newBits(psigBits(q->rel));
	        getBits(current_psig_page,tid,pageSig);
	        if(isSubset(querySig,pageSig)){
	            setBit(q->pages,psig_pid*maxPsigsPP(q->rel)+tid);
	        }
	        freeBits(pageSig);
	    }
	    free(current_psig_page);
	}
	freeBits(querySig);

}
