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

	if(type == 'd'){
		key = malloc(l1); 			  // l1 bytes for v1
 		pointer = malloc(l2);                     // l2 bytes for v2

        	for(int i = 0; i < sizeofdb; i++){
                	if(cmp(t1,l1,p1,temp) < 0){
                        	memcpy(key,&data[offset],l1);                                  // take current key 
                        	memcpy(pointer,&data[offfset-sizeof(int)],l2);		       // and pointer

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

typedef enum BF_ErrorCode {
    AM_OK,
    AM_OPEN_FILES_LIMIT_ERROR,     /* Υπάρχουν ήδη BF_MAX_OPEN_FILES αρχεία ανοικτά */
    AM_INVALID_FILE_ERROR,         /* Ο αναγνωριστικός αριθμός αρχείου δεν αντιστιχεί σε κάποιο ανοιχτό αρχείο */
    AM_ACTIVE_ERROR,               /* Το επίπεδο BF είναι ενεργό και δεν μπορεί να αρχικοποιηθεί */
    AM_FILE_ALREADY_EXISTS,        /* Το αρχείο δεν μπορεί να δημιουργιθεί γιατι υπάρχει ήδη */
    AM_FULL_MEMORY_ERROR,          /* Η μνήμη έχει γεμίσει με ενεργά block */
    AM_INVALID_BLOCK_NUMBER_ERROR, /* Το block που ζητήθηκε δεν υπάρχει στο αρχείο */
    AM_AVAILABLE_PIN_BLOCKS_ERROR,
    AM_ERROR
 } BF_ErrorCode;

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

int AM_DestroyIndex(char *fileName){
    for(int i = 0; i < 20; i++){
	if(!strcmp(fileName,OpenFiles[i]->fileName)){  //if opened and found
	    remove(fileName);
    	}
	break;
    }
    return AME_OK;
}

