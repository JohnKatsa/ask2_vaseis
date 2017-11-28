#include "AM.h"
#include "structs.h"
#include <string.h>
#include "bf.h"
struct scan *Scans[20];
struct openedFiles *OpenFiles[20];

int AM_errno = AME_OK;

int sizeofr;	// size of record
int sizeofb;	// number of entries the block can keep

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

	sizeofr = attrLength1 + attrLength2;
	sizeofb = (BF_BLOCK_SIZE - sizeof(char) - 2*sizeof(int))/sizeofr;

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


int cmp(char t1, int l1, void* value1, void* temp)
{
	int n;
	if(t1 == 'c'){
		n = strncmp(value1,temp,l1);
		return n;
	}
	else{
		n = (*(int*)(value1) < *(int*)(temp));
		//n = 10;
		if(n == 1)
			return -1;
	}
	return n;
}

int AM_InsertEntry(int fileDesc, void *value1, void *value2) {

	int blocks_num, offset, temp;
	BF_Block *block;
	BF_Block_Init(&block);
	BF_GetBlock(fileDesc, 0, block);
	char *data = BF_Block_GetData(block);

	int l1, l2;				// length of 1 & 2
	char t1, t2;				// type of 1 & 2
	offset = strlen("B+Tree")+1;
	
	memcpy(&t1,&data,sizeof(char));	// ================  nomizw 8elei &data[offset] ki edw ==================
	offset += sizeof(char);

	memcpy(&l1,&data[offset],sizeof(int));
	offset += sizeof(int);

	memcpy(&t2,&data[offset],sizeof(char));
	offset += sizeof(char);

	memcpy(&l2,&data[offset],sizeof(int));	
	offset += sizeof(int);
	
	if(BF_GetBlockCounter(fileDesc,&blocks_num) == 1){	// ====== BF_GetBlockCounter epistrefei BF_OK an 8ymamai kala. prepei na baloyme to blocks_num gia sygkrish =======

		memcpy(&data[offset],&blocks_num,sizeof(int));  // pointer to root //	=========== what ?? ========== //

		int offset = 0, temp;
		BF_AllocateBlock(fileDesc,block);		// make a data block
		data = BF_Block_GetData(block);			//

		memset(&data[offset],'d',sizeof(char));		//
		offset += sizeof(char);				// making of data block

		temp = 1;					// one record
		memcpy(&data[offset],&temp,sizeof(int));	// directly take one for the new record
		offset += sizeof(int);

		temp = 0;					//
		memcpy(&data[offset],&temp,sizeof(int));	// 0 means no sibling
		offset += sizeof(int);

		memcpy(&data[offset],value1,l1);		// pass value 1
		offset += l1;
		memcpy(&data[offset],value2,l2);		// pass value 2

		return AME_OK;
	}
	else{	// exists root
		int temp;
		char tempc;

		memcpy(&temp,&data[offset],sizeof(int));	// see where is the root
		BF_GetBlock(fileDesc,temp,block);		// take the root block
		data = BF_Block_GetData(block);

		offset = 0;

		memcpy(&tempc,&data[offset],sizeof(char));		// (k)ey or (d)ata
		while( tempc == 'k' ){	// index node

			offset += sizeof(char) + sizeof(int) + sizeof(int);	// key 1	// =============== what?? =============== //
			memcpy(&temp,&data[offset],l1);				// key in block (loop invariant)
			while((&data[offset] - &data[0]) < BF_BLOCK_SIZE)       // while in block
				if(tempc != 'k' )
					break;
				if(cmp(t1,l1,value1,&temp) == -1){
					memcpy(&temp,&data[offset - sizeof(int)],sizeof(int));	// find where should go
					BF_GetBlock(fileDesc,temp,block);			// take the right block
					data = BF_Block_GetData(block);
					memcpy(&tempc,data,sizeof(char));
					offset = sizeof(char) + 2*sizeof(int);			// start of new block
					memcpy(&temp,&data[offset],sizeof(int));
					continue;
					//	go to this node
				}
				else{
					offset += 2*sizeof(int);	// distance between 2 keys
					memcpy(&temp,&data[offset],sizeof(int));
				}
		}

	}

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
	//char str[20] = hash(AM_errno);
	//printf("%s\n",str);


}

void AM_Close() {

}
