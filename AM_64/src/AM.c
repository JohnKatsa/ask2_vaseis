#include "AM.h"
#include "structs.h"
#include <string.h>
#include "bf.h"
#include "StackInterface.h"
struct scan *Scans[20];
struct openedFiles *OpenFiles[20];

int AM_errno = AME_OK;

int sizeofr;	// size of record
int sizeofb;	// number of entries the block can keep

void AM_Init() {
	BF_Init(LRU);
	return;
}

void insert(int fd, int block_num,void* p1,void* p2)	//filedesc, block_num(lets say current block), p1(key or value1), p2(pointer or value2)
{
	char type = getType(fd,block_num);

	int l1 = get_l1(fd);
	int l2 = get_l2(fd);

	BF_Block *block;
        BF_Block_Init(&block);
        BF_GetBlock(fd,block_num,block);
        char* data = BF_Block_GetData(block);

        int offset = sizeof(char) + 2*sizeof(int);
        void* temp = malloc(l1);                                       // value1 < temp
        memcpy(temp,&data[offset],l1);
	void *key,*pointer;
	if(type == 'd'){
		key = malloc(l1); 			  // l1 bytes for v1
 		pointer = malloc(l2);                     // l2 bytes for v2

        	for(int i = 0; i < sizeofdb; i++){
                	if(cmp(get_t1(fd),l1,p1,temp) < 0){
                        	memcpy(key,&data[offset],l1);                                  // take current key 
                        	memcpy(pointer,&data[offset-sizeof(int)],l2);		       // and pointer

                        	memcpy(&data[offset],p1,l1);                               	// write new key 
                        	memcpy(&data[offset-sizeof(int)],p2,l2);		 	// and pointer (= block_number)

                        	memcpy(p1,key,l1);                           	// iterate with new key 
                        	memcpy(p2,pointer,l2);			 	// and pointer (= block_number)

                	}
                	offset += l1 + l2;
        	}
	}
	else{
		key = malloc(l1); 				   // l1 bytes for key (= sizeof(value1))
 		pointer = malloc(sizeof(int));                     // 4 bytes for pointer

		for(int i = 0; i < sizeofib; i++){
                	if(cmp(t1,l1,p1,temp) < 0){
                                  memcpy(key,&data[offset],l1);                                  // take current key 
                                  memcpy(pointer,&data[offfset-sizeof(int)],sizeof(int));                 // and pointer
  
                                  memcpy(&data[offset],p1,l1);                                    // write new key 
                                  memcpy(&data[offset-sizeof(int)],p2,sizeof(int));               // and pointer (= block_number)
           
                                  memcpy(p1,key,l1);                              // iterate with new key 
                                  memcpy(p2,pointer,sizeof(int));                 // and pointer (= block_number)
           
                	}
                        offset += l1 + sizeof(int);
                }
	}

}



void setKeysNumber(int fileDesc,int block_num,int new_num){
	BF_Block *block;	// Init na min 3exasw
	BF_GetBlock(fileDesc,block_num,block);
	char *data = BF_Block_GetData(block);
	int offset = sizeof(char);
	
	memcpy(&data[offset],&new_num,sizeof(int));

}

void setRoot(int fileDesc,int new_root){
	BF_Block *block;
	BF_GetBlock(fileDesc,0,block);
	char *data = BF_Block_GetData(block);

	int offset = strlen("B+Tree")+1+sizeof(int)*2 + sizeof(char)*2;

	memcpy(&data[offset],&new_root,sizeof(int));
}

int getRightSibling(int fileDesc, int block_num){
	BF_Block *block;        // Init na min 3exasw
	BF_GetBlock(fileDesc,block_num,block);
	char *data = BF_Block_GetData(block);
	int offset = sizeof(char)+sizeof(int);
	int sibling;
	memcpy(&sibling,&data[offset],sizeof(int));
	return sibling;
}

int get_l1(int fd)
{
    BF_Block *block;
    BF_Block_Init(&block);
    BF_GetBlock(fd,0,block);
    char* data = BF_Block_GetData(block);

    int offset = sizeof("B+Tree")+1 + sizeof(char);

    int l1;
    memcpy(&l1,&data[offset],sizeof(int));

    BF_UnpinBlock(block);

    return l1;
}

int get_l2(int fd)
{
    BF_Block *block;
    BF_Block_Init(&block);
    BF_GetBlock(fd,0,block);
    char* data = BF_Block_GetData(block);

    int offset = sizeof("B+Tree")+1 + 2*sizeof(char) + sizeof(int);

    int l2;
    memcpy(&l2,&data[offset],sizeof(int));

    BF_UnpinBlock(block);

    return l2;
 }

char get_t1(int fd)
{
    BF_Block *block;
    BF_Block_Init(&block);
    BF_GetBlock(fd,0,block);
    char* data = BF_Block_GetData(block);

    int offset = sizeof("B+Tree")+1;
 
    char t1;
    memcpy(&t1,&data[offset],sizeof(char));
 
    BF_UnpinBlock(block);
 
    return t1;
}
 

char get_t2(int fd)
{
    BF_Block *block;
    BF_Block_Init(&block);
    BF_GetBlock(fd,0,block);
    char* data = BF_Block_GetData(block);

    int offset = sizeof("B+Tree")+1 + sizeof(char) + sizeof(int);

    char t2;
    memcpy(&t2,&data[offset],sizeof(char));

    BF_UnpinBlock(block);

   return t2;
}

int getmax(int fd, int type)
{
    BF_Block *block;
    BF_Block_Init(&block);
    BF_GetBlock(fd,0,block);
    char* data = BF_Block_GetData(block);

    int s1, s2;

    if(type == 0){	// index
	s1 = get_l1(fd);
	s2 = sizeof(int);
    }
    else{		// data
	s1 = get_l1(fd);
	s2 = get_l2(fd);
    }

    return (BF_BLOCK_SIZE - (sizeof(char) + 2*sizeof(int)))/(s1 + s2);
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
	for(int i = 0; i < 20; i++){
	if(OpenFiles[i] != NULL){
	if(!strcmp(fileName,OpenFiles[i]->fileName)){  //if opened and found
	    remove(fileName);
    	}
}
	break;
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
	int i;
	for( i=0;i<20;i++){
		if(OpenFiles[i] != NULL){
			if(OpenFiles[i]->fileDesc == fileDesc) break;
		}
	}
	if(i>=20){  // if no scans running
		for(int j=0;j<20;j++){
			if(OpenFiles[j] != NULL){
				if(OpenFiles[j]->fileDesc == fileDesc){
					free(OpenFiles[j]);
					OpenFiles[j] = NULL;
					return AME_OK;
				}
			}
		}
//		return AME_OK;
	}
}

//////////////////////////////////////////////////////////////////////////////////

char getType(int fileDesc, int blockNum){
	BF_Block *block;
	BF_GetBlock(fileDesc,blockNum,block);

	char temp,*data;
	data = BF_Block_GetData(block);

	memcpy(&temp,data,sizeof(char));
	return temp;
}

int getKeysNumber(int fileDesc, int blockNum){
	BF_Block *block;
	BF_GetBlock(fileDesc,blockNum,block);

	int number;
	char *data;
	data = BF_Block_GetData(block);

	memcpy(&number,&data[sizeof(char)],sizeof(int));
	return number;
}

int getRoot(int fileDesc){
	BF_Block *block;
	BF_GetBlock(fileDesc,0,block);
	
	int root;
	char *data;
	data = BF_Block_GetData(block);
	
	int offset = strlen("B+Tree")+1 +2*sizeof(int) + 2*sizeof(char);
	memcpy(&root,&data[offset],sizeof(int));

	return root;
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

		int blocks_num,offset;
	BF_Block *block;
	BF_Block_Init(&block);
	BF_GetBlock(fileDesc, 0, block);
	char *data = BF_Block_GetData(block);
	int l1,l2;
	char t1,t2;
	Stack s;
	InitializeStack(&s);
	offset = strlen("B+Tree")+1;

	memcpy(&t1,&data[offset],sizeof(char));
	offset += sizeof(char);

	memcpy(&l1,&data[offset],sizeof(int));
	offset += sizeof(int); 

	memcpy(&t2,&data[offset],sizeof(char));
	offset += sizeof(char);

	memcpy(&l2,&data[offset],sizeof(int)); 
	offset += sizeof(int);

	BF_GetBlockCounter(fileDesc,&blocks_num);
	int temp;
	if(blocks_num == 1) {
		BF_AllocateBlock(fileDesc,block); 
		memcpy(&data[offset],&blocks_num,sizeof(int)); // make it root
		int offset = 0, temp;
		char type='d';
		// set data or index type
		data = BF_Block_GetData(block);
		memcpy(data,&type,sizeof(char));
		offset += sizeof(char);

		//set keys nymber to 1
		temp = 1; // fisrt record
		memcpy(&data[offset],&temp,sizeof(int));
		offset += sizeof(int);

		//insert data
		memcpy(&data[offset],value1,l1);
		offset += l1;
		memcpy(&data[offset],value2,l2);
		offset += l2;

		BF_Block_SetDirty(block);
	}
	else {
		temp = getRoot(fileDesc);
		//		BF_GetBlock(fileDesc, temp, block);
		//		data = BF_Block_GetData(block);

		void *key = malloc(l1);
		void *value = malloc(l2);

		int i=0,kNumber; // the i_th element
		offset = 0;
		while(1){
			BF_GetBlock(fileDesc, temp, block);
			data = BF_Block_GetData(block);

			if(offset > BF_BLOCK_SIZE) return -1; // something is wrong
			char type;
			memcpy(&type,&data[offset],sizeof(char));

			offset += sizeof(char) + 2*sizeof(int); //first key
			while(type == 'k' && i < getKeysNumber(fileDesc,temp)){
				if(offset >= BF_BLOCK_SIZE) return AME_EOF;		// ================= wtf?
				// na ftia3w tis if gia maxsize klp
				memcpy(key,&data[offset],l1);	// key to copmare
				if(cmp(t1,l1,value1,key) < 0 ){
					Push(temp,&s);	// push current block to stack
					offset -= sizeof(int) ; // find block pointer
					memcpy(&temp,&data[offset],sizeof(int));
					offset = 0;
					break;
				}
				else {
					offset += sizeof(int) + l1;
					i++;
				}
			}
			if(type == 'd'){
				if(getKeysNumber(fileDesc,temp) < getmax(fileDesc,0)) { 	//def MAXKEYS
					offset = sizeof(char) + 2*sizeof(int) + (getKeysNumber(fileDesc,temp)-1)*(l1+l2) ;	// mipws den 8elei -1?
					memcpy(&data[offset],value1,l1);
					offset += l1;
					memcpy(&data[offset],value2,l2);
					offset += l2;

					i = getKeysNumber(fileDesc,temp);
					i++;
					// create insert_and_sort() 
					memcpy(&data[sizeof(char)],&i,sizeof(int));	// increase keysnumber
				}
				else {
					Push(temp,&s);
					split(fileDesc,&s,value1,value2);
				}
			}
		}
	}


void split(int fileDesc,Stack *stack, void* value1,void* value2){
	int current;
	Pop(stack,&current);
	BF_GetBlock(fileDesc, current, block);
	char* data = BF_Block_GetData(block);
	char type;
	void *upkey;

	memcpy(&type,data,sizeof(char));	// block type
	int i,offset;
	if(type == 'd') {
		offset = sizeof(char) + 2*sizeof(int);	// ignore type,keys_num,sibling
		l1 = get_l1();	// ====================
		l2 = get_l2();
	}
	else{
	       offset = sizeof(char) + sizeof(int);
	       l1 = sizeof(int);
	       l2 = get_l1();
	}
	
	int flag = 0;
	char *key,*value,*tempkey,*tempvalue;
	key = malloc(l1);
	tempkey = malloc(l1);
	value = malloc(l2);
	tempvalue = malloc(l2);

	for(i=0;i<=(getKeysNumber(fileDesc,current)-1)/2;i++)	{	// ================   condition?? =================
		memcpy(key,&data[offset],l1);
		offset += l1;
		memcpy(value,&data[offset],l2);
		offset += l2;
		if(cmp(t1,l1,value1,key)){	// swap(t1,t2,l1,l2,value1,value2);
			offset = offset - (l1 + l2);
			flag = 1;
			memcpy(tempkey,&data[offset],l1);
			offset += l1;
			memcpy(tempvalue,&data[offset],l2);
			offset -= l1; // go back to overwrite data

			memcpy(&data[offset],value1,l1);
			offset += l1;
			memcpy(&data[offset],value2,l2);
			offset += l2;

			memcpy(key,tempkey,l1);
			memcpy(value,tempvalue,l2);

			for(int j=i;j<=(getKeysNumber(fileDesc,current)-1)/2;j++){
				memcpy(tempkey,&data[offset],l1);
				offset += l1;
				memcpy(tempvalue,&data[offset],l2);
				offset -= l1; 

				memcpy(&data[offset],key,l1);
				offset += l1;
				memcpy(&data[offset],value,l2);
				offset += l2;

				memcpy(key,tempkey,l1);
				memcpy(value,tempvalue,l2);
			}
			break;
		}
	}
	setKeysNumber(fileDesc,current,i);

	int offset1 = offset;
	
	BF_AllocateBlock(fileDesc,block);		// pote allazw current ????
	char* data2 = BF_Block_GetData(block);

	int blocks_num;
	BF_GetBlockCounter(fileDesc,&blocks_num);
	blocks_num--;
	memcpy(data2,&type,sizeof(char));
	offset = sizeof(char);

	int number = getKeysNumber(fileDesc,current)/2;		// is it fine? =========================
	memcpy(&data2[offset],&number,sizeof(int));
	offset += sizeof(int);
	if(type == 'd'){	
	number = getRightSibling(fileDesc,current);
	
	memcpy(&data2[offset],&number,sizeof(int));
	offset += sizeof(int);

	memcpy(&data[sizeof(char)+sizeof(int)],&blocks_num,sizeof(int)); // change sibling
	}
	if(type == 'd'){
	memcpy(&data2[offset],key,l1);			// ============================== mallon den xreizetai auto giati to key anebainei================================
	offset += l1;

	memcpy(&data2[offset],value,l2);
	offset += l2;
	}

	for(i=(getKeysNumber(fileDesc,current)+1)/2;i<getKeysNumber(fileDesc,current);i++){  
		if(flag == 0 && cmp(t1,l1,value1,&data[offset1])){		// is it offset1? ====================
			memcpy(&data[offset],value1,l1);
			offset += l1;
			memcpy(&data[offset],value2,l2);
			offset += l2; 			// ========= need one more loop? =========
			flag = 1;

		}
		memcpy(&data2[offset],&data[offset1],l1);
		offset += l1;
		offset1 += l1;

		memcpy(&data2[offset],&data[offset1],l2);
		offset += l2;
		offset1 += l2;
	}
	if(current == getRoot(fileDesc)) {
		BF_AllocateBlock(fileDesc,block);
		data = BF_Block_GetData(block);
		type = 'k';
		memcpy(data,&type,sizeof(char));
		offset = sizeof(char);

		number = 1;	// one record
		memcpy(&data[offset],&number,sizeof(int));
		offset += sizeof(int);
		BF_GetBlockCounter(fileDesc,&blocks_num);
		blocks_num--;
		insert(fileDesc,--blocks_num,&key,&blocks_num);		// =============== right values? ==============

		setRoot(fileDesc,blocks_num);
		return AME_OK;
	}

//	current=stack.pop();
	Pop(stack,&current);
	int newblock;
	BF_GetBlockCounter(fileDesc,&newblock);
	newblock--;	// last created block is te splited one
	int id;
	if(type == 'd') id = 1;
	else id = 0;
	if(getKeysNumber(fileDesc,current) >= getmax(fileDesc,id)) {
		Push(current,stack);
		split(fileDesc,stack,&key,&newblock);
	}
	else insert(fileDesc,current,value1,&newblock);	// newblock >?>?> ===========


	
}


	int operation(int op,char type,int l1, void *value1,void* value2){
		switch(op) {
			case 1:
				if(type == 'i') return *(int*)value1 == *(int*)value2;
				else if(type == 'f') return *(float*)value1 == *(float*)value2;
				else return !strncmp(value1,value2,l1);
				break;
			case 2: 
				if(type == 'i') return *(int*)value1 != *(int*)value2;
				else if(type == 'f') return *(float*)value1 != *(float*)value2;
				else return strncmp(value1,value2,l1);
		}
	}	
	int AM_OpenIndexScan(int fileDesc, int op, void *value) {
		Scan *s = malloc(sizeof(Scan));
		s->fileDesc = fileDesc;
		s->op = op;
		s->value = value;
		s->offset = 0;

		BF_Block *block;
		BF_Block_Init(&block);
		BF_GetBlock(fileDesc,0,block);
		char *data = BF_Block_GetData(block);
		int index2 = strlen("B+Tree")+1+2*sizeof(char) + 2*sizeof(int);
		int root;
		memcpy(&root,&data[index2],sizeof(int));
		BF_GetBlock(fileDesc,root,block);
		data = BF_Block_GetData(block);
		char index;
		memcpy(&index,data,sizeof(char));
		int offset = sizeof(char);
		int seq = 0;
		int l1 = get_l1():
		char t1 = get_t1();
		while(1){
			if(offset + sizeof(int)  <= BF_BLOCK_SIZE){	// correct condition?
				offset += sizeof(int); // ignore block_pointer and get the key
				int key;
				memcpy(&key,&data[offset],l1); // CASES!!
				if(operation(op,t1,key,value) ) {
					if(index == 'k'){
						offset -= sizeof(int);	// go back to block_pointer
						memcpy(&root,&data[offset],sizeof(int));
						BF_GetBlock(fileDesc,root,block);
						data = BF_Block_GetData(block);
						memcpy(&index,data,sizeof(char));
						offset = sizeof(char);
						seq = 0;
						continue;
					}
					else {
						s->blockNum = root;
						s->offset = seq;
						break;
					}
				}
				else {
					offset += l1;
					seq++;
				}

			}
			else return -1;
		}


		int scanDesc=0;
		while(Scans[scanDesc] != NULL) scanDesc++;
		Scans[scanDesc] = s;
		return scanDesc;
	}


	void *AM_FindNextEntry(int scanDesc) {
		Scan *s;// = Scans[scanDesc];
		for(int i=0;i<20;i++)
			if(Scans[i] != NULL)
				if(Scans[i]->scanDesc == scanDesc)
					s = Scans[i];

		BF_Block *block;
		BF_GetBlock(s->fileDesc,s->blockNum,block);
		char *data = BF_Block_GetData(block);

		int offset = sizeof(char) + sizeof(int)*2 + s->offset*(get_l1(s->fileDesc)+get_l2(s->fileDesc));

		if(operation(s->op,get_t1(s->fileDesc),&data[offset],s->value)){	// na ftia3w to offset 
			if(s->offset < getKeysNumber(s->fileDesc,s->blockNum)){
				s->offset++;
			}
			else {
				s->offset = 0;
				s->blockNum = getRightSibling(s->fileDesc,s->blockNum);
			}

			return &data[s->offset*sizeof(int)];
		}
		return NULL;


	}


	int AM_CloseIndexScan(int scanDesc) {
		for(int i=0;i<20;i++){
			if(Scans[i] != NULL){
				if(Scans[i]->scanDesc == scanDesc){
					free(Scans[i]);
					Scans[i]=NULL;
				}
			}
		}
		return AME_OK;
	}


	void AM_PrintError(char *errString) {
		printf("%s\n",errString);	/* Hash table for errors must be implemented */
		//char str[20] = hash(AM_errno);
		//printf("%s\n",str);


	}

	void AM_Close() {

	}
