#include "stdio.h"
#include "stdlib.h"

struct scan {
	int fileDesc;
	int blockNum;
	int recordNum;
};

struct openedFiles {
	char *fileName;
	int attrLength1;
	char attrType1;
	char attrType2;
	int attrLength2;
};
