#include "AM.h"
#include "structs.h"
#include <string.h>
#include "bf.h"
struct scan *Scans[20];
struct openedFiles *OpenFiles[20];

int AM_errno = AME_OK;

void AM_Init() {
	BF_Init(LRU);
	return;
}


int AM_CreateIndex(char *fileName, 
		char attrType1, 
		int attrLength1, 
		char attrType2, 
		int attrLength2) {
	if(BF_CreateFile(fileName) != BF_OK) return -2;// HP_ERROR;
	int fd;
	BF_OpenFile(fileName,&fd);
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
	//	BF_Block_Destroy(&block);
	BF_CloseFile(fd);
	return AME_OK;
}


int AM_DestroyIndex(char *fileName) {
	if(1){	//if no opened
		remove(fileName);
	}
	return AME_OK;
}


int AM_OpenIndex (char *fileName) {
	int *fileDesc = malloc(sizeof(int));
	if(BF_OpenFile(fileName,fileDesc) != BF_OK) return -2;//HP_ERROR;
	BF_Block *block;
	BF_Block_Init(&block);
	BF_GetBlock(*fileDesc,0,block);
	char *data = BF_Block_GetData(block);
	char str[strlen("B+Tree")+1];
	if(data != NULL) memcpy(str,data,strlen("B+Tree")+1);
	if(strcmp(str,"B+Tree")) return -2;
	struct openedFiles *opFiles = malloc(sizeof(struct openedFiles));
	opFiles->fileName = fileName;

	int offset = strlen("B+Tree")+1;
	int Length1,Length2;
	char Type1,Type2;
	memcpy(&Type1,&(data[offset]),sizeof(char));
	offset += sizeof(char);

	memcpy(&Length1,&(data[offset]),sizeof(int));
	offset += sizeof(int);

	memcpy(&Type2,&(data[offset]),sizeof(char));
	offset += sizeof(char);

	memcpy(&Length2,&(data[offset]),sizeof(int));
	
	for(int i=0;i<20;i++){
		if (OpenFiles[i] == NULL) OpenFiles[i] = opFiles;
	}
	BF_UnpinBlock(block);
	BF_Block_Destroy(&block);
	return AME_OK;
}


int AM_CloseIndex (int fileDesc) {
	if(1){  // if no scans running
	free(OpenFiles[fileDesc]);
	OpenFiles[fileDesc] = NULL;
	return AME_OK;
	}
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
	free(Scans[scanDesc]);
	Scans[scanDesc]=NULL;
	return AME_OK;
}


void AM_PrintError(char *errString) {
	printf("%s\n",errString);	/* Hash table for errors must be implemented */
	char str[20] = hash(AM_errno);
	printf("%s\n",str);


}

void AM_Close() {

}
