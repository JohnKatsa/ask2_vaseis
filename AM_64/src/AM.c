#include "AM.h"
#include "structs.h"
#include <string.h>

struct scan Scans[20];
struct openedFiles OpenFiles[20];

int AM_errno = AME_OK;

void AM_Init() {
	return;
}


int AM_CreateIndex(char *fileName, 
		char attrType1, 
		int attrLength1, 
		char attrType2, 
		int attrLength2) {
	if(BF_CreateFile(filename) != BF_OK) return HP_ERROR;
	int fd;
	BF_OpenFile(filename,&fd);
	BF_Block *block;
	BF_Block_Init(&block);
	BF_AllocateBlock(fd,block);
	char *data = BF_Block_GetData(block);
	/* Identify a B+ Tree  */
	char str[] = "B+Tree";
	memcpy(data,str,strlen(str)+1);
	/* ********************** */

	int offset = strlen(str)+1;
	memcpy(&(data[offset]),&attrType1,sizeof(char));
	offset += sizeof(char);

	memcpy(&(data[offset]),&attrLength1,sizeof(int));
	offset += sizeof(int);

	memcpy(&(data[offset]),&attrType2,sizeof(char));
	offset += sizeof(char);

	memcpy(&(data[offset]),&attrLength2,sizeof(int));
	offset += sizeof(int);

	/* All four fields written on block 0 */

	BF_Block_SetDirty(block);
	BF_UnpinBlock(block);
	BF_Block_Destroy(&block);
	BF_CloseFile(fd);
	return AME_OK;
}


int AM_DestroyIndex(char *fileName) {
	return AME_OK;
}


int AM_OpenIndex (char *fileName) {
	return AME_OK;
}


int AM_CloseIndex (int fileDesc) {
	return AME_OK;
}


int AM_InsertEntry(int fileDesc, void *value1, void *value2) {
	return AME_OK;
}


int AM_OpenIndexScan(int fileDesc, int op, void *value) {
	return AME_OK;
}


void *AM_FindNextEntry(int scanDesc) {

}


int AM_CloseIndexScan(int scanDesc) {
	return AME_OK;
}


void AM_PrintError(char *errString) {

}

void AM_Close() {

}
