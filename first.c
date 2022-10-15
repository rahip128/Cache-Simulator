#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hits = 0, misses = 0, reads = 0, writes = 0, lines = 1;
int cacheSize = 0, blockSize = 0;
int bBits = 0, setBits = 0, tagBits = 0, setIndex = 0;
unsigned long long tag = 0;
char replacement_policy[] = "    ";

struct L1Cache { 
	unsigned long long** cache;
};

struct L1Cache* createCache() {
	struct L1Cache* c = malloc(sizeof(struct L1Cache));
	c->cache = malloc(sizeof(unsigned long long) * (cacheSize/(blockSize)*lines));
	for(int i = 0; i < ((cacheSize/blockSize)*lines); i++){
		c->cache[i] = malloc(sizeof(unsigned long long) * lines);
	}
	return c;
}

int log2base(int value){
	int count = 0;
	while(value != 1){
		value/=2;
		count++;
	}
	return count;
}

int two2Pow(int value){
	int result = 1;
	for(int i = 0; i < value; i++){
		result *= 2;
	}
	return result;
}

void write(struct L1Cache* c, int associativity){
	if(associativity == 1){
		if(c->cache[setIndex][0] == tag){
			hits++;
			writes++;
		} else {
			c->cache[setIndex][0] = tag;
			misses++;
			writes++;
			reads++;
		}
	} else if (associativity == 2){
		int h = 0;
		for(int i = 0; i < ((cacheSize/blockSize)); i++){
			if(c->cache[i][0] == tag){
				if(strcmp(replacement_policy, "lru") == 0){
					unsigned long long temp = c->cache[i][0];
					for(int j = i; j < ((cacheSize/blockSize)) - 1; j++){
						c->cache[j][0] = c->cache[j+1][0];
					}
					c->cache[((cacheSize/blockSize)) - 1][0] = temp;	
				}
				hits++;
				writes++;
				h = 1;
				break;
			}
		}
		if(h != 1){
			for(int i = 0; i < ((cacheSize/blockSize)) - 1; i++){
				c->cache[i][0] = c->cache[i+1][0];
			}
			c->cache[((cacheSize/blockSize)) - 1][0] = tag;	
			misses++;
			writes++;
			reads++;
		}
	} else if (associativity == 3){
		int h = 0;
		for(int i = 0; i < lines; i++){
			if(c->cache[setIndex][i] == tag){
				if(strcmp(replacement_policy, "lru") == 0){
					unsigned long long temp = c->cache[setIndex][i];
					for(int j = i; j < lines - 1; j++){
						c->cache[setIndex][j] = c->cache[setIndex][j+1];
					}
					c->cache[setIndex][lines - 1] = temp;
				}
				hits++;
				writes++;
				h = 1;
				break;
			}
		}
		if(h != 1){
			for(int i = 0; i < lines - 1; i++){
				c->cache[setIndex][i] = c->cache[setIndex][i+1];
			}
			c->cache[setIndex][lines - 1] = tag;	
			misses++;
			writes++;
			reads++;
		}
	}
}

void read(struct L1Cache* c, int associativity){
	if(associativity == 1){
		if(c->cache[setIndex][0] == tag){
			hits++;
		} else {
			misses++;
			reads++;
			c->cache[setIndex][0] = tag;
		}
	} else if (associativity == 2){
		int h = 0;
		for(int i = 0; i < ((cacheSize/blockSize)); i++){
			if(c->cache[i][0] == tag){
				if(strcmp(replacement_policy, "lru") == 0){
					unsigned long long temp = c->cache[i][0];
					for(int j = i; j < ((cacheSize/blockSize)) - 1; j++){
						c->cache[j][0] = c->cache[j+1][0];
					}
					c->cache[((cacheSize/blockSize)) - 1][0] = temp;	
				}
				hits++;
				h = 1;
				break;
			}
		}
		if(h != 1){
			for(int i = 0; i < (cacheSize/blockSize) - 1; i++){
				c->cache[i][0] = c->cache[i+1][0];
			}
			c->cache[(cacheSize/blockSize) - 1][0] = tag;
			misses++;
			reads++;
		}
	}  else if (associativity == 3){
		int h = 0;
		for(int i = 0; i < lines; i++){
			if(c->cache[setIndex][i] == tag){
				if(strcmp(replacement_policy, "lru") == 0){
					unsigned long long temp = c->cache[setIndex][i];
					for(int j = i; j < lines - 1; j++){
						c->cache[setIndex][j] = c->cache[setIndex][j+1];
					}
					c->cache[setIndex][lines - 1] = temp;
				}
				hits++;
				h = 1;
				break;
			}
		}
		if(h != 1){	
			for(int i = 0; i < lines - 1; i++){
				c->cache[setIndex][i] = c->cache[setIndex][i+1];
			}
			c->cache[setIndex][lines - 1] = tag;
			misses++;
			reads++;
		}
	}
}

void freeCache(struct L1Cache* c){
	for(int i = 0; i < ((cacheSize/blockSize)*lines); i++){
		free(c->cache[i]);
	}
	free(c->cache);
	free(c);
}

int main(int argc, char* argv[argc+1]){
	int associativity = -1;
	if (argc != 6){
		printf("error\n");
		return EXIT_SUCCESS;
	}

	cacheSize = atoi(argv[1]);
	if(!((cacheSize != 0) && (cacheSize & (cacheSize - 1)) == 0)){
		printf("error\n");
		return EXIT_SUCCESS;
	}
	blockSize = atoi(argv[4]);
	if(!((blockSize != 0) && ((blockSize & (blockSize - 1)) == 0))){
		printf("error\n");
		return EXIT_SUCCESS;
	} else {
		bBits = log2base(blockSize);
	}
	if(strcmp(argv[2], "direct") == 0){
		setBits = log2base(cacheSize/blockSize);
		associativity = 1;
		lines = 1;

	} else if(strcmp(argv[2], "assoc") == 0){
		setBits = log2base(1);
		associativity = 2;
		lines = 1;


	} else {
		char* c = strtok(argv[2], ":");
		c = strtok(NULL, ":");
		if(c == NULL){
			printf("error\n");
			return EXIT_SUCCESS;
		}
		lines = atoi(c);
		setBits = log2base(cacheSize/(blockSize * lines));
		associativity = 3;
	}

	if(strcmp(argv[3], "lru") == 0){
		strcpy(replacement_policy, "lru");

	} else if(strcmp(argv[3], "fifo") == 0){
		strcpy(replacement_policy, "fifo");

	} else {
		printf("error\n");
		return EXIT_SUCCESS;
	}

	FILE* fp = fopen(argv[5], "r");
	if(fp == 0){
		printf("error\n");
		return EXIT_SUCCESS;
	}
	char command;
	unsigned long long address;

	struct L1Cache* c = createCache();
	while (fscanf(fp, "%c %llx", &command, &address) != EOF) {
		tagBits = 48 - bBits - setBits;
		setIndex = (address >> bBits) & (two2Pow(setBits) - 1);
		tag = (address >> (bBits + setBits)) & (two2Pow(tagBits) - 1);

		if(command == 'W'){
			write(c, associativity);
		} else if(command == 'R'){
            		read(c, associativity);
        	}
            
    	}
	printf("memread:%d\n", reads);
	printf("memwrite:%d\n", writes);
	printf("cachehit:%d\n", hits);
	printf("cachemiss:%d\n", misses);
	fclose(fp);
	freeCache(c);
	return EXIT_SUCCESS;

	
}
