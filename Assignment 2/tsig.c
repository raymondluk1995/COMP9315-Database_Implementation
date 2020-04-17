

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"

// make a tuple signature

Bits codeWord(char * attribute, int m, int k){ // reference: week 7 lecture page 5
    // m: width of tuple signature
    // k: bits set per attribute
    int nbits =0;
    Bits cWord = newBits(m);
    if (strcmp(attribute,"?")==0) // wildcard attribute
        return (cWord);
    // set the seed to be the hash value of input attribute
    srandom(hash_any(attribute,strlen(attribute)));

    while (nbits<k){
        int i = random()%m;
        if(!bitIsSet(cWord,i)){
            setBit(cWord,i);
            nbits++;
        }
    }
    return (cWord);
}

Bits makeTupleSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	//TODO
	Bits tupleSig = newBits(tsigBits(r));
	char ** attributes = tupleVals(r,t);
    for (Count i=0;i<nAttrs(r);i++){
        orBits(tupleSig,codeWord(attributes[i],tsigBits(r),codeBits(r)));
    }
    free(attributes);
	return tupleSig;
}

// find "matching" pages using tuple signatures

void findPagesUsingTupSigs(Query q)
{
	assert(q != NULL);
	//TODO
	Bits querySig = makeTupleSig(q->rel,q->qstring);
    unsetAllBits(q->pages);
    // tsig_pid stands for the page id in the tuple-signature file
	for (PageID tsig_pid =0;tsig_pid<nTsigPages(q->rel);tsig_pid++){
	    Page tsig_page = getPage(tsigFile(q->rel),tsig_pid);
		q->nsigpages ++;
        // tid stands for tuple id in the current tuple-signature page
	    for (Offset tid=0;tid<pageNitems(tsig_page);tid++){
	        Bits tupleSig = newBits(tsigBits(q->rel));
	        getBits(tsig_page,tid,tupleSig);
			q->nsigs++;
	        // Check whether query signature is a subset of tuple signature
	        if(isSubset(querySig,tupleSig)){
				Offset tuple_position = tsig_pid*maxTsigsPP(q->rel)+tid;
			    setBit(q->pages,tuple_position/maxTupsPP(q->rel));
	        }
	        freeBits(tupleSig);
	    }
	    free(tsig_page);
	}
	freeBits(querySig);
}
