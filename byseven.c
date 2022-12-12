#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if(argc !=2){
        printf("Error: syntex for byseven is ./byseven int");
        return EXIT_FAILURE;
    }
    else if (argc >2){
        printf("Too many arguments supplied.\n");
        return EXIT_FAILURE;
    }
    else{
        char *a = argv[1];
        int input = atoi(a);
        for (int i=1; i<=input; i++){
            printf("%d\n", i*7);
        }
    }

   
    return EXIT_SUCCESS;
}