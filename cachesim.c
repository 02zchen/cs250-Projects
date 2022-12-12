#include <stdio.h>
#include <stdlib.h>
#include <cstring>

//create flat mem;
char memory[2<<24] = {0};

int log2(int page_size){
    int ret = 0;
    while (page_size > 0){
        page_size = page_size >> 1;
        ret ++;
    }
    return ret-1;
}

int ones(unsigned int x){
    return (1ul<<x)-1;
}

struct Node{
    int tag;
    struct Node* next;
    struct Node* prev;
    char* data;
};

struct cacheSet{
    int ways;
    int nodeCount = 0;
    struct Node* head;
    struct Node* tail;
    int cacheNum;
};

void load(struct cacheSet* cache, int access_size, unsigned int address, int tag, int index, int block_offset, int block_size ){
    //printf("%hx")
    struct cacheSet* set = &cache[index];
    //printf("%s %d %d %d %s", "set, tag, nodecount: ", set->cacheNum, tag, set->nodeCount, " | ");
    struct Node* current = set->head;
    int hit = 0;
    /*
    if(current == NULL){
        printf("%s", "Is null");
    }
    */
    
    while(current != NULL){
        //if you find the tag, move the node to the front and toggle hit 
        //printf("%02hhx\n", current->tag);  
             
        if(current->tag == tag){
             
            hit = 1;
            //if the matching node is the head, dont swap anything
            if(current == set->head){
                //printf("%s", "case 1\n");
                //printf("%02hhx\n", current->data[0]);
                break;
            }
            //if the matching node is the tail, move it to the front
            if(current == set->tail){
                //printf("%s %02hhx\n", "looking for tag: ", tag);
                //printf("%s %02hhx\n", "head tag: ", set->head->tag);
                //printf("%02hhx\n", set->head->next->tag);      
                //printf("%02hhx", set->tail->prev->tag);   
                //printf("%s %02hhx\n", "tail tag: ", set->tail->tag);        
                
                //printf("%s", "case 2\n");
                current->prev->next = NULL;
                set->tail = current->prev;
                current->prev = NULL;
                current->next = set->head; 
                current->next->prev = current;
                set->head = current;
                //printf("%02hhx", set->head->tag);        
                
                //printf("%02hhx\n", set->tail->tag);   
                break;
            }
            else{
                //printf("%s", "case 3\n");
                struct Node* current_next = current->next; // null after 1
                struct Node* current_prev = current->prev; // null after 1
                
                current_next->prev = current_prev;  //null-
                current_prev->next = current_next;

                current->next = set->head;
                set->head->prev = current;

                set->head = current;
                current->prev = NULL;
                break;
            }

        }
        current = current->next;
    }
    //printf("%s", "breaks\n");
    if(hit){
        printf("%s 0x%x %s", "load ", address, " hit ");
       
        for (int i=0; i < access_size; i++){

             printf("%02hhx", current->data[block_offset+i]);
            
        }
        printf("\n");
    }
    else{
        //if conflict miss / capacity miss
        if (set->nodeCount >= set->ways){
            //find the lru node and remove it from the list
            if (set->nodeCount == 1){
                //printf("%s\n", "triggered limit 1");
                //just evict the node from the linked list
                set->head = NULL;
                set->tail = NULL;
                free(set->head);
                free(set->tail);
                
                
            }
            else{
                //its this 
                //printf("%s\n", "triggered limit 2");
                struct Node* lru = set->tail;
                lru->prev->next = NULL;
                set->tail = lru->prev;
                lru->prev = NULL;
                
                free(lru);
            }
            set->nodeCount -=1;
            //free lru
            //free(lru);
        }
        //create the node to be stored, add it to the front of the linked list cache
        struct Node* insert = (struct Node*)malloc(sizeof(struct Node)); 

        char* tempData = (char*) calloc(block_size, sizeof(char));
        //go to memory and store the entire block into cache 
        for (int i = 0; i < block_size; i++){
            //printf("%02hhx", memory[address+i]);
            tempData[i]= memory[address+i-block_offset];
        }
        
        //printf("%02hhx", tempData[0]);
        printf("%s 0x%x %s", "load ", address, " miss ");
        for (int i = 0; i < access_size; i++){
            //printf("%02hhx", memory[address+i]);
            printf("%02hhx", tempData[i+block_offset]);
        }

        printf("\n");
        insert->data = tempData;
        insert->tag = tag;
    
        insert->prev = NULL;
        insert->next = set->head;
        if(set->nodeCount == 0){ 
            set->tail = insert; //have to start the linked list by also setting tail to the first node
        }
        else{
            insert->next->prev = insert; //if you have more than 1 node, you have to link the old set head's previous to the next not
        }
        
        set->head = insert;
    
        set->nodeCount +=1;
        //printf("%s\n", "reached end of one loop");

    }
    
}




void store(struct cacheSet* cache, int access_size, unsigned int address, int tag, int index, int block_offset, char* store_value){
    struct cacheSet* set = &cache[index];
    struct Node* current = set->head;
    int hit = 0;
    //iterate through the set's linked list, looking for the node of interest
    while (current != NULL){
        // if hit, put the node into the front (most recent) and then push the data to the cache 
        if(current->tag == tag){
            hit =1;
            if(current == set->head){
                //printf("%s", "case 1\n");
                //printf("%02hhx\n", current->data[0]);
                break;
            }

            if(current == set->tail){
                //printf("%s %02hhx\n", "looking for tag: ", tag);
                //printf("%s %02hhx\n", "head tag: ", set->head->tag);
                //printf("%02hhx\n", set->head->next->tag);      
                //printf("%02hhx", set->tail->prev->tag);   
                //printf("%s %02hhx\n", "tail tag: ", set->tail->tag);        
                
                //printf("%s", "case 2\n");
                current->prev->next = NULL;
                set->tail = current->prev;
                current->prev = NULL;
                current->next = set->head; 
                current->next->prev = current;
                set->head = current;
                //printf("%02hhx", set->head->tag);        
                
                //printf("%02hhx\n", set->tail->tag);   
                break;
            }
            else{
                //printf("%s", "case 3\n");
                struct Node* current_next = current->next; // null after 1
                struct Node* current_prev = current->prev; // null after 1
                
                current_next->prev = current_prev;  //null-
                current_prev->next = current_next;

                current->next = set->head;
                set->head->prev = current;

                set->head = current;
                current->prev = NULL;
                break;
            }
        }
        current = current->next;
    }


    //regardless of hit or miss, push to lower level, stores properly
    for(int i = 0; i < access_size; i++){
        memory[address+i] = store_value[i];
    }

    if(hit){
        //printf("%d", block_offset);
        for (int i = 0; i<access_size; i++){    
                
            current->data[i+block_offset] = store_value[i];
        }
        printf("%s 0x%x %s\n", "store ", address, " hit");
    }
    else{
        printf("%s 0x%x %s\n", "store ", address, " miss");
    }
}



int main(int argc, char *argv[]){
    if(argc !=5){
        printf("Error: syntex for virt2phys is<trace-file> <cache-size-kB> <associativity> <block-size>\n");
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(argv[1], "r"); //read from the trace file
    int cache_size =  (int) strtol(argv[2], NULL, 0); // in kb, multiply by 1024
    cache_size = cache_size*1024;
    int associativity =  (int)strtol(argv[3], NULL,0 ); // num of columns
    int block_size =  (int)strtol(argv[4],NULL, 0); // in bytes
    //printf("%d\n",block_size);

     ; //num of blocks
    int numSets = cache_size / block_size / associativity; // num of rows

    //set the int partitions for the address bits
    int offset_bits = log2(block_size);
    int index_bits = log2(numSets);
    int tag_bits = 24 - index_bits-offset_bits;

    if (fp == NULL) {
        printf("%s\n","Wrong file name or file unreadable");
        return EXIT_FAILURE;
    }

    //create pointer for a region of n=umSet amounts of cachsets
    struct cacheSet* cache = (struct cacheSet*) malloc(numSets*sizeof(cacheSet));

    //initialize each of the sets
    for(int i = 0; i < numSets; i++){
        cache[i].ways = associativity;
        cache[i].nodeCount = 0;
        //printf("%d", cache[i].nodeCount);
        //cache[i].block_size = block_size;
        cache[i].cacheNum = i;
    }
    
    /*
    for (int j = 0; j< 5; j++){
        changeCaches(&cache[0]);
        printf("%d", cache[0].nodeCount);
    }
    */
    //create buffers to store scanned data
    char insn[64];
    int is_store;
    unsigned int address;
    int access_size; 
    char store_value[8];

    // loop through every line of tracefil until EOF
    while (fscanf(fp, "%s 0x%x %d", insn, &address, &access_size) == 3){
        
        //printf("%s 0x%x %d\n", insn, address, access_size);
        //break address into tag, index, offset
        int tag = address >> (offset_bits+index_bits);
        int index = (address>> offset_bits) & ones(index_bits);
        int block_offset = address & ones(offset_bits);
        //printf("%d %d %d\n", tag, index, block_offset);
        
        is_store = strcmp(insn, "store");
        //printf("%s %d\n", "is a store: ", is_store);
        
        //stores
        if(!is_store){
            //get value to be stored
            for(int i  = 0; i <access_size; i++){
                fscanf(fp, "%2hhx", &store_value[i]); 
            }
            store(cache, access_size, address, tag, index, block_offset, store_value);
        }
        else{
             //get the index set
            // malloc space for a node insert outside of the function
           
            load(cache, access_size, address, tag, index, block_offset, block_size);

    
            //printf("%s", "returns\n");
            //printf("%s %d\n", "node count: ", cache[0].nodeCount);
            //printf("%s %d\n", "node tag: ", cache[0].head->tag);
            
            


            
        }
    }
    fclose(fp);
    //clear heap of cache;
    
    for (int i =0; i<numSets; i++){
        struct cacheSet set = cache[i];
        struct Node* head = set.head;
        for(int j = 0; j < set.nodeCount; j++){
            struct Node* temp = head->next;
            free(head);
            head = temp;
        }
    }
    
   //printf("%02hhx", memory[63]);
   
    return EXIT_SUCCESS;
}