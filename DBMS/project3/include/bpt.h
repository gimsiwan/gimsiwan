#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>



typedef struct header_page{
	off_t freep_offset;
	off_t rootp_offset;
	int64_t page_number;
	char reserve[4072];
}header_page;

typedef struct page_header{
	off_t offsets;
	int isleaf;
	int key_number;
	char reserve[104];
}page_header;

typedef struct record{
	int64_t key;
	char value[120];
}record;

typedef struct pageinfo{
	int64_t key;
	off_t offset;
}pageinfo;

typedef union pagedata{
	pageinfo pageinfos[248];
	record records[31];
}pagedata;

typedef struct page{
	page_header header;
	off_t more_offset;
	pagedata data;
}page;

typedef union pagetype{
	page normal_page;
	header_page header;
}pagetype;

typedef struct buffer{
	pagetype frame;
	int table_id;
	off_t page_offset;
	short is_dirty;
	short pin_count;
	short ref_bit;
}buffer;


int init_db (int num_buf);
int readpage(int table_id,off_t ofset, int pin);
int close_table(int table_id);
int shutdown_db(void);

int open_table (char *pathname);

off_t find_leaf(int table, int64_t key);
char * find(int table, int64_t key);

int cut( int length );
int get_left_index(int table_id, off_t prnt_offset, int64_t key);
record * make_record(int64_t key,char* value);
void makefrpage(int table_id, off_t offset);
void start_new_tree(int table_id, record * pointer);
int insert(int table_id, int64_t key, char* value);
void insert_into_leaf(int table_id, off_t leaf_ofset, record * pointer);
void insert_into_leaf_after_splitting(int table_id, off_t leaf_ofset, record * pointer);
void insert_into_parent(int table_id, off_t left_ofset, off_t right_ofset, int64_t new_key);
void insert_into_new_root(int table_id, off_t left, int64_t key, off_t right);
void insert_into_node(int table_id, off_t prnt_ofset, off_t left_ofset, off_t right_ofset, int64_t key, int left_index);
void insert_into_node_after_splitting(int table_id, off_t prnt_ofset, off_t left, off_t right, int left_index, int64_t key);


int delete(int table_id, int64_t key);
void delete_entry(int table_id, off_t offset, int64_t key);
void remove_entry_from_node(int table_id, off_t offset, int64_t key);
void adjust_root(int table_id);
int get_neighbor_index(int table_id, off_t offset);
void coalesce_nodes(int table_id, off_t neighbor_offset, off_t offset, int neighbor_index, int64_t k_prime);
void redistribute_nodes(int table_id, off_t neighbor_offset, off_t offset, int neighbor_index, int64_t k_prime, int k_prime_index);