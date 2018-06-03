#define PAGESIZE 4096
#include "bpt.h"

int fd[11] = {0};
char copy[120]={};
int inbuffer = 0;
int max = 0;
buffer * buffer_array = NULL;


/*
 * join
 */

int join_table(int table_id1, int table_id2, char * pathname){

	if(fd[table_id1] == 0 || fd[table_id2] == 0){
		perror("table is not open");
		return -1;
	}

	int result_key_num = 0;
	int key_num1=0;
 	int key_num2=0;
 	short finish = 0;
	off_t leaf_ofset1 = find_leaf(table_id1, 0);
	off_t leaf_ofset2 = find_leaf(table_id2, 0);	
	FILE * fp = fopen(pathname, "w+");
	page page_one = buffer_array[readpage(table_id1, leaf_ofset1, 0)].frame.normal_page;
	page page_two = buffer_array[readpage(table_id2, leaf_ofset2, 0)].frame.normal_page;
	int hold_index_one = hold_page();
	int hold_index_two = hold_page();
	int hold_index = hold_index_one;

	while(1){

		while(page_one.data.records[key_num1].key<page_two.data.records[key_num2].key){
			
			key_num1++;

			if(key_num1==page_one.header.key_number){
				if(page_one.more_offset == 0){
					finish =1;
					break;
				}
				page_one = buffer_array[readpage(table_id1, page_one.more_offset, 0)].frame.normal_page;
				key_num1 = 0;
			}
		}

		if (finish) break;

		while(page_one.data.records[key_num1].key>page_two.data.records[key_num2].key){

			key_num2++;

			if(key_num2==page_two.header.key_number){
				if(page_two.more_offset == 0){
					finish = 1;
					break;
				}
				page_two = buffer_array[readpage(table_id2, page_two.more_offset, 0)].frame.normal_page;
				key_num2 = 0;
			}
		}

		if (finish) break;

		if(page_one.data.records[key_num1].key == page_two.data.records[key_num2].key){

			buffer_array[hold_index].frame.normal_page.data.records[result_key_num].key = page_one.data.records[key_num1].key;
			strcpy(buffer_array[hold_index].frame.normal_page.data.records[result_key_num].value,page_one.data.records[key_num1].value);
			result_key_num++;

			if(result_key_num == 31 && hold_index == hold_index_one){
				result_key_num = 0;
				hold_index = hold_index_two;
			}

			buffer_array[hold_index].frame.normal_page.data.records[result_key_num].key = page_two.data.records[key_num2].key;
			strcpy(buffer_array[hold_index].frame.normal_page.data.records[result_key_num].value,page_two.data.records[key_num2].value);
			result_key_num++;

			if(result_key_num == 31 && hold_index == hold_index_two){
				result_key_num += 31;
				disk_write(fp, hold_index_one, hold_index_two, result_key_num);
				hold_index = hold_index_one;
				result_key_num = 0;
			}

			key_num1++;

			if(key_num1==page_one.header.key_number){
				if(page_one.more_offset == 0) break;

				page_one = buffer_array[readpage(table_id1, page_one.more_offset, 0)].frame.normal_page;
				key_num1 = 0;
			}

		}

	}

	if(hold_index == hold_index_two) result_key_num += 31;
	disk_write(fp, hold_index_one, hold_index_two, result_key_num);

	if(buffer_array[hold_index_one].pin_count>0) buffer_array[hold_index_one].pin_count--;	
	if(buffer_array[hold_index_two].pin_count>0) buffer_array[hold_index_two].pin_count--;

	fclose(fp);

	return 0;
}

void disk_write(FILE *fp, int index_one, int index_two, int result_key){

	int double_check = 0;
	int result_key_num = result_key;
	int index = index_one;
	int64_t key;
	char * value;
	int i = 0;

	if(result_key_num>31){
		double_check = 1;
		result_key_num -= 31;
	}

	if(double_check){
		while(1){
			key = buffer_array[index].frame.normal_page.data.records[i].key;
			value = buffer_array[index].frame.normal_page.data.records[i++].value;
			fprintf(fp,"%ld,%s,",key,value);
			if(i==31 && index == index_one){
				index = index_two;
				i = 0;
			}
			value = buffer_array[index].frame.normal_page.data.records[i++].value;
			fprintf(fp,"%ld,%s\n",key,value);
			if(i==result_key_num && index == index_two) break;
		}
		return;
	}

	while(1){
		if(i==result_key_num) break;
		key = buffer_array[index].frame.normal_page.data.records[i].key;
		value = buffer_array[index].frame.normal_page.data.records[i++].value;
		fprintf(fp, "%ld,%s,", key, value);
		value = buffer_array[index].frame.normal_page.data.records[i++].value;
		fprintf(fp, "%ld,%s\n", key, value);
	}
	return;
}

int hold_page(void){

	int i = 0;

	if(inbuffer<max){
		inbuffer++;
		buffer_array[inbuffer].pin_count++;
		i = inbuffer;
	}
	else{
		for(i=0;i<=inbuffer;i++){
			if(i==inbuffer) i = 0;

			if(buffer_array[i].pin_count ==0){
				if(buffer_array[i].ref_bit ==0){
					if(buffer_array[i].is_dirty == 1){
						if(buffer_array[i].page_offset ==0){
							if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.header,PAGESIZE,SEEK_SET)<0){
								perror("write error");
								return -1;
							}
						}else{
							if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.normal_page,PAGESIZE,SEEK_SET + buffer_array[i].page_offset)<0){
								perror("write error");
								return -1;
							}
						}
						buffer_array[i].is_dirty = 0;
					}break;
				}else buffer_array[i].ref_bit = 0;
			}
		}

		memset(&buffer_array[i],0,sizeof(buffer));
		buffer_array[i].pin_count++;
	}

	return i;
}


/*
 * buffer
 */

int init_db (int num_buf){

	if(num_buf<16){
		perror("number of buffer have to bigger than 16");
		exit(EXIT_FAILURE);
	}
	max = num_buf;
	buffer_array = (buffer*)calloc(1,sizeof(buffer)*num_buf);
	if(buffer_array == NULL){
		perror("buffer array alloc error");
		return -1;
	}
	else return 0;
}

int readpage(int table_id, off_t offset, int pin){

	int i = 0;

	for(i=0;i<inbuffer;i++){
		if(buffer_array[i].page_offset == offset && buffer_array[i].table_id == table_id) break;
	}
	if(i>=inbuffer){
		if(inbuffer==max){
			for(i=0;i<=inbuffer;i++){
				if(i==inbuffer) i = 0;

				if(buffer_array[i].pin_count ==0){
					if(buffer_array[i].ref_bit ==0){
						if(buffer_array[i].is_dirty == 1){
							if(buffer_array[i].page_offset ==0){
								if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.header,PAGESIZE,SEEK_SET)<0){
									perror("write error");
									return -1;
								}
							}else{
								if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.normal_page,PAGESIZE,SEEK_SET + buffer_array[i].page_offset)<0){
									perror("write error");
									return -1;
								}
							}
							buffer_array[i].is_dirty = 0;
						}break;
					}else buffer_array[i].ref_bit = 0;
				}
			}
		}else{
			i = inbuffer;
			inbuffer++;
		}

		if(pin==1) buffer_array[i].pin_count++;
		if(offset == 0){
			if(pread(fd[table_id],&buffer_array[i].frame.header,PAGESIZE,SEEK_SET)<0){
				perror("read error");
				exit(EXIT_FAILURE);
			}
		}else{
			if(pread(fd[table_id],&buffer_array[i].frame.normal_page,PAGESIZE,SEEK_SET + offset)<0){
				perror("read error");
				exit(EXIT_FAILURE);
			}
		}
		buffer_array[i].table_id = table_id;
		buffer_array[i].page_offset = offset;
		buffer_array[i].ref_bit = 1;

		return i;
	}// not in buffer => you should have to read from disk

	if(pin==1) buffer_array[i].pin_count++;
	buffer_array[i].ref_bit = 1;
	return i;
}


int close_table(int table_id){
	int i=0;

	for(i=0;i<inbuffer;i++){
		if(buffer_array[i].table_id == table_id){
			while(buffer_array[i].pin_count != 0){};
			if(buffer_array[i].is_dirty == 1){
				if(buffer_array[i].page_offset == 0){
					if(pwrite(fd[table_id],&buffer_array[i].frame.header,PAGESIZE,SEEK_SET+buffer_array[i].page_offset)<0){
						perror("write error");
						return -1;
					}
				}
				else{
					if(pwrite(fd[table_id],&buffer_array[i].frame.normal_page,PAGESIZE,SEEK_SET+buffer_array[i].page_offset)<0){
						perror("write error");
						return -1;
					}
				}
				buffer_array[i].is_dirty = 0;
			}
			buffer_array[i].table_id = 0;
			buffer_array[i].ref_bit = 0;
		}
	}
	close(fd[table_id]);
	fd[table_id] = 0;
	return 0;
}



int shutdown_db(void){

	int i = 0;

	for(i=0;i<inbuffer;i++){

		while(buffer_array[i].pin_count != 0){};
		if(buffer_array[i].is_dirty == 1 && buffer_array[i].table_id != 0){
			if(buffer_array[i].page_offset == 0){
				if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.header,PAGESIZE,SEEK_SET)<0){
					perror("write error");
					return -1;
				}
			}
			else{
				if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.normal_page,PAGESIZE,SEEK_SET+buffer_array[i].page_offset)<0){
					perror("write error");
					return -1;
				}
			}
		}
	}
	free(buffer_array);

	for(i=1;i<11;i++){
		if(fd[i]!=0) close(fd[i]);
	}

	return 0;
}




/*
 * open
 */

int open_table (char *pathname){
	int i = 0;
	int table_id=0;

	if(buffer_array == NULL){
		perror("buffer is Null");
		exit(EXIT_FAILURE);
	}

	for(i=1;i<11;i++){
		if(fd[i] == 0) break;
	}
	if(i>=11){
		perror("no space to store the table_id");
		exit(EXIT_FAILURE);
	}
	table_id = i;

	if((fd[table_id] = open(pathname,O_RDWR|O_CREAT|O_EXCL,0777))>0){
		

		header_page *header;

		header=(header_page *)calloc(1,PAGESIZE);
		header->rootp_offset = 0;
		header->freep_offset = 0;
		header->page_number = 1;
		
		if(inbuffer==max){
			for(i=0;i<=inbuffer;i++){
				if(i==inbuffer) i = 0;

				if(buffer_array[i].pin_count ==0){
					if(buffer_array[i].ref_bit ==0){
						if(buffer_array[i].is_dirty == 1){
							if(buffer_array[i].page_offset ==0){
								if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.header,PAGESIZE,SEEK_SET)<0){
									perror("write error");
									return -1;
								}
							}else{
								if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.normal_page,PAGESIZE,SEEK_SET + buffer_array[i].page_offset)<0){
									perror("write error");
									return -1;
								}
							}
						}break;
					}else buffer_array[i].ref_bit = 0;
				}
			}
		}else{
			i = inbuffer;
			inbuffer++;
		}
		buffer_array[i].frame.header = *header;
		buffer_array[i].table_id = table_id;
		buffer_array[i].page_offset = 0;
		buffer_array[i].is_dirty = 1;
		buffer_array[i].ref_bit = 1;

		free(header);
		return table_id;
	}
	
	else if((fd[table_id] = open(pathname,O_RDWR))>0) return table_id;
	else return -1;
}


/*
 * find
 */

off_t find_leaf(int table_id, int64_t key) {

	off_t ofsnum;
	int i = 0;
	header_page header;
	page c;

	header = buffer_array[readpage(table_id, 0, 0)].frame.header;

	if (header.rootp_offset == 0) {
	        return 0;
	}
	ofsnum = header.rootp_offset;

	c = buffer_array[readpage(table_id, ofsnum, 0)].frame.normal_page;

	while (!(c.header).isleaf) {
	   	i = 0;
		while (i < (c.header).key_number) {
			if (key >= ((c.data).pageinfos[i]).key) i++;
	     	else break;
        	}

		if(i==0) ofsnum = c.more_offset;
		else ofsnum=((c.data).pageinfos[i-1]).offset;
	
		c = buffer_array[readpage(table_id,ofsnum,0)].frame.normal_page;
	}

    	return ofsnum;
}

char * find(int table_id, int64_t key) {

	if(buffer_array == NULL){
		perror("buffer is Null");
		exit(EXIT_FAILURE);
	}

    int i = 0;
    off_t offsnum = find_leaf(table_id, key);
    page c;

    if (offsnum == 0) return NULL;
    else{
		c = buffer_array[readpage(table_id,offsnum,0)].frame.normal_page;

  	 	for (i = 0; i < (c.header).key_number; i++){
       		if (((c.data).records[i]).key == key)
			break;
		}
    		if (i == (c.header).key_number) return NULL;
		else{
			strcpy(copy,(c.data).records[i].value);
			return copy;
    		}
    }
}

/*
 * insert
 */


int cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}

int get_left_index(int table_id, off_t prnt_offset, int64_t key) {

	int left_index = 0;
	page parent;
	parent = buffer_array[readpage(table_id, prnt_offset, 0)].frame.normal_page;
	if (key < (parent.data).pageinfos[0].key)
		return -1;

	while (left_index < (parent.header).key_number - 1 && (parent.data).pageinfos[left_index+1].key <= key)
		left_index++;
    return left_index;
}

record * make_record(int64_t key, char* value) {
	
	record * new_record;

	new_record=(record*)malloc(sizeof(record));

	new_record->key = key;
	strcpy(new_record->value,value);
    	
	return new_record;
}


int insert(int table_id, int64_t key, char* value) {

	record * pointer;
	page leaf;
	off_t leaf_ofset;
	header_page header;
    int h, l;
    
    /* The current implementation ignores
     * duplicates.
     */

	if (find(table_id, key) != NULL)
		return -1;

    /* Create a new record for the
     * value.
     */
	pointer = make_record(key, value);
	
	h = readpage(table_id, 0, 1);
	header = buffer_array[h].frame.header;

    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

	if (header.rootp_offset == 0){
		start_new_tree(table_id, pointer);
		free(pointer);
		return 0;
	}

    /* Case: the tree already exists.
     * (Rest of function body.)
     */
	
	leaf_ofset = find_leaf(table_id, key);
	l = readpage(table_id, leaf_ofset, 1);
	leaf = buffer_array[l].frame.normal_page;

    /* Case: leaf has room for key and pointer.
     */

	if ((leaf.header).key_number < 31) {
		if(buffer_array[h].pin_count>0) buffer_array[h].pin_count--;
		insert_into_leaf(table_id, leaf_ofset, pointer);
		free(pointer);
	    	return 0;
	}
    /* Case:  leaf must be split.
     */
	insert_into_leaf_after_splitting(table_id, leaf_ofset, pointer);
	free(pointer);

	return 0;
}


void start_new_tree(int table_id, record* pointer) {
	
	header_page header;
	page root;
	off_t offset;
	int h, r;

	h = readpage(table_id, 0, 0);
	header=buffer_array[h].frame.header;

	if(header.freep_offset==0)
		makefrpage(table_id, 0);

	header=buffer_array[h].frame.header;
    
	offset = header.freep_offset;

	r= readpage(table_id, offset, 0);
	root = buffer_array[r].frame.normal_page;

	header.freep_offset=(root.header).offsets;
	header.rootp_offset=offset;
	(root.header).offsets=0;
	(root.header).isleaf=1;
	(root.header).key_number=1;
	root.more_offset=0;
	(root.data).records[0].key=pointer->key;
	strcpy((root.data).records[0].value, pointer->value);

	buffer_array[r].frame.normal_page = root;
	buffer_array[r].is_dirty = 1;
	buffer_array[r].ref_bit = 1;

	buffer_array[h].frame.header = header;
	buffer_array[h].is_dirty = 1;
	buffer_array[h].ref_bit = 1;
	
	if(header.freep_offset==0)
		makefrpage(table_id, 0);

	if(buffer_array[h].pin_count>0) buffer_array[h].pin_count--;
}


/* 해당 offset의 page를 free page로 만들어주기
 * 만약 offset이 0이라면 파일의 맨끝에 frpage만들기
 */
void makefrpage(int table_id, off_t offset){

	off_t oftnum = offset;

	header_page header;
	page * freepage;
	int h, i;

	freepage=(page*)calloc(1, PAGESIZE);

	h = readpage(table_id, 0, 0);
	header = buffer_array[h].frame.header;

	if(oftnum==0){
		oftnum = (header.page_number)*PAGESIZE;
		(header.page_number)++;

		if(inbuffer==max){
			for(i=0;i<=inbuffer;i++){
				if(i==inbuffer) i = 0;
				if(buffer_array[i].pin_count ==0){
					if(buffer_array[i].ref_bit ==0){
						if(buffer_array[i].is_dirty == 1){
							if(buffer_array[i].page_offset ==0){
								if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.header,PAGESIZE,SEEK_SET)<0){
									perror("write error");
									exit(EXIT_FAILURE);
								}
							}else{
								if(pwrite(fd[buffer_array[i].table_id],&buffer_array[i].frame.normal_page,PAGESIZE,SEEK_SET + buffer_array[i].page_offset)<0){
									perror("write error");
									exit(EXIT_FAILURE);
								}
							}
						}break;
					}else buffer_array[i].ref_bit = 0;
				}
			}
		}else{
			i = inbuffer;
			inbuffer++;
		}
	}else{ i = readpage(table_id,oftnum,0); }

	if(header.freep_offset==0) header.freep_offset = oftnum;
	else{
		off_t frpg_ofset = header.freep_offset;
		header.freep_offset = oftnum;
		(freepage->header).offsets=frpg_ofset;
	}
	
	buffer_array[h].frame.header = header;
	buffer_array[h].is_dirty = 1;
	buffer_array[h].ref_bit = 1;

	buffer_array[i].frame.normal_page = *freepage;
	buffer_array[i].table_id = table_id;
	buffer_array[i].page_offset = oftnum;
	buffer_array[i].is_dirty = 1;
	buffer_array[i].ref_bit = 1;

	free(freepage);
}


void insert_into_leaf(int table_id, off_t leaf_ofset, record *pointer) {
//leaf pin check
	int i, insertion_point;
	page leaf;
	int l;
	
	l = readpage(table_id,leaf_ofset, 0);
	leaf = buffer_array[l].frame.normal_page;

	insertion_point = 0;
	while (insertion_point < (leaf.header).key_number &&
			(leaf.data).records[insertion_point].key < pointer->key)
		insertion_point++;

	for (i = (leaf.header).key_number; i > insertion_point; i--) {
		(leaf.data).records[i].key = (leaf.data).records[i-1].key;
		strcpy((leaf.data).records[i].value, (leaf.data).records[i-1].value);
	}

	(leaf.data).records[insertion_point].key = pointer->key;
	strcpy((leaf.data).records[insertion_point].value, pointer->value);
	(leaf.header).key_number++;

	buffer_array[l].frame.normal_page = leaf;
	buffer_array[l].is_dirty = 1;
	buffer_array[l].ref_bit = 1;
	if(buffer_array[l].pin_count>0) buffer_array[l].pin_count--;
}

void insert_into_parent(int table_id, off_t left_ofset, off_t right_ofset, int64_t new_key){
//header, left, right pin check
	off_t prnt_ofset;
	page parent;
	page left;
	int left_index;
	int l, p;

	l = readpage(table_id, left_ofset, 0);
	left = buffer_array[l].frame.normal_page;

	prnt_ofset = (left.header).offsets;

    /* Case: new root. */

	if (prnt_ofset == 0)
		insert_into_new_root(table_id, left_ofset, new_key, right_ofset);

    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */
	else{
		p = readpage(table_id, prnt_ofset, 1); // parent pin check
		parent = buffer_array[p].frame.normal_page;

		left_index = get_left_index(table_id, prnt_ofset, (left.data).records[0].key);

    /* Simple case: the new key fits into the node. 
     */
		if(buffer_array[l].pin_count>0) buffer_array[l].pin_count--;

	 	if ((parent.header).key_number < 248){
	 		if(buffer_array[readpage(table_id,0,0)].pin_count>0) buffer_array[readpage(table_id,0,0)].pin_count--;
		    	insert_into_node(table_id, prnt_ofset, left_ofset, right_ofset, new_key, left_index);
		}
    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */
		else{
			if(buffer_array[readpage(table_id,right_ofset,0)].pin_count>0) buffer_array[readpage(table_id,right_ofset,0)].pin_count--;
			insert_into_node_after_splitting(table_id, prnt_ofset, left_ofset, right_ofset, left_index, new_key);	
		}
	}
}


void insert_into_leaf_after_splitting(int table_id, off_t leaf_ofset, record * pointer) {
//leaf, header pin check
	page leaf;
	page new_page;
	off_t newpg_ofset;
	header_page header;
	record tmp[32];
	int insertion_index, split, i, j;
	int64_t new_key;
	int h, l, n;

	h = readpage(table_id, 0, 0);
	header = buffer_array[h].frame.header;

	l = readpage(table_id, leaf_ofset, 0);
	leaf = buffer_array[l].frame.normal_page;


	if(header.freep_offset==0)
		makefrpage(table_id, 0);

	header = buffer_array[h].frame.header;

	newpg_ofset=header.freep_offset;

	n = readpage(table_id, newpg_ofset, 1); //newpg_ofset pin check
	new_page = buffer_array[n].frame.normal_page;

	header.freep_offset=(new_page.header).offsets;

	buffer_array[h].frame.header = header;
	buffer_array[h].is_dirty = 1;
	buffer_array[h].ref_bit = 1;
	
	insertion_index = 0;
   	while (insertion_index < 31 && (leaf.data).records[insertion_index].key < pointer->key)
   		insertion_index++;
	
	for (i = 0, j = 0; i < (leaf.header).key_number; i++, j++) {
	    	if (j == insertion_index) j++;
		tmp[j].key = (leaf.data).records[i].key;
		strcpy(tmp[j].value, (leaf.data).records[i].value);
	}
	
	tmp[insertion_index].key = pointer->key;
	strcpy(tmp[insertion_index].value, pointer->value);
	(leaf.header).key_number = 0;
	(new_page.header).key_number = 0;

    	split = cut(31);

	for (i = 0; i < split; i++) {
	    (leaf.data).records[i].key = tmp[i].key;
		strcpy((leaf.data).records[i].value, tmp[i].value);
	    (leaf.header).key_number++;
	}

	for (i = split, j = 0; i < 32; i++, j++) {
        	(new_page.data).records[j].key = tmp[i].key;
		strcpy((new_page.data).records[j].value, tmp[i].value);
	    (new_page.header).key_number++;
	}

	new_page.more_offset = leaf.more_offset;
	leaf.more_offset = newpg_ofset;

	char reset[120] = {};
	for (i = (leaf.header).key_number; i < 31; i++){
	    	(leaf.data).records[i].key = (int64_t)NULL;
		strcpy((leaf.data).records[i].value,reset);
	}
	for (i = (new_page.header).key_number; i < 31; i++){
	    	(new_page.data).records[i].key = (int64_t)NULL;
		strcpy((new_page.data).records[i].value, reset);
	}

	(new_page.header).offsets = (leaf.header).offsets;
	(new_page.header).isleaf = 1;
	new_key = (new_page.data).records[0].key;

	buffer_array[l].frame.normal_page = leaf;
	buffer_array[l].is_dirty = 1;
	buffer_array[l].ref_bit = 1;

	buffer_array[n].frame.normal_page = new_page;
	buffer_array[n].is_dirty = 1;
	buffer_array[n].ref_bit = 1;

	insert_into_parent(table_id, leaf_ofset, newpg_ofset, new_key );
}


void insert_into_new_root(int table_id, off_t left, int64_t key, off_t right) {
// header, right, left pin check
	header_page header;
	page root;
	page leftp;
	page rightp;
	off_t offset;
	int h, l, r, t;

	h = readpage(table_id,0,0);
	header = buffer_array[h].frame.header;

	l = readpage(table_id,left, 0);
	leftp = buffer_array[l].frame.normal_page;

	r = readpage(table_id,right,0);
	rightp = buffer_array[r].frame.normal_page;

	if(header.freep_offset==0)
		makefrpage(table_id, 0);

	header = buffer_array[h].frame.header;

	offset = header.freep_offset;

	t = readpage(table_id, offset, 0);
	root = buffer_array[t].frame.normal_page;
	
	header.freep_offset=(root.header).offsets;
	header.rootp_offset=offset;
	(root.header).offsets=0;
	(root.header).isleaf=0;
	(root.header).key_number=1;

	root.more_offset=left;
	(root.data).pageinfos[0].key=key;	
	(root.data).pageinfos[0].offset=right;

	(leftp.header).offsets = offset;
	(rightp.header).offsets = offset;

	buffer_array[t].frame.normal_page = root;
	buffer_array[t].is_dirty = 1;
	buffer_array[t].ref_bit = 1;

	buffer_array[h].frame.header = header;
	buffer_array[h].is_dirty = 1;
	buffer_array[h].ref_bit = 1;
	if(buffer_array[h].pin_count>0) buffer_array[h].pin_count--;

	buffer_array[l].frame.normal_page = leftp;
	buffer_array[l].is_dirty = 1;
	buffer_array[l].ref_bit = 1;
	if(buffer_array[l].pin_count>0) buffer_array[l].pin_count--;

	buffer_array[r].frame.normal_page = rightp;
	buffer_array[r].is_dirty = 1;
	buffer_array[r].ref_bit = 1;
	if(buffer_array[r].pin_count>0) buffer_array[r].pin_count--;
	
}


void insert_into_node(int table_id, off_t prnt_ofset, off_t left_ofset, off_t right_ofset, int64_t key, int left_index) {
// right , parent pin check
	int i;
	page parent;
	page right_page;
	int p, r;
	
	p = readpage(table_id, prnt_ofset, 0);
	parent = buffer_array[p].frame.normal_page;
	
	r = readpage(table_id, right_ofset, 0);
	right_page = buffer_array[r].frame.normal_page;
	
	(right_page.header).offsets = prnt_ofset;

	for (i = (parent.header).key_number - 1; i > left_index; i--) {
	    	(parent.data).pageinfos[i + 1].key = (parent.data).pageinfos[i].key;
	    	(parent.data).pageinfos[i + 1].offset = (parent.data).pageinfos[i].offset;
	}
	if(i == -1) parent.more_offset=left_ofset;
	else (parent.data).pageinfos[left_index].offset = left_ofset;
		
	(parent.data).pageinfos[left_index + 1].offset = right_ofset;
	(parent.data).pageinfos[left_index + 1].key = key;
	(parent.header).key_number++;
	
	buffer_array[p].frame.normal_page = parent;
	buffer_array[p].is_dirty = 1;
	buffer_array[p].ref_bit = 1;
	if(buffer_array[p].pin_count>0) buffer_array[p].pin_count--;

	buffer_array[r].frame.normal_page = right_page;
	buffer_array[r].is_dirty = 1;
	buffer_array[r].ref_bit = 1;
	if(buffer_array[r].pin_count>0) buffer_array[r].pin_count--;
}



void insert_into_node_after_splitting(int table_id, off_t prnt_ofset, off_t left, off_t right, int left_index, int64_t key){
//header, parent pin check
	int i, j, split;
	int64_t k_prime;
	header_page header;
	page old_page, new_page, child;
	pageinfo tmp[249];
	off_t tmpoffset;
	off_t newpg_ofset;
	int h, p, n, c;

	h = readpage(table_id,0,0);
	header = buffer_array[h].frame.header;

	p = readpage(table_id,prnt_ofset,0);
	old_page = buffer_array[p].frame.normal_page;

    /* First create a temporary set of keys and pointers
     * to hold everything in order, including
     * the new key and pointer, inserted in their
     * correct places. 
     * Then create a new node and copy half of the 
     * keys and pointers to the old node and
     * the other half to the new.
     */

	for (i = 0, j = 0; i < (old_page.header).key_number; i++, j++) {
	    	if (j == left_index + 1) j++;
	    	tmp[j].key = (old_page.data).pageinfos[i].key;
		tmp[j].offset = (old_page.data).pageinfos[i].offset;
	} 

	tmp[left_index+1].key = key;
	tmp[left_index+1].offset = right;
	if(left_index == -1) tmpoffset= left;
	else{
		tmp[left_index].offset =left;
		tmpoffset = old_page.more_offset;
	}
    /* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */  
	split = cut(248);
	
	if(header.freep_offset==0)
		makefrpage(table_id, 0);

	header = buffer_array[h].frame.header;

	newpg_ofset=header.freep_offset;

	n = readpage(table_id,newpg_ofset,1); //new_page pin check
	new_page = buffer_array[n].frame.normal_page;

	header.freep_offset=(new_page.header).offsets;
	
	buffer_array[h].frame.header = header;
	buffer_array[h].is_dirty = 1;
	buffer_array[h].ref_bit =1;

	(new_page.header).isleaf = 0;
	(new_page.header).offsets = (old_page.header).offsets;
	(old_page.header).key_number = 0;
	(new_page.header).key_number = 0;

	old_page.more_offset = tmpoffset;
	for (i = 0; i < split; i++) {
        	(old_page.data).pageinfos[i].key = tmp[i].key;
		(old_page.data).pageinfos[i].offset = tmp[i].offset;
       	(old_page.header).key_number++;
	}

	(new_page).more_offset = tmp[split].offset;
	k_prime = tmp[split].key;

	for (++i, j = 0; i < 249; i++, j++) {
	    	(new_page.data).pageinfos[j].key = tmp[i].key;
		(new_page.data).pageinfos[j].offset = tmp[i].offset;
	    	(new_page.header).key_number++;
	}
	(new_page.header).offsets = (old_page.header).offsets; 
	
	c = readpage(table_id,new_page.more_offset, 0);
	child = buffer_array[c].frame.normal_page;

	(child.header).offsets = newpg_ofset;
	
	buffer_array[c].frame.normal_page = child;
	buffer_array[c].is_dirty = 1;
	buffer_array[c].ref_bit =1;

	for (i = 0; i < (new_page.header).key_number; i++) {		

		c = readpage(table_id,(new_page.data).pageinfos[i].offset, 0);
		child = buffer_array[c].frame.normal_page;

		(child.header).offsets = newpg_ofset;
	
		buffer_array[c].frame.normal_page = child;
		buffer_array[c].is_dirty = 1;
		buffer_array[c].ref_bit =1;
    	}

	for (i = (old_page.header).key_number; i < 248; i++){
	    	(old_page.data).pageinfos[i].key = (int64_t)NULL;
		(old_page.data).pageinfos[i].offset = (off_t)NULL;
	}
	for (i = (new_page.header).key_number; i < 248; i++){
	    	(new_page.data).pageinfos[i].key = (int64_t)NULL;
		(new_page.data).pageinfos[i].offset = (off_t)NULL;
	}

    /* Insert a new key into the parent of the two
     * nodes resulting from the split, with
     * the old node to the left and the new to the right.
     */
	buffer_array[p].frame.normal_page = old_page;
	buffer_array[p].is_dirty = 1;
	buffer_array[p].ref_bit =1;

	buffer_array[n].frame.normal_page = new_page;
	buffer_array[n].is_dirty = 1;
	buffer_array[n].ref_bit =1;

	insert_into_parent(table_id, prnt_ofset, newpg_ofset, k_prime);
}


/*
 * delete
 */

int delete(int table_id, int64_t key){

    off_t leaf_ofset;

    leaf_ofset = find_leaf(table_id, key);
    if (leaf_ofset != 0 && find(table_id, key) != NULL){
    		delete_entry(table_id, leaf_ofset, key);
		return 0;
    }
    return -1;
}

void delete_entry(int table_id, off_t offset, int64_t key){

	int min_keys, capacity, neighbor_index, k_prime_index;
	int64_t k_prime;
	off_t neighbor_ofset;
	header_page header;
	page leaf, parent, neighbor;
	int h, l, n;

	h = readpage(table_id, 0, 1); //header pin check
	header = buffer_array[h].frame.header;

    	remove_entry_from_node(table_id, offset, key);

    /* Case:  deletion from the root. 
     */

    	header = buffer_array[h].frame.header;

    	if (offset == header.rootp_offset) adjust_root(table_id);
	else{
		if(buffer_array[h].pin_count>0) buffer_array[h].pin_count--;

		l = readpage(table_id, offset, 1); // leaf pin check
		leaf = buffer_array[l].frame.normal_page;

		parent = buffer_array[readpage(table_id, leaf.header.offsets, 0)].frame.normal_page;
				
		min_keys = (leaf.header).isleaf ? cut(31) : cut(248);

    /* Case:  node stays at or above minimum.
     * (The simple case.)
     */
		if ((leaf.header).key_number < min_keys){

    /* Case:  node falls below minimum.
     * Either coalescence or redistribution
     * is needed.
     */

    /* Find the appropriate neighbor node with which
     * to coalesce.
     * Also find the key (k_prime) in the parent
     * between the pointer to node n and the pointer
     * to the neighbor.
     */
			neighbor_index = get_neighbor_index(table_id, offset);

			k_prime_index = neighbor_index == -2 ? 0 : neighbor_index; 

			if(neighbor_index==-1) neighbor_ofset = parent.more_offset;
			else neighbor_ofset = parent.data.pageinfos[k_prime_index].offset;

			if(neighbor_index == -2)
				k_prime = parent.data.pageinfos[k_prime_index].key;
			else k_prime = parent.data.pageinfos[k_prime_index + 1].key;

			n = readpage(table_id,neighbor_ofset, 1); //neighbor pin check
			neighbor = buffer_array[n].frame.normal_page;

			capacity = (leaf.header).isleaf ? 31 : 247;
	
    /* Coalescence. */
			if ((neighbor.header).key_number + (leaf.header).key_number <= capacity)
				coalesce_nodes(table_id, neighbor_ofset, offset, neighbor_index, k_prime); // neighbor , offset, header pin

    /* Redistribution. */

			else redistribute_nodes(table_id, neighbor_ofset, offset, neighbor_index, k_prime, k_prime_index);
		}
		else{
			if(buffer_array[l].pin_count>0) buffer_array[l].pin_count--;
		}
	}
}

void remove_entry_from_node(int table_id, off_t offset, int64_t key){

	int i = 0;
	page leaf;
	int l;
	
	l = readpage(table_id,offset,1);
	leaf = buffer_array[l].frame.normal_page;

	if((leaf.header).isleaf){
		while (leaf.data.records[i].key != key) i++;
		for (++i; i < (leaf.header).key_number; i++){
		    	(leaf.data).records[i-1].key = (leaf.data).records[i].key;
			strcpy((leaf.data).records[i-1].value , (leaf.data).records[i].value);
		}
	}else{
		while(leaf.data.pageinfos[i].key != key) i++;

		makefrpage(table_id, leaf.data.pageinfos[i].offset);
			
		for (++i; i < (leaf.header).key_number; i++){
		    	(leaf.data).pageinfos[i-1].key = (leaf.data).pageinfos[i].key;
			(leaf.data).pageinfos[i-1].offset = (leaf.data).pageinfos[i].offset;
		}
	}

	(leaf.header).key_number--;

	char resets[120] = {};
	if ((leaf.header).isleaf){
        	for (i = (leaf.header).key_number; i < 31; i++){
			leaf.data.records[i].key = (int64_t)NULL;
			strcpy((leaf.data).records[i].value,resets);
		}
	}else{
        	for (i = (leaf.header).key_number; i < 248; i++){
            	leaf.data.pageinfos[i].key = (int64_t)NULL;
			leaf.data.pageinfos[i].offset = (off_t)NULL;
		}
	}

	buffer_array[l].frame.normal_page = leaf;
	buffer_array[l].is_dirty = 1;
	buffer_array[l].ref_bit = 1;
	if(buffer_array[l].pin_count>0) buffer_array[l].pin_count--;
}



void adjust_root(int table_id) {

    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */

	header_page header;
	page root;
	off_t root_ofset;
	page head_left;
	int h, c, r;

	h = readpage(table_id, 0, 0);
	header = buffer_array[h].frame.header;

	root_ofset=header.rootp_offset;
	r = readpage(table_id, root_ofset, 1);
	root = buffer_array[r].frame.normal_page;

	if ((root.header).key_number <= 0){

		if (!(root.header).isleaf) {

			c = readpage(table_id,root.more_offset,0);
			head_left = buffer_array[c].frame.normal_page;

        		header.rootp_offset = root.more_offset;

        		(head_left.header).offsets = 0;
			
			buffer_array[c].frame.normal_page = head_left;
			buffer_array[c].is_dirty = 1;
			buffer_array[c].ref_bit =1;
    		}
	    	else header.rootp_offset = 0;

		buffer_array[h].frame.header = header;
		buffer_array[h].is_dirty = 1;
		buffer_array[h].ref_bit = 1;

		makefrpage(table_id, root_ofset);
	}
	if(buffer_array[r].pin_count>0) buffer_array[r].pin_count--;
	if(buffer_array[h].pin_count>0) buffer_array[h].pin_count--;
}


int get_neighbor_index(int table_id, off_t offset) {

	int i;
	page pages;
	page parent;
	int l;
	l = readpage(table_id, offset, 0);
	pages = buffer_array[l].frame.normal_page;
	parent = buffer_array[readpage(table_id, pages.header.offsets,0)].frame.normal_page;

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.  
     * If n is the leftmost child, this means
     * return -1.
     */
	if(parent.more_offset==offset) return -2;
	for (i = 0; i < (parent.header).key_number; i++){
       		if ((parent.data).pageinfos[i].offset == offset) return i - 1;
	}

	if(buffer_array[l].pin_count>0) buffer_array[l].pin_count--;

	exit(EXIT_FAILURE);
}


void coalesce_nodes(int table_id, off_t neighbor_offset, off_t offset, int neighbor_index, int64_t k_prime) {
//neighbor_offset , offset pin check

	int i, j, neighbor_insertion_index, n_end;
    	page neighbor, right, tempt_page;
	off_t tmp;
	int n, r, t;

    /* Swap neighbor with node if node is on the
     * extreme left and neighbor is to its right.
     */
	if (neighbor_index == -2) {
		tmp = neighbor_offset;
		neighbor_offset = offset;
		offset = tmp;
	}
	n = readpage(table_id,neighbor_offset, 0);
	neighbor = buffer_array[n].frame.normal_page;

	r = readpage(table_id, offset, 0);
	right = buffer_array[r].frame.normal_page;

    /* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */

	neighbor_insertion_index = neighbor.header.key_number;

    /* Case:  nonleaf node.
     * Append k_prime and the following pointer.
     * Append all pointers and keys from the neighbor.
     */

	if (!right.header.isleaf){

        /* Append k_prime.
         */
        	neighbor.data.pageinfos[neighbor_insertion_index].key = k_prime;
		neighbor.data.pageinfos[neighbor_insertion_index].offset = right.more_offset;
        	neighbor.header.key_number++;
		
		t = readpage(table_id, right.more_offset, 0);
		tempt_page = buffer_array[t].frame.normal_page;
		(tempt_page.header).offsets= neighbor_offset;
		buffer_array[t].frame.normal_page = tempt_page;
		buffer_array[t].is_dirty =1;
		buffer_array[t].ref_bit =0;

        	n_end = (right.header).key_number;
	
		for (j = 0; j < n_end; j++) {
        	    	
			t = readpage(table_id, right.data.pageinfos[j].offset, 0);
			tempt_page = buffer_array[t].frame.normal_page;
			(tempt_page.header).offsets= neighbor_offset;
			buffer_array[t].frame.normal_page = tempt_page;
			buffer_array[t].is_dirty =1;
			buffer_array[t].ref_bit =0;
        	}

        	for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            	neighbor.data.pageinfos[i].key = right.data.pageinfos[j].key;
            	neighbor.data.pageinfos[i].offset = right.data.pageinfos[j].offset;
            	neighbor.header.key_number++;
            	right.header.key_number--;
        	}

		buffer_array[r].frame.normal_page = right;
		buffer_array[r].is_dirty = 1;
		buffer_array[r].ref_bit = 1;
		if(buffer_array[r].pin_count>0) buffer_array[r].pin_count--;
	}

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

	else {
		if(buffer_array[r].pin_count>0) buffer_array[r].pin_count--;
	    for (i = neighbor_insertion_index, j = 0; j < (right.header).key_number; i++, j++) {
	         neighbor.data.records[i].key = right.data.records[j].key;
	        	strcpy(neighbor.data.records[i].value, right.data.records[j].value);
	         neighbor.header.key_number++;
	    	}
        	neighbor.more_offset = right.more_offset;
	}

	buffer_array[n].frame.normal_page = neighbor;
	buffer_array[n].is_dirty = 1;
	buffer_array[n].ref_bit = 1;
	if(buffer_array[n].pin_count>0) buffer_array[n].pin_count--;

	delete_entry(table_id, (neighbor.header).offsets, k_prime);
}

void redistribute_nodes(int table_id, off_t neighbor_offset, off_t offset, int neighbor_index, int64_t k_prime, int k_prime_index) {  
// offset, neighbor parent pin count
	int i;
	page neighbor, pages, parent, child;
	int n, l, p, c;
    /* Case: n has a neighbor to the left. 
     * Pull the neighbor's last key-pointer pair over
     * from the neighbor's right end to n's left end.
     */

	n = readpage(table_id, neighbor_offset, 0);
	neighbor = buffer_array[n].frame.normal_page;

	l = readpage(table_id, offset, 0);
	pages = buffer_array[l].frame.normal_page;

	p = readpage(table_id, neighbor.header.offsets, 1);
	parent= buffer_array[p].frame.normal_page;

	if (neighbor_index != -2){

        	if (!(pages.header).isleaf){
			for (i = pages.header.key_number; i > 0; i--) {
            		pages.data.pageinfos[i].key = pages.data.pageinfos[i-1].key;
            		pages.data.pageinfos[i].offset = pages.data.pageinfos[i-1].offset;
        		}
			pages.data.pageinfos[0].offset = pages.more_offset;
			pages.data.pageinfos[0].key = parent.data.pageinfos[k_prime_index + 1].key;

			pages.more_offset = neighbor.data.pageinfos[neighbor.header.key_number-1].offset;
			parent.data.pageinfos[k_prime_index + 1].key = neighbor.data.pageinfos[neighbor.header.key_number-1].key;
			
			neighbor.data.pageinfos[neighbor.header.key_number-1].key = (int64_t)NULL;
			neighbor.data.pageinfos[neighbor.header.key_number-1].offset = (off_t)NULL;

			c= readpage(table_id, pages.more_offset, 0);
			child = buffer_array[c].frame.normal_page;
			child.header.offsets = offset;

			buffer_array[c].frame.normal_page = child;
			buffer_array[c].is_dirty = 1;
			buffer_array[c].ref_bit = 1;
        	}
        	else {
		 	for (i = pages.header.key_number; i > 0; i--) {
            		pages.data.records[i].key=pages.data.records[i-1].key;
				strcpy(pages.data.records[i].value, pages.data.records[i-1].value);
        		}
			pages.data.records[0].key = neighbor.data.records[neighbor.header.key_number - 1].key;
			strcpy(pages.data.records[0].value, neighbor.data.records[neighbor.header.key_number - 1].value);
			char reset[120] = {};
			neighbor.data.records[neighbor.header.key_number - 1].key = (int64_t)NULL;
			strcpy(neighbor.data.records[neighbor.header.key_number - 1].value, reset);
			parent.data.pageinfos[k_prime_index+1].key = pages.data.records[0].key;
        	}
    	}

    /* Case: n is the leftmost child.
     * Take a key-pointer pair from the neighbor to the right.
     * Move the neighbor's leftmost key-pointer pair
     * to n's rightmost position.
     */

    	else {  
        	if (pages.header.isleaf){
            		pages.data.records[pages.header.key_number].key = neighbor.data.records[0].key;
            		strcpy(pages.data.records[pages.header.key_number].value, neighbor.data.records[0].value);
            		parent.data.pageinfos[k_prime_index].key = neighbor.data.records[1].key;

			for (i = 0; i < neighbor.header.key_number - 1; i++) {
 	       		neighbor.data.records[i].key = neighbor.data.records[i+1].key;
 	       		strcpy(neighbor.data.records[i].value, neighbor.data.records[i+1].value);
 		    	}
			char reset[120] = {};
			neighbor.data.records[neighbor.header.key_number -1].key = (int64_t)NULL;
 	         strcpy(neighbor.data.records[neighbor.header.key_number-1].value, reset);
        	}
        	else {
			pages.data.pageinfos[pages.header.key_number].key = parent.data.pageinfos[k_prime_index].key;
			pages.data.pageinfos[pages.header.key_number].offset = neighbor.more_offset;
			parent.data.pageinfos[k_prime_index].key = neighbor.data.pageinfos[0].key;
			neighbor.more_offset = neighbor.data.pageinfos[0].offset;		

			c = readpage(table_id, pages.data.pageinfos[pages.header.key_number].offset, 0);
			child = buffer_array[c].frame.normal_page;
			child.header.offsets = offset;
			
			buffer_array[c].frame.normal_page = child;
			buffer_array[c].is_dirty = 1;
			buffer_array[c].ref_bit = 1;
			
			for (i = 0; i < neighbor.header.key_number - 1; i++) {
 	       		neighbor.data.pageinfos[i].key = neighbor.data.pageinfos[i+1].key;
 	       		neighbor.data.pageinfos[i].offset = neighbor.data.pageinfos[i+1].offset;
 		    	}

			neighbor.data.pageinfos[neighbor.header.key_number - 1].key = (int64_t)NULL;
 	         neighbor.data.pageinfos[neighbor.header.key_number - 1].offset = (off_t)NULL;
        	}
    	}

    /* n now has one more key and one more pointer;
     * the neighbor has one fewer of each.
     */
	pages.header.key_number++;
	neighbor.header.key_number--;

	buffer_array[l].frame.normal_page = pages;
	buffer_array[l].is_dirty = 1;
	buffer_array[l].ref_bit = 1;
	if(buffer_array[l].pin_count>0) buffer_array[l].pin_count--;

	buffer_array[n].frame.normal_page = neighbor;
	buffer_array[n].is_dirty = 1;
	buffer_array[n].ref_bit = 1;
	if(buffer_array[n].pin_count>0) buffer_array[n].pin_count--;

	buffer_array[p].frame.normal_page = parent;
	buffer_array[p].is_dirty = 1;
	buffer_array[p].ref_bit = 1;
	if(buffer_array[p].pin_count>0) buffer_array[p].pin_count--;
}