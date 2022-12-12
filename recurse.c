#include <stdio.h>
#include <stdlib.h>

int recursion(int i){
    if(i ==0){
        return 2;
    }
    else{
        return 3*(i-1)+recursion(i-1)+1;
    }
}

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
        int ret = recursion(input);
        printf("%d\n", ret);
    }

   
    return EXIT_SUCCESS;
}