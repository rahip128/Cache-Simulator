#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void l1miss();

struct Cache { 
	unsigned long long** cache;
	int cacheSize, blockSize;
	char rP[4];
	int lines, setBits, tagBits, asc, numSets;
};
unsigned long long tagL1, tagL2;
unsigned long long address;
int setIndexL1, setIndexL2;
int l1cachehit = 0, l1cachemiss = 0, l2cachehit = 0, l2cachemiss = 0;
int reads = 0, writes = 0, bBits = 0;
char replacement_policy[] = "    ";
char command;
struct Cache* L1;
struct Cache* L2;
struct Cache* addressHolder;


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

struct Cache* createCache(int cS, int bS, int a, int l, char rP[4]) {
	struct Cache* c = malloc(sizeof(struct Cache));
	c->cacheSize = cS;
	c->blockSize = bS;
	c->asc = a;
	strcpy(c->rP, replacement_policy);
	if(c->asc == 1 || c->asc == 3){
		c->lines = l;
		c->numSets = (c->cacheSize/(c->blockSize*c->lines));
	} else {
		c->lines = c->cacheSize/c->blockSize;
		c->numSets = 1;

	}
	c->setBits = log2base(c->numSets);
	c->tagBits = 48 - bBits - c->setBits;
	c->cache = malloc(sizeof(unsigned long long) * c->numSets);
	
	for(int i = 0; i < c->numSets; i++){
		c->cache[i] = malloc(sizeof(unsigned long long) * c->lines);
	}

	for(int i = 0; i < c->numSets; i++){
		for(int j = 0; j < c->lines; j++){
			c->cache[i][j] = 0;
		}
	}
	return c;
}



void checkL2(){
	if(L1->asc == 1){
		if(L2->asc == 1){
			if(L2->cache[setIndexL2][0] == tagL2){
				l2cachehit++;
				if(L1->cache[setIndexL1][0] != 0 || L1->cache[setIndexL1][0] != -1){
					L1->cache[setIndexL1][0] = tagL2;
				} else {
					unsigned long long flag = L1->cache[setIndexL1][0];
					L1->cache[setIndexL1][0] = tagL2;
					L2->cache[setIndexL2][0] = flag;
				}
				
	
			} else {
				l2cachemiss++;
				reads++;
				if(L1->cache[setIndexL1][0] != 0 || L1->cache[setIndexL1][0] != -1){
					L1->cache[setIndexL1][0] = tagL1;
				} else {
					unsigned long long flag = L1->cache[setIndexL1][0];
					L1->cache[setIndexL1][0] = tagL1;
					L2->cache[setIndexL2][0] = flag;
				}
				
				
			}

		} else if (L2->asc == 2){
			int h = 0;
			for(int i = 0; i < (L2->cacheSize/L2->blockSize); i++){
				if(L2->cache[i][0] == tagL2){
					l2cachehit++;
					writes++;
					h = 1;
					break;
				}
			} 
			if(h == 1){
				L1->cache[setIndexL1][0] = tagL2;
				for(int i = 0; i < (L2->cacheSize/L1->blockSize) - 1; i++){
					L2->cache[i][0] = L2->cache[i+1][0];
				}
				L2->cache[(L2->cacheSize/L2->blockSize) - 1][0] = -1;
			} else {
				L1->cache[setIndexL1][0] = tagL1;
				l2cachemiss++;
				reads++;
				writes++;
			}




		} else if (L2->asc == 3){
			int h = 0;
			for(int i = 0; i < (L2->cacheSize/L1->blockSize); i++){
				if(L2->cache[setIndexL2][i] == tagL2){
					l2cachehit++;
					writes++;
					h = 1;
					break;
				}
			} 
			if(h == 1){
				L1->cache[setIndexL1][0] = tagL2;
				for(int i = 0; i < (L2->cacheSize/L2->blockSize) - 1; i++){
					L2->cache[setIndexL2][i] = L2->cache[setIndexL2][i];
				}
				L2->cache[setIndexL2][(L2->cacheSize/L2->blockSize) - 1] = -1;
			} else {
				L1->cache[setIndexL1][0] = tagL1;
				l2cachemiss++;
				reads++;
				writes++;
			}
		}
	} else if (L1->asc == 2){
		if(L2->asc == 1){
			if(L2->cache[setIndexL2][0] == tagL2){
				unsigned long long flag = L1->cache[0][0];
				for(int j = 0; j < ((L1->cacheSize/L1->blockSize)) - 1; j++){
					L1->cache[j][0] = L1->cache[j+1][0];
				}
				L1->cache[setIndexL1][0] = tagL2;
				l2cachehit++;
				writes++;
				L2->cache[setIndexL2][0] = flag;
			} else {
				L1->cache[setIndexL1][0] = tagL1;
				l2cachemiss++;
				reads++;
				writes++;
			}


		} else if (L2->asc == 2){
			int h = 0;
			for(int i = 0; i < (L2->cacheSize/L2->blockSize); i++){
				if(L2->cache[i][0] == tagL2){
					l2cachehit++;
					writes++;
					h = 1;					
					unsigned long long flag = L1->cache[0][0];
					for(int j = 0; j < ((L1->cacheSize/L1->blockSize)) - 1; j++){
						L1->cache[j][0] = L1->cache[j+1][0];
					}
					L2->cache[i][0] = flag;	
					L1->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = tagL2;
					break;
				}
			} 
			if(h != 1){
				unsigned long long flag = L1->cache[0][0];
				for(int j = 0; j < ((L1->cacheSize/L1->blockSize)) - 1; j++){
					L1->cache[j][0] = L1->cache[j+1][0];
				}
				for(int j = 0; j < ((L2->cacheSize/L2->blockSize)) - 1; j++){
					L2->cache[j][0] = L2->cache[j+1][0];
				}
				L2->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = flag;	
				L1->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = tagL1;

				l2cachemiss++;
				reads++;
				writes++;
			}




		} else if (L2->asc == 3){
			int h = 0;
			for(int i = 0; i < (L2->cacheSize/L2->blockSize); i++){
				if(L2->cache[setIndexL2][i] == tagL2){
					l2cachehit++;
					writes++;
					h = 1;					
					unsigned long long flag = L1->cache[0][0];
					for(int j = 0; j < ((L1->cacheSize/L1->blockSize)) - 1; j++){
						L1->cache[j][0] = L1->cache[j+1][0];
					}
					L2->cache[setIndexL2][i] = flag;	
					L1->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = tagL2;
					break;
				}
			} 
			if(h != 1){
				unsigned long long flag = L1->cache[0][0];
				for(int j = 0; j < ((L1->cacheSize/L1->blockSize)) - 1; j++){
					L1->cache[j][0] = L1->cache[j+1][0];
				}
				for(int j = 0; j < ((L2->cacheSize/L2->blockSize)) - 1; j++){
					L2->cache[setIndexL2][j] = L2->cache[setIndexL2][j+1];
				}
				L2->cache[setIndexL2][((L1->cacheSize/L1->blockSize)) - 1] = flag;	
				L1->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = tagL1;

				l2cachemiss++;
				reads++;
				writes++;
			}
		}
	} else if (L1->asc == 3){
		if(L2->asc == 1){
			if(L2->cache[setIndexL2][0] == tagL2){
				L1->cache[setIndexL1][0] = tagL2;
				l2cachehit++;
				writes++;
				L2->cache[setIndexL2][0] = -1;
			} else {
				L1->cache[setIndexL1][0] = tagL1;
				l2cachemiss++;
				reads++;
				writes++;
			}


		} else if (L2->asc == 2){
			int h = 0;
			for(int i = 0; i < (L2->cacheSize/L2->blockSize); i++){
				if(L2->cache[i][0] == tagL2){
					l2cachehit++;
					writes++;
					h = 1;					
					unsigned long long flag = L1->cache[0][0];
					for(int j = 0; j < ((L1->cacheSize/L1->blockSize)) - 1; j++){
						L1->cache[j][0] = L1->cache[j+1][0];
					}
					L2->cache[i][0] = flag;	
					L1->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = tagL2;
					break;
				}
			} 
			if(h != 1){
				unsigned long long flag = L1->cache[0][0];
				for(int j = 0; j < ((L1->cacheSize/L1->blockSize)) - 1; j++){
					L1->cache[j][0] = L1->cache[j+1][0];
				}
				for(int j = 0; j < ((L2->cacheSize/L2->blockSize)) - 1; j++){
					L2->cache[j][0] = L2->cache[j+1][0];
				}
				L2->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = flag;	
				L1->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = tagL1;

				l2cachemiss++;
				reads++;
				writes++;
			}




		} else if (L2->asc == 3){
			int h = 0;
			for(int i = 0; i < (L2->cacheSize/L2->blockSize); i++){
				if(L2->cache[setIndexL2][i] == tagL2){
					l2cachehit++;
					writes++;
					h = 1;					
					unsigned long long flag = L1->cache[0][0];
					for(int j = 0; j < ((L1->cacheSize/L1->blockSize)) - 1; j++){
						L1->cache[j][0] = L1->cache[j+1][0];
					}
					L2->cache[setIndexL2][i] = flag;	
					L1->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = tagL2;
					break;
				}
			} 
			if(h != 1){
				unsigned long long flag = L1->cache[0][0];
				for(int j = 0; j < ((L1->cacheSize/L1->blockSize)) - 1; j++){
					L1->cache[j][0] = L1->cache[j+1][0];
				}
				for(int j = 0; j < ((L2->cacheSize/L2->blockSize)) - 1; j++){
					L2->cache[setIndexL2][j] = L2->cache[setIndexL2][j+1];
				}
				L2->cache[setIndexL2][((L1->cacheSize/L1->blockSize)) - 1] = flag;	
				L1->cache[((L1->cacheSize/L1->blockSize)) - 1][0] = tagL1;

				l2cachemiss++;
				reads++;
				writes++;
			}
		}

	}

}

void write(){
	if(L1->asc == 1){
		if(L1->cache[setIndexL1][0] == tagL1){
			l1cachehit++;
		} else {
			l1cachemiss++;
			l1miss();
		}
	} else if (L1->asc == 2){
		int h = 0;
		for(int i = 0; i < L1->lines; i++){
			if(L1->cache[setIndexL1][i] == tagL1){
				l1cachehit++;
				if(strcmp(L1->rP, "lru") == 0){
					int a = 0;
					unsigned long long flag = L1->cache[setIndexL1][i];
					for(int j = 0; j < L1->lines; j++){
						if(L1->cache[setIndexL1][j] == 0){
							L1->cache[setIndexL1][j] = flag;
							addressHolder->cache[setIndexL1][j] = address;
							a = 1;
							break;
						}
					}
					if(a != 1){
						for(int j = i; j < L1->lines - 1; j++){
							L1->cache[setIndexL1][j] = L1->cache[setIndexL1][j+1];
							addressHolder->cache[setIndexL1][j] = addressHolder->cache[setIndexL1][j+1];
						}
						L1->cache[setIndexL1][L1->lines - 1] = flag;
						addressHolder->cache[setIndexL1][L1->lines - 1] = address;
					}
				}
				h = 1;
				break;
			}
		}
		if(h != 1){
			l1cachemiss++;
			l1miss();
		}
	} else if (L1->asc == 3){
		int h = 0;
		for(int i = 0; i < L1->lines; i++){
			if(L1->cache[setIndexL1][i] == tagL1){
				l1cachehit++;
				if(strcmp(L1->rP, "lru") == 0){
					int a = 0;
					unsigned long long flag = L1->cache[setIndexL1][i];
					for(int j = 0; j < L1->lines; j++){
						if(L1->cache[setIndexL1][j] == 0){
							L1->cache[setIndexL1][j] = flag;
							addressHolder->cache[setIndexL1][j] = address;
							a = 1;
							break;
						}
					}
					if(a != 1){
						for(int j = i; j < L1->lines - 1; j++){
							L1->cache[setIndexL1][j] = L1->cache[setIndexL1][j+1];
							addressHolder->cache[setIndexL1][j] = addressHolder->cache[setIndexL1][j+1];
						}
						L1->cache[setIndexL1][L1->lines - 1] = flag;
						addressHolder->cache[setIndexL1][L1->lines - 1] = address;
					}
				}
				h = 1;
				break;
			}
		}
		if(h != 1){	
			l1cachemiss++;
			l1miss();
		}
	}
}

void read(){
	if(L1->asc == 1){
		if(L1->cache[setIndexL1][0] == tagL1){
			l1cachehit++;
		} else {
			l1cachemiss++;
			l1miss();
		}
	} else if (L1->asc == 2){
		int h = 0;
		for(int i = 0; i < L1->lines; i++){
			if(L1->cache[setIndexL1][i] == tagL1){
				l1cachehit++;
				if(strcmp(L1->rP, "lru") == 0){
					int a = 0;
					unsigned long long flag = L1->cache[setIndexL1][i];
					for(int j = 0; j < L1->lines; j++){
						if(L1->cache[setIndexL1][j] == 0){
							L1->cache[setIndexL1][j] = flag;
							addressHolder->cache[setIndexL1][j] = address;
							a = 1;
							break;
						}
					}
					if(a != 1){
						for(int j = i; j < L1->lines - 1; j++){
							L1->cache[setIndexL1][j] = L1->cache[setIndexL1][j+1];
							addressHolder->cache[setIndexL1][j] = addressHolder->cache[setIndexL1][j+1];
						}
						L1->cache[setIndexL1][L1->lines - 1] = flag;
						addressHolder->cache[setIndexL1][L1->lines - 1] = address;
					}
				}
				h = 1;
				break;
			}
		}
		if(h != 1){
			l1cachemiss++;
			l1miss();
		}
	} else if (L1->asc == 3){
		int h = 0;
		for(int i = 0; i < L1->lines; i++){
			if(L1->cache[setIndexL1][i] == tagL1){
				l1cachehit++;
				if(strcmp(L1->rP, "lru") == 0){
					int a = 0;
					unsigned long long flag = L1->cache[setIndexL1][i];
					for(int j = 0; j < L1->lines; j++){
						if(L1->cache[setIndexL1][j] == 0){
							L1->cache[setIndexL1][j] = flag;
							addressHolder->cache[setIndexL1][j] = address;
							a = 1;
							break;
						}
					}
					if(a != 1){
						for(int j = i; j < L1->lines - 1; j++){
							L1->cache[setIndexL1][j] = L1->cache[setIndexL1][j+1];
							addressHolder->cache[setIndexL1][j] = addressHolder->cache[setIndexL1][j+1];
						}
						L1->cache[setIndexL1][L1->lines - 1] = flag;
						addressHolder->cache[setIndexL1][L1->lines - 1] = address;
					}
				}
				h = 1;
				break;
			}
		}
		if(h != 1){	
			l1cachemiss++;
			l1miss();
		}
	}
}




void l1miss(){
	int i = 0;
	int hit = 0;
	for(i = 0; i < L2->lines; i++){
		if(L2->cache[setIndexL2][i] == tagL2){ //L2 Hit
			hit = 1;
			l2cachehit++;
			for(int j = i; j < L2->lines-1; j++){
				L2->cache[setIndexL2][j] = L2->cache[setIndexL2][j+1];
			}
			L2->cache[setIndexL2][L2->lines-1] = 0;
			int h = 0;
			for (int k = 0; k < L1->lines; k++){
				if (L1->cache[setIndexL1][k] == 0){
					L1->cache[setIndexL1][k] = tagL1;
					addressHolder->cache[setIndexL1][k] = address;
					h = 1;
					break;
				}
			}
			if(h != 1){ //Evict case
				int a = 0;
				unsigned long long flag = addressHolder->cache[setIndexL1][0];

				for(int k = 0; k < L1->lines-1; k++){
					L1->cache[setIndexL1][k] = L1->cache[setIndexL1][k+1];
					addressHolder->cache[setIndexL1][k] = addressHolder->cache[setIndexL1][k+1];
				}
				L1->cache[setIndexL1][L1->lines - 1] = tagL1;
				addressHolder->cache[setIndexL1][L1->lines - 1] = address;
				
				for (int k = 0; k < L2->lines; k++){
					if (L2->cache[(flag >> bBits) & (L2->numSets - 1)][k] == 0){
						L2->cache[(flag >> bBits) & (L2->numSets - 1)][k] = (flag >> (bBits + L2->setBits)) & (two2Pow(L2->tagBits) - 1);
						a = 1;
						break;
					}		
				}
				if(a != 1){
					for(int k = 0; k < L2->lines-1; k++){
						L2->cache[(flag >> bBits) & (L2->numSets - 1)][k] = L2->cache[(flag >> bBits) & (L2->numSets - 1)][k+1];
					}
					L2->cache[(flag >> bBits) & (L2->numSets - 1)][L2->lines-1] = (flag >> (bBits + L2->setBits)) & (two2Pow(L2->tagBits) - 1);
				}	
			}
		}	
	}
	if(hit != 1){
		//L2 Miss
		int h = 0;
		l2cachemiss++;
		reads++;
		for (int k = 0; k < L1->lines; k++){
			if (L1->cache[setIndexL1][k] == 0){
				L1->cache[setIndexL1][k] = tagL1;
				addressHolder->cache[setIndexL1][k] = address;
				h = 1;
				break;
			}
		}
		if(h != 1){ //Evict case
			int a = 0;
			unsigned long long flag = addressHolder->cache[setIndexL1][0];

			for(int k = 0; k < L1->lines-1; k++){
				L1->cache[setIndexL1][k] = L1->cache[setIndexL1][k+1];
				addressHolder->cache[setIndexL1][k] = addressHolder->cache[setIndexL1][k+1];
			}

			L1->cache[setIndexL1][L1->lines - 1] = tagL1;
			addressHolder->cache[setIndexL1][L1->lines-1] = address;

			for (int k = 0; k < L2->lines; k++){
				if (L2->cache[(flag >> bBits) & (L2->numSets - 1)][k] == 0){
					L2->cache[(flag >> bBits) & (L2->numSets - 1)][k] = (flag >> (bBits + L2->setBits)) & (two2Pow(L2->tagBits) - 1);
					a = 1;
					break;
				}		
			}
			if(a != 1){
				for(int k = 0; k < L2->lines-1; k++){
					L2->cache[(flag >> bBits) & (L2->numSets - 1)][k] = L2->cache[(flag >> bBits) & (L2->numSets - 1)][k+1];
				}
				L2->cache[(flag >> bBits) & (L2->numSets - 1)][L2->lines-1] = (flag >> (bBits + L2->setBits)) & (two2Pow(L2->tagBits) - 1);
			}
		}
	}
}


void freeCache(struct Cache* c){
	for(int i = 0; i < c->numSets; i++){
		free(c->cache[i]);
	}
	free(c->cache);
	free(c);
}

int main(int argc, char* argv[argc+1]){
	int associativity = 0;
	int l = 1;
	if (argc != 9){
		printf("error\n");
		return EXIT_SUCCESS;
	}
	int cS1 = atoi(argv[1]);
	int cS2 = atoi(argv[5]);
	
	if(!((cS1 != 0) && (cS1 & (cS1 - 1)) == 0)){
		printf("error\n");
		return EXIT_SUCCESS;
	}

	if(!((cS2 != 0) && (cS2 & (cS2 - 1)) == 0)){
		printf("error\n");
		return EXIT_SUCCESS;
	}
	
	int blockSiz = atoi(argv[4]);
	if(!((blockSiz != 0) && ((blockSiz & (blockSiz - 1)) == 0))){
		printf("error2\n");
		return EXIT_SUCCESS;
	} else {
		bBits = log2base(blockSiz);
	}
	if(strcmp(argv[2], "direct") == 0){
		associativity = 1;

	} else if(strcmp(argv[2], "assoc") == 0){
		associativity = 2;

	} else {
		char* c = strtok(argv[2], ":");
		c = strtok(NULL, ":");
		if(c == NULL){
			printf("error\n");
			return EXIT_SUCCESS;
		}
		l = atoi(c);
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
	L1 = createCache(cS1, blockSiz, associativity, l, replacement_policy);
	addressHolder = createCache(cS1, blockSiz, associativity, l, replacement_policy);
	if(strcmp(argv[6], "direct") == 0){
		associativity = 1;
		l = 1;

	} else if(strcmp(argv[6], "assoc") == 0){
		associativity = 2;
		l = 1;

	} else {
		char* c = strtok(argv[6], ":");
		c = strtok(NULL, ":");
		if(c == NULL){
			printf("error\n");
			return EXIT_SUCCESS;
		}
		associativity = 3;
		l = atoi(c);
	}
	
	if(strcmp(argv[7], "lru") == 0){
		strcpy(replacement_policy, "lru");

	} else if(strcmp(argv[7], "fifo") == 0){
		strcpy(replacement_policy, "fifo");

	} else {
		printf("error\n");
		return EXIT_SUCCESS;
	}
	L2 = createCache(cS2, blockSiz, associativity, l, replacement_policy);
	FILE* fp = fopen(argv[8], "r");
	if(fp == 0){
		printf("error\n");
		return EXIT_SUCCESS;
	}

	
	while (fscanf(fp, "%c %llx", &command, &address) != EOF) {
		setIndexL1 = (address >> bBits) & (two2Pow(L1->setBits) - 1);
		tagL1 = (address >> (bBits + L1->setBits)) & (two2Pow(L1->tagBits) - 1);
		setIndexL2 = (address >> bBits) & (two2Pow(L2->setBits) - 1);
		tagL2 = (address >> (bBits + L2->setBits)) & (two2Pow(L2->tagBits) - 1);
		if(command == 'W'){
			write();
			writes++;
		} else if(command == 'R'){
            		read();
        	}
            
    	}
	printf("memread:%d\n", reads);
	printf("memwrite:%d\n", writes);
	printf("l1cachehit:%d\n", l1cachehit);
	printf("l1cachemiss:%d\n", l1cachemiss);
	printf("l2cachehit:%d\n", l2cachehit);
	printf("l2cachemiss:%d\n", l2cachemiss);
	fclose(fp);
	freeCache(L1);
	freeCache(L2);
	freeCache(addressHolder);
	return EXIT_SUCCESS;	
}
