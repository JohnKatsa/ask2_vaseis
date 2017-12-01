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
	int i,offset=0;
	for(i=0;i<=getKeysNumber(current)/2;i++){
		if(cmp(....)) swap(t1,t2,l1,l2,value1,value2);
	}
	setKeysNumber(current,i);
	
	memcpy(upkey,data[offset1],l1);	// key to climb up after split

	BF_AllocateBlock(block);		// pote allazw current ????
	char* data2 = BF_Block_GetData(block);

	memcpy(data2,type,sizeof(char));
	offset = sizeof(char);

	int number = getKeysNumber(current)/2;
	memcpy(&data2[offset],&number,sizeof(int));
	offset += sizeof(int);
	
	number = getRightSibling(current);

	memcpy(&data2[offset],&number,sizeof(int));
	offset += sizeof(int);

	number = getLeftSibling(current);

	memcpy(&data2[offset],&number,sizeof(int));
	offset += sizeof(int);

	if(type == 'k') offset1 += l1 + sizeof(int);		// dont write twice upkey


	for(i=getKeysNumber(current)/2+1;i<getKeysNumber(current);i++){  
		if(type == 'k') offset1 += l1 + sizeof(int); 		
		memcpy(&data2[offset],&data[offset1],l1);
		offset += l1;
		offset1 += l1;

		if(type == 'k') {
			memcpy(&data[offset],&data2[offset1],sizeof(int));
			offset += sizeof(int);
			offset1 += sizeof(int);
		}
		else {
			memcpy(&data[offset],&data2[offset1],l2);
			offset += l2;
			offset1 += l2;
		}
	}

	current=stack.pop();
	int newblock;
	BF_GetBlockCounter(fileDesc,&newblock);
	newblock--;	// last created block is te splited one
	if(getKeysNumber(current) >= MAXKEYS) {
		stack.push(current);
		split(stack,value1,&newblock);
	}
	else insert(current,value1);


	
























