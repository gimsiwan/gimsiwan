#include "bpt.h"
#include <string.h>
#include <time.h>
#include <stdbool.h>
int main(int argc, char ** argv) {
	int64_t key, temp, t;
	char instruction;
	char value[300];
	off_t testoffset;
	page *tpage;
	bool automate = true;
	char returns[120];
	int count=0;
	header_page header;
	srand(time(NULL));
	if (argc == 1)
		temp = open_db("./mydb");
	else {
		temp = open_db(argv[1]);
	}
	if (automate) {
		bool check[1048576];
		bool nodup;
		int inputnum;
		char c = 'i';
		int temp, toWrite;
		printf("insert num: ");
		scanf("%d", &inputnum);
		memset(check, false, sizeof(check));
		for (int i = inputnum; i >= 1; i--) {
			do {
				nodup = true;
				t = rand() % inputnum;
				if (check[t])
					nodup = false;
			}	while(!nodup);
			check[t] = true;
			// t = i;
			sprintf(value, "%ld", t);
			printf("%ld %s\n", t, value);
			insert(t, value);
		}
	}
	if (automate) {
		bool check[1048576];
		bool nodup;
		int deletenum;
		char c = 'i';
		int temp, toWrite;
		printf("delete num: ");
		scanf("%d", &deletenum);
		memset(check, false, sizeof(check));
		for (int i = deletenum; i >= 1; i--) {
			do {
				nodup = true;
				t = rand() % deletenum;
				if (check[t])
					nodup = false;
			}	while(!nodup);
			check[t] = true;
			// t = i;
			sprintf(value, "%ld", t);
			printf("%ld %s\n", t, value);
			delete(t);
		}
	}
	while(getchar() != '\n');
	while(1) {
		char print[120];
		printf("i / f / d / q\n");
		instruction = fgetc(stdin);
		switch(instruction) {
			case 'i':
				scanf("%ld %s", &key, value);
				while(getchar() != '\n');
				printf("%d\n",insert(key, value));
				printf("key: %ld\n\n", key);
				break;
			case 'f':
				scanf("%ld", &key);
				while(getchar() != '\n');
				if(find(key)==NULL) printf("NULL\n");				
				else printf("%s\n",find(key));
				break;
			case 'd':
				scanf("%ld", &key);
				while(getchar() != '\n');
				printf("%d\n",delete(key));
				printf("key: %ld\n\n", key);
				break;
			case 'q':
				return EXIT_SUCCESS;
			default:
				while(getchar() != '\n');
				printf("No such Instruction\n");
		}
	}
	return EXIT_SUCCESS;
}
