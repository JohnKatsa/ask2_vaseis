void AM_InsertEntry(int fileDesc, void *value1, void *value2){
	int blocks_num,offset;
	BF_Block *block;
	BF_GetBlock(fileDesc, 0, block);
	char *data = BF_Block_GetData(block);
	int l1,l2;
	char t1,t2;
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

		switch(t1){
			case 'i':
				int key;
				break;
			case 'f':
				float key;
				break;
			case 'c':
				char key[l1];
				break;
		}

		int i,kNumber; // the i_th element
		offset = 0;
		while(1){
			BF_GetBlock(fileDesc, temp, block);
			data = BF_Block_GetData(block);

			if(offset > BF_BLOCK_SIZE) return -1; // something is wrong
			char type;
			memcpy(&type,&data[offset],sizeof(char));

			offset += sizeof(char) + 2*sizeof(int); //first key
			while(type == 'k'){
				// na ftia3w tis if gia maxsize klp
				memcpy(&key,&data[offset],l1);	// key to copmare
				if(cmp(t1,l1,value1,&key) == -1){
					stack.push(temp);	// push current block to stack
					offset -= sizeof(int) ; // find block pointer
					memcpy(&temp,&data[offset],sizeof(int));
					offset = 0;
					break;
				}
				else offset += sizeof(int) + l1;
			}
			if(type == 'd'){
				if(getKeysNumber(temp) < MAXKEYS) { 	//def MAXKEYS
					offset = sizeof(char) + sizeof(int) + getKeysNumber(temp)*(l1+l2) ;
					memcpy(&data[offset],value1,l1);
					offset += l1;
					memcpy(&data[offset],value2,l2);
					offset += l2;

					i = getKeysNumber(temp);
					i++;
					// create insert_and_sort() 
					memcpy(&data[sizeof(char)],&i,sizeof(int));	// increase keysnumber
				}
				else {
					stack.push(temp);
					split(stack,value1,value2);
				}
			}
		}
	}


void split(Stack stack, void* value1,void* value2){
	int current = stack.pop();
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
	void key,value,tempkey,tempvalue;

	for(i=0;i<=(getKeysNumber(current)-1)/2;i++)	{	// ================   condition?? =================
		memcpy(&key,&data[offset],l1);
		offset += l1;
		memcpy(&value,&data[offset],l2);
		offset += l2;
		if(cmp(....)){	// swap(t1,t2,l1,l2,value1,value2);
			offset = offset - (l1 + l2);
			flag = 1;
			memcpy(&tempkey,&data[offset],l1);
			offset += l1;
			memcpy(&tempvalue,&data[offset],l2);
			offset -= l1; // go back to overwrite data

			memcpy(&data[offset],value1,l1);
			offset += l1;
			memcpy(&data[offset],value2,l2);
			offset += l2;

			memcpy(&key,&tempkey,l1);
			memcpy(&value,&tempvalue,l2);

			for(int j=i;j<=(getKeysNumber(current)-1)/2;j++){
				memcpy(&tempkey,&data[offset],l1);
				offset += l1;
				memcpy(&tempvalue,&data[offset],l2);
				offset -= l1; 

				memcpy(&data[offset],&key,l1);
				offset += l1;
				memcpy(&data[offset],&value,l2);
				offset += l2;

				memcpy(&key,&tempkey,l1);
				memcpy(&value,&tempvalue,l2);
			}
			break;
		}
	}
	setKeysNumber(current,i);

	int offset1 = offset;
	
	BF_AllocateBlock(block);		// pote allazw current ????
	char* data2 = BF_Block_GetData(block);

	int blocks_num;
	BF_GetBlockCounter(fileDesc,&blocks_num);
	blocks_num--;
	memcpy(data2,type,sizeof(char));
	offset = sizeof(char);

	int number = getKeysNumber(current)/2;
	memcpy(&data2[offset],&number,sizeof(int));
	offset += sizeof(int);
	if(type == 'd'){	
	number = getRightSibling(current);
	
	memcpy(&data2[offset],&number,sizeof(int));
	offset += sizeof(int);

	memcpy(&data[sizeof(char)+sizeof(int)],&blocks_num,sizeof(int)); // change sibling
	}
	if(type == 'd'){
	memcpy(&data2[offset],&key,l1);			// ============================== mallon den xreizetai auto giati to key anebainei================================
	offset += l1;

	memcpy(&data2[offset],&value,l2);
	offset += l2;
	}

	for(i=(getKeysNumber(current)+1)/2;i<getKeysNumber(current);i++){  
		if(flag == 0 && cmp(...)){
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
	if(current == getRoot()) {
		BF_AllocateBlock(block);
		data = BF_Block_GetData(block);
		type = 'k';
		memcpy(data,&type,sizeof(char));
		offset = sizeof(char);

		number = 1;	// one record
		memcpy(&data[offset],&number,sizeof(int));
		offset += sizeof(int);
		BF_GetBlockCounter(fileDesc,&blocks_num);
		insert(fileDesc,--blocks_num,&key,&(--blocks_num));		// =============== right values? ==============

		setRoot(fileDesc,blocks_num);
		return AME_OK;
	}

	current=stack.pop();
	int newblock;
	BF_GetBlockCounter(fileDesc,&newblock);
	newblock--;	// last created block is te splited one
	if(getKeysNumber(current) >= MAXKEYS) {
		stack.push(current);
		split(stack,&key,&newblock);
	}
	else insert(current,value1);


	
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



	























