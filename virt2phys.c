#include <stdio.h>
#include <stdlib.h>



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

int bitShiftR(unsigned int va, int page_size){
    int shift =  log2(page_size);
    va = va >> shift;
    return va;
}

int bitShiftL(unsigned int ppn, int page_size){
    int shift = log2(page_size);
    ppn = ppn << shift;
    return ppn;
}

int getLowerN(unsigned int va, int page_size){
    int shift = log2(page_size);
    int offset = va & ones(shift);
    return offset;
}

int main(int argc, char *argv[]){
    if(argc !=3){
        printf("Error: syntex for virt2phys is page table file name, virtual address (hex) ");
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(argv[1], "r"); //read from input 1 the file pointer
    if (fp == NULL) {
        printf("%s\n","Wrong file name or file unreadable");
        return EXIT_FAILURE;
    }

    char* va = argv[2]; //hex to decode
    int virtual_address = (int)strtol(va, NULL, 16);
   
    int a_bits = 0;
    int page_size = 0;

    fscanf(fp, "%d", &a_bits); //in bits
    fscanf(fp, "%d", &page_size); //in bytes

    //create array for vpn to ppn
    int virtual_pages = (1 << a_bits) / page_size;
    int page_table[virtual_pages];

    int ppn;
    for (int i=0; i < virtual_pages; i++){
        fscanf(fp,"%d", &ppn);
        page_table[i] = ppn;
    }

    //translate virtual add to vpn
    //int binary_va = hexTranlsate(virtual_address);

    //shift the binary va right by log(page_size) to get vpn
    int vpn = bitShiftR(virtual_address, page_size);
    int offset = getLowerN(virtual_address, page_size);

    //get corresponding ppn to vpn
    unsigned int result_ppn = page_table[vpn];

    if (result_ppn == -1){
        printf("%s", "PAGEFAULT\n");
        return EXIT_SUCCESS;
    }
    else{
        result_ppn = bitShiftL(result_ppn, page_size);
        result_ppn += offset;
        printf("%x\n", result_ppn);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;

}