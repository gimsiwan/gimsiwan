#define PAGESIZE 4096
#include "bpt.h"

int fd = -1;
char copy[120]={};

/*
 * open
 */

int open_db (char *pathname){
	
	if((fd = open(pathname,O_RDWR|O_CREAT|O_EXCL|O_SYNC,0777))>0){
		
		header_page *header;

		header=(header_page *)calloc(1,PAGESIZE);
		header->rootp_offset = 0;
		header->freep_offset = 0;
		header->page_number = 1;
		
		if(pwrite(fd,header,PAGESIZE,SEEK_SET)<0){
			perror("write error");
			return -1;
		}
		free(header);
	return 0;
	}
	
	else if((fd = open(pathname,O_RDWR|O_SYNC))>0) return 0;
	else return -1;
}


/*
 * find
 */

off_t find_leaf(int64_t key) {

	off_t ofsnum;
	int i = 0;    
	header_page header;
	page c;

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	if (header.rootp_offset == 0) {
        return 0;
	}
    
	if(pread(fd,&c,PAGESIZE,SEEK_SET+header.rootp_offset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	ofsnum = header.rootp_offset;

	while (!(c.header).isleaf) {
		i = 0;

	     while (i < (c.header).key_number) {
			if (key >= ((c.data).pageinfos[i]).key) i++;
	        	else break;
        	}

		if(i==0) ofsnum = c.more_offset;
		else ofsnum=((c.data).pageinfos[i-1]).offset;
	
		if(pread(fd, &c, PAGESIZE, SEEK_SET + ofsnum)<0){
			perror("read error");
			exit(EXIT_FAILURE);
		}
	}
    	return ofsnum;
}

char * find(int64_t key) {

    int i = 0;
    off_t offsnum = find_leaf(key);
    page c;

    if (offsnum == 0) return NULL;
    else{
		if(pread(fd, &c, PAGESIZE, SEEK_SET + offsnum)<0){
			perror("read error");
			exit(EXIT_FAILURE);
		}

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

int get_left_index(off_t prnt_offset, int64_t key) {

	int left_index = 0;
	page parent;
	if(pread(fd,&parent,PAGESIZE,SEEK_SET+prnt_offset)<0){
		perror("read error");
 		exit(EXIT_FAILURE);
	}
	if (key < (parent.data).pageinfos[0].key)
		return -1;

	while (left_index < (parent.header).key_number - 1 && (parent.data).pageinfos[left_index+1].key <= key)
		left_index++;
    return left_index;
}

record * make_record(int64_t key, char* value){
	
	record * new_record;

	new_record=(record*)malloc(sizeof(record));

    	new_record->key = key;
	strcpy(new_record->value,value);
    	
	return new_record;
}


int insert(int64_t key, char* value) {

	record * pointer;
	page leaf;
	off_t leaf_ofset;
	header_page header;
    
    /* The current implementation ignores
     * duplicates.
     */

	if (find(key) != NULL)
		return -1;

    /* Create a new record for the
     * value.
     */
	pointer = make_record(key, value);
	
	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

	if (header.rootp_offset == 0){
		start_new_tree(pointer);
		free(pointer);
		return 0;
	}

    /* Case: the tree already exists.
     * (Rest of function body.)
     */
	
	leaf_ofset = find_leaf(key);

	if(pread(fd,&leaf,PAGESIZE,SEEK_SET+leaf_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

    /* Case: leaf has room for key and pointer.
     */

	if ((leaf.header).key_number < 31) {
		insert_into_leaf(leaf_ofset, pointer);
		free(pointer);
	    	return 0;
	}
    /* Case:  leaf must be split.
     */
	insert_into_leaf_after_splitting(leaf_ofset, pointer);
	free(pointer);

	return 0;
}



void start_new_tree(record* pointer) {

	header_page header;
	page root;

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
    	}	

	if(header.freep_offset==0)
		makefrpage(0);

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
    	}
    
	off_t offset = header.freep_offset;

	if(pread(fd,&root,PAGESIZE,SEEK_SET+offset)<0){
		perror("read error");
	    	exit(EXIT_FAILURE);
	}

	header.freep_offset=(root.header).offsets;
	header.rootp_offset=offset;
	(root.header).offsets=0;
	(root.header).isleaf=1;
	(root.header).key_number=1;
	root.more_offset=0;
	(root.data).records[0].key=pointer->key;
	strcpy((root.data).records[0].value, pointer->value);

	if(pwrite(fd,&root,PAGESIZE,SEEK_SET+offset)<0){
		perror("write error");
        	exit(EXIT_FAILURE);
   	}
	if(pwrite(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}

	if(header.freep_offset==0)
		makefrpage(0);
}


/* 해당 offset의 page를 free page로 만들어주기
 * 만약 offset이 0이라면 파일의 맨끝에 frpage만들기
 */
void makefrpage(off_t offset){

	off_t oftnum = offset;

	header_page header;
	page * freepage;
	freepage=(page*)calloc(1, PAGESIZE);

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
    	}	

	if(oftnum==0){
		oftnum = (header.page_number)*PAGESIZE;
		(header.page_number)++;
	}

	if(header.freep_offset==0) header.freep_offset = oftnum;
	else{
		off_t frpg_ofset = header.freep_offset;
		header.freep_offset = oftnum;
		(freepage->header).offsets =frpg_ofset;
	}
	if(pwrite(fd,freepage,PAGESIZE,SEEK_SET+oftnum)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}

	if(pwrite(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}
	free(freepage);
}


void insert_into_leaf(off_t leaf_ofset, record *pointer) {
	
	int i, insertion_point;
	page leaf;
	
	if(pread(fd,&leaf,PAGESIZE,SEEK_SET+leaf_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}
	
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

	if(pwrite(fd,&leaf,PAGESIZE,SEEK_SET+leaf_ofset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}

}

void insert_into_parent(off_t left_ofset, off_t right_ofset, int64_t new_key ) {

	off_t prnt_ofset;
	page parent;
	page left;
	int left_index;

	if(pread(fd,&left,PAGESIZE,SEEK_SET+left_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	prnt_ofset = (left.header).offsets;

    /* Case: new root. */

	if (prnt_ofset == 0)
	    	insert_into_new_root(left_ofset, new_key, right_ofset);

    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */
	else{
		if(pread(fd,&parent,PAGESIZE,SEEK_SET+prnt_ofset)<0){
			perror("read error");
			exit(EXIT_FAILURE);
		}

		left_index = get_left_index(prnt_ofset, (left.data).records[0].key);

    /* Simple case: the new key fits into the node. 
     */
	 	if ((parent.header).key_number < 248)
		    	insert_into_node(prnt_ofset, left_ofset, right_ofset, new_key, left_index);

    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */
		else insert_into_node_after_splitting(prnt_ofset, left_ofset, right_ofset, left_index, new_key);	
	}
}



void insert_into_leaf_after_splitting(off_t leaf_ofset, record * pointer) {

	page leaf;
	page new_page;
	off_t newpg_ofset;
	header_page header;
	record tmp[32];
	int insertion_index, split, i, j;
	int64_t new_key;

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	if(pread(fd,&leaf,PAGESIZE,SEEK_SET+leaf_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}
	

	if(header.freep_offset==0)
		makefrpage(0);

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	newpg_ofset=header.freep_offset;

	if(pread(fd,&new_page,PAGESIZE,SEEK_SET+newpg_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}
	
	header.freep_offset=(new_page.header).offsets;

	if(pwrite(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}
	
	
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
 
	if(pwrite(fd,&leaf,PAGESIZE,SEEK_SET+leaf_ofset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	};
	if(pwrite(fd,&new_page,PAGESIZE,SEEK_SET+newpg_ofset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	};
	insert_into_parent(leaf_ofset, newpg_ofset, new_key );
}


void insert_into_new_root(off_t left, int64_t key, off_t right) {

	header_page header;
	page root;
	page leftp;
	page rightp;
	off_t offset;

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}
	if(pread(fd,&leftp,PAGESIZE,SEEK_SET+left)<0){
		perror("read error");
	        exit(EXIT_FAILURE);
	}
	if(pread(fd,&rightp,PAGESIZE,SEEK_SET+right)<0){
		perror("read error");
	        exit(EXIT_FAILURE);
	}

	if(header.freep_offset==0)
		makefrpage(0);

    	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	offset = header.freep_offset;

	if(pread(fd,&root,PAGESIZE,SEEK_SET+offset)<0){
		perror("read error");
	        exit(EXIT_FAILURE);
	}
	
	
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

	if(pwrite(fd,&root,PAGESIZE,SEEK_SET+offset)<0){
		perror("write error");
        	exit(EXIT_FAILURE);
	}
	if(pwrite(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}
	if(pwrite(fd,&leftp,PAGESIZE,SEEK_SET+left)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}
	if(pwrite(fd,&rightp,PAGESIZE,SEEK_SET+right)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}
	
}


void insert_into_node(off_t prnt_ofset, off_t left_ofset, off_t right_ofset, int64_t key, int left_index) {

	int i;
	page parent;
	page right_page;

	if(pread(fd,&parent,PAGESIZE,SEEK_SET+prnt_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}
	if(pread(fd,&right_page,PAGESIZE,SEEK_SET+right_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

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
	

	if(pwrite(fd,&parent,PAGESIZE,SEEK_SET+prnt_ofset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}

	if(pwrite(fd,&right_page,PAGESIZE,SEEK_SET+right_ofset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}

}



void insert_into_node_after_splitting(off_t prnt_ofset, off_t left, off_t right, int left_index, int64_t key) {
	
	int i, j, split;
	int64_t k_prime;
	header_page header;
	page old_page, new_page, child;
	pageinfo tmp[249];
	off_t tmpoffset;
	off_t newpg_ofset;


	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	if(pread(fd,&old_page,PAGESIZE,SEEK_SET+prnt_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

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
	};
    /* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */  
	split = cut(248);
	
	if(header.freep_offset==0)
		makefrpage(0);

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	newpg_ofset=header.freep_offset;

	if(pread(fd,&new_page,PAGESIZE,SEEK_SET+newpg_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	header.freep_offset=(new_page.header).offsets;
	
	if(pwrite(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}

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

	pread(fd, &child, PAGESIZE, SEEK_SET + new_page.more_offset);
	(child.header).offsets = newpg_ofset;
	pwrite(fd, &child, PAGESIZE, SEEK_SET + new_page.more_offset);
	for (i = 0; i < (new_page.header).key_number; i++) {		

		pread(fd, &child, PAGESIZE, SEEK_SET + (new_page.data).pageinfos[i].offset);
		(child.header).offsets = newpg_ofset;
	    	pwrite(fd, &child, PAGESIZE, SEEK_SET + (new_page.data).pageinfos[i].offset);
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

	if(pwrite(fd,&old_page,PAGESIZE,SEEK_SET+prnt_ofset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}
	if(pwrite(fd,&new_page,PAGESIZE,SEEK_SET+newpg_ofset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}

	insert_into_parent(prnt_ofset, newpg_ofset, k_prime);
}



/*
 * delete
 */

int delete(int64_t key) {

    off_t leaf_ofset;

    leaf_ofset = find_leaf(key);
    if ( leaf_ofset != 0 && find(key) != NULL) {
        delete_entry(leaf_ofset, key);
	return 0;
    }
    return -1;
}

void delete_entry(off_t offset, int64_t key) {


	int min_keys;
   	int capacity;
	int neighbor_index;
    	int k_prime_index;
	int64_t k_prime;
	off_t neighbor_ofset;
	header_page header;
	page leaf;
	page parent;
	page neighbor;

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

    	remove_entry_from_node(offset, key);

    /* Case:  deletion from the root. 
     */
	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

    	if (offset == header.rootp_offset)
        	adjust_root();
	else{

		if(pread(fd,&leaf,PAGESIZE,SEEK_SET+offset)<0){
			perror("read error");
			exit(EXIT_FAILURE);
		}
		if(pread(fd,&parent,PAGESIZE,SEEK_SET+(leaf.header).offsets)<0){
			perror("read error");
			exit(EXIT_FAILURE);
		}
				

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
			neighbor_index = get_neighbor_index(offset);

			k_prime_index = neighbor_index == -2 ? 0 : neighbor_index; 

			if(neighbor_index==-1) neighbor_ofset = parent.more_offset;
			else neighbor_ofset = parent.data.pageinfos[k_prime_index].offset;

			if(neighbor_index == -2)
				k_prime = parent.data.pageinfos[k_prime_index].key;
			else k_prime = parent.data.pageinfos[k_prime_index + 1].key;

			if(pread(fd,&neighbor,PAGESIZE,SEEK_SET+neighbor_ofset)<0){
				perror("read error");
				exit(EXIT_FAILURE);
			}

			capacity = (leaf.header).isleaf ? 31 : 247;
	
    /* Coalescence. */


			if ((neighbor.header).key_number + (leaf.header).key_number <= capacity)
				coalesce_nodes(neighbor_ofset, offset, neighbor_index, k_prime);

    /* Redistribution. */

			else redistribute_nodes(neighbor_ofset, offset, neighbor_index, k_prime, k_prime_index);
		}
	}
}

void remove_entry_from_node(off_t offset, int64_t key) {

	int i;
	page leaf;
	
	if(pread(fd,&leaf,PAGESIZE,SEEK_SET+offset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	i = 0;
	if((leaf.header).isleaf){
		while (leaf.data.records[i].key != key)
        		i++;
		for (++i; i < (leaf.header).key_number; i++){
		   	(leaf.data).records[i-1].key = (leaf.data).records[i].key;
			strcpy((leaf.data).records[i-1].value , (leaf.data).records[i].value);
		}
	}else{
		
		while (leaf.data.pageinfos[i].key != key)
        		i++;

		makefrpage(leaf.data.pageinfos[i].offset);
			
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

	if(pwrite(fd,&leaf,PAGESIZE,SEEK_SET+offset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}
}



void adjust_root(void) {

    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */

	header_page header;
	page root;
	off_t root_ofset;
	page head_left;

	if(pread(fd,&header,PAGESIZE,SEEK_SET)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	root_ofset=header.rootp_offset;
	
	if(pread(fd,&root,PAGESIZE,SEEK_SET+root_ofset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	if ((root.header).key_number <= 0){ 

		if (!(root.header).isleaf) {


			if(pread(fd,&head_left,PAGESIZE,SEEK_SET+root.more_offset)<0){
				perror("read error");
				exit(EXIT_FAILURE);
			}

        		header.rootp_offset = root.more_offset;

        		(head_left.header).offsets = 0;
			
			if(pwrite(fd,&head_left,PAGESIZE,SEEK_SET+root.more_offset)<0){
				perror("write error");
				exit(EXIT_FAILURE);
			}
    		}

	    	else header.rootp_offset = 0;

	    	if(pwrite(fd,&header,PAGESIZE,SEEK_SET)<0){
			perror("write error");
			exit(EXIT_FAILURE);
		}

		makefrpage(root_ofset);

	}
}


int get_neighbor_index(off_t offset) {

	int i;
	page pages;
	page parent;

	if(pread(fd,&pages,PAGESIZE,SEEK_SET+offset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	if(pread(fd,&parent,PAGESIZE,SEEK_SET+(pages.header).offsets)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

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

	exit(EXIT_FAILURE);
}


void coalesce_nodes(off_t neighbor_offset, off_t offset, int neighbor_index, int64_t k_prime) {

	
	
	int i, j, neighbor_insertion_index, n_end;
    	page neighbor;
	page right;
	page tempt_page;
	off_t tmp;

    /* Swap neighbor with node if node is on the
     * extreme left and neighbor is to its right.
     */

	if (neighbor_index == -2) {
	        tmp = neighbor_offset;
		neighbor_offset = offset;
		offset = tmp;
	}

	if(pread(fd,&neighbor,PAGESIZE,SEEK_SET+neighbor_offset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

	if(pread(fd,&right,PAGESIZE,SEEK_SET+offset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}

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

	if (!right.header.isleaf) {

        /* Append k_prime.
         */

        	neighbor.data.pageinfos[neighbor_insertion_index].key = k_prime;
		neighbor.data.pageinfos[neighbor_insertion_index].offset = right.more_offset;
        	neighbor.header.key_number++;
		

		if(pread(fd,&tempt_page,PAGESIZE,SEEK_SET+right.more_offset)<0){
			perror("read error");
			exit(EXIT_FAILURE);
		}
		(tempt_page.header).offsets= neighbor_offset;
		if(pwrite(fd,&tempt_page,PAGESIZE,SEEK_SET+right.more_offset)<0){
			perror("write error");
			exit(EXIT_FAILURE);
		}   

        	n_end = (right.header).key_number;
	
		for (j = 0; j < n_end; j++) {
        	    	
            	if(pread(fd,&tempt_page,PAGESIZE,SEEK_SET+right.data.pageinfos[j].offset)<0){
				perror("read error");
				exit(EXIT_FAILURE);
			}
			(tempt_page.header).offsets= neighbor_offset;
			if(pwrite(fd,&tempt_page,PAGESIZE,SEEK_SET+right.data.pageinfos[j].offset)<0){
				perror("write error");
				exit(EXIT_FAILURE);
			}            
            
        	}

        	for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            	neighbor.data.pageinfos[i].key = right.data.pageinfos[j].key;
            	neighbor.data.pageinfos[i].offset = right.data.pageinfos[j].offset;
            	neighbor.header.key_number++;
            	right.header.key_number--;
        	}

		if(pwrite(fd,&neighbor,PAGESIZE,SEEK_SET+neighbor_offset)<0){
			perror("write error");
			exit(EXIT_FAILURE);
		}

		if(pwrite(fd,&right,PAGESIZE,SEEK_SET+offset)<0){
			perror("write error");
			exit(EXIT_FAILURE);
		}
	
	}

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

	else {
	        for (i = neighbor_insertion_index, j = 0; j < (right.header).key_number; i++, j++) {
	            neighbor.data.records[i].key = right.data.records[j].key;
	            strcpy(neighbor.data.records[i].value, right.data.records[j].value);
	            neighbor.header.key_number++;
	        }

        	neighbor.more_offset = right.more_offset;

		if(pwrite(fd,&neighbor,PAGESIZE,SEEK_SET+neighbor_offset)<0){
			perror("write error");
			exit(EXIT_FAILURE);
		}

	}

	delete_entry((neighbor.header).offsets, k_prime);
}


void redistribute_nodes(off_t neighbor_offset, off_t offset, int neighbor_index, int64_t k_prime, int k_prime_index) {  

	int i;
	page neighbor;
	page pages;
	page parent;
	page child;
    /* Case: n has a neighbor to the left. 
     * Pull the neighbor's last key-pointer pair over
     * from the neighbor's right end to n's left end.
     */

	if(pread(fd,&neighbor,PAGESIZE,SEEK_SET+neighbor_offset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}
	if(pread(fd,&pages,PAGESIZE,SEEK_SET+offset)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}
	if(pread(fd,&parent,PAGESIZE,SEEK_SET+neighbor.header.offsets)<0){
		perror("read error");
		exit(EXIT_FAILURE);
	}	



	if (neighbor_index != -2) {

        	if (!(pages.header).isleaf) {
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

			if(pread(fd,&child,PAGESIZE,SEEK_SET + pages.more_offset)<0){
				perror("read error");
				exit(EXIT_FAILURE);
			}
			child.header.offsets = offset;

			if(pwrite(fd,&child,PAGESIZE,SEEK_SET + pages.more_offset)<0){
				perror("write error");
				exit(EXIT_FAILURE);
			}
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
        	if (pages.header.isleaf) {
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

			if(pread(fd,&child,PAGESIZE,SEEK_SET+pages.data.pageinfos[pages.header.key_number].offset)<0){
				perror("read error");
				exit(EXIT_FAILURE);
			}
			child.header.offsets = offset;
			
			if(pwrite(fd,&child,PAGESIZE,SEEK_SET+pages.data.pageinfos[pages.header.key_number].offset)<0){
				perror("write error");
				exit(EXIT_FAILURE);
			}
			
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

	if(pwrite(fd,&pages,PAGESIZE,SEEK_SET + offset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}
	if(pwrite(fd,&neighbor,PAGESIZE,SEEK_SET + neighbor_offset)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}
	if(pwrite(fd,&parent,PAGESIZE,SEEK_SET + pages.header.offsets)<0){
		perror("write error");
		exit(EXIT_FAILURE);
	}

}

