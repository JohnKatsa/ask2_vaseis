#include "stdio.h"
#include "stdlib.h"

struct scan {
	int fileDesc;
	int blockNum;
	int offset;
	int op;
	void *value;
};

typedef struct scan Scan;

struct openedFiles {
	char *fileName;
	int attrLength1;
	char attrType1;
	char attrType2;
	int attrLength2;
};
