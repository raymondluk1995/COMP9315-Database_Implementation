// Test Bits ADT

#include <stdio.h>
#include "defs.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"

int main(int argc, char **argv)
{
	Bits a = newBits(60);
	Bits b = newBits(60);
	setBit(a, 3);
	setBit(a, 10);
	setBit(a, 40);
	showBits(a);printf("\n");
	andBits(a, b);


////	setBit(b, 50);
//	setBit(a, 3);
////	setBit(a, 40);
	showBits(a);printf("\n");
	showBits(b);printf("\n");
//	printf("if a is b subset:%s\n", isSubset(a, b)?"true":"false");
	printf("set posittion %d th bit to 1\n", 3);    // testing
	printf("t=0: "); showBits(b); printf("\n");
	printf("set posittion %d th bit to 1\n", 5);    // testing
	setBit(b, 5);
	printf("t=1: "); showBits(b); printf("\n");
	printf("set posittion %d th bit to 1\n", 0);    // testing
	setBit(b, 0);
	printf("set posittion %d th bit to 1\n", 50);    // testing
	setBit(b, 50);
	printf("set posittion %d th bit to 1\n", 59);    // testing
	setBit(b, 59);
	printf("t=2: "); showBits(b); printf("\n");
	if (bitIsSet(b,5)) printf("Bit 5 is set\n");
	if (bitIsSet(b,10)) printf("Bit 10 is set\n");
	setAllBits(b);
	printf("set all bits to 1\n");
	printf("t=3: "); showBits(b); printf("\n");
	printf("set %d th bits to 0\n", 40);
	unsetBit(b, 40);
	printf("t=4: "); showBits(b); printf("\n");
	unsetBit(b, 50);
	unsetBit(b, 20);
	if (bitIsSet(b,20)) printf("Bit 20 is set\n");
	if (bitIsSet(b,40)) printf("Bit 40 is set\n");
	if (bitIsSet(b,50)) printf("Bit 50 is set\n");
	printf("t=5: "); showBits(b); printf("\n");
	return 0;
}
