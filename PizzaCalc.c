#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PI 3.14159265358979323846

//create and print out pizzas on a file w 
//use malloc callc and free to dynamically allocate mem

struct pizza{
    char* name;
    float ppd;
    struct pizza* next;
};

int comparator(struct pizza* current, struct pizza* new_pizza){
    int ret = 0;
    if(current->next->ppd == new_pizza->ppd){
        ret = strcmp(new_pizza->name, current->next->name)>0;
    }
    if(current->next->ppd > new_pizza->ppd){
        ret = 1;
    }
    return ret;
}

int main(int argc, char *argv[]){
    if(argc !=2){
        printf("Error: syntex for PizzaCalc is arglen <string>");
        return EXIT_FAILURE;
    }

    struct pizza* head = NULL;
    int count = 0;
    FILE *fp = fopen(argv[1], "r");

    if (fp == NULL) {
        printf("%s\n","Wrong file name or file unreadable");
        return EXIT_FAILURE;
    }
    while (1){
        char* pName = (char*) malloc(60*sizeof(char));
        float pDia;
        float pPrice;

        int x = fscanf(fp, "%s", pName);
        
        if(x ==-1){
            printf("%s\n","PIZZA FILE IS EMPTY");
            free(pName);
            return EXIT_SUCCESS;
        }
        //printf("%s\n", pName);
        
        if (strcmp(pName, "DONE") == 0){
            free(pName);
            //printf("%s\n", "exiting");
            break;
        }
        
        fscanf(fp, "%f", &pDia);
        //printf("%f\n", pDia);
        fscanf(fp, "%f", &pPrice);
        //printf("%f\n", pPrice);

        struct pizza* new_pizza = (struct pizza*)malloc(sizeof(struct pizza));
        //printf("%s\n", "ran malloc");

        new_pizza->name = pName;
        if(pPrice == 0){
            new_pizza->ppd = 0;

        }else{
            new_pizza->ppd = (PI/4*pDia*pDia)/pPrice;
        }
        new_pizza->next = NULL;

        if(head == NULL){
            head = new_pizza;
            //printf("%s, %s", head->name, tail->name);
        }else{
            struct pizza* current = head;
            
            if(new_pizza->ppd > current->ppd){
                struct pizza* temp = head;
                head = new_pizza;
                head->next = temp;
            }
            else{
                //printf("%s: %s\n", "New pizza-----------------------", new_pizza->name);
                while (current->next != NULL && comparator(current, new_pizza)>0){
                    //printf("%s\n", current->name);                    
                    current = current->next;
    
                    //printf(" %s\n", current->name);
                }
                new_pizza->next = current->next;
                current->next = new_pizza;

            }
        }
        count++;
    }
 
    fclose(fp);
    //sortList();
 
    for(int i = 0; i <count; i++){
        //free mem
      
        struct pizza* temp = head->next;
       
        printf("%s %f\n", head->name, head->ppd);
        free(head->name);
        free(head);
        head = temp;
    }
    return EXIT_SUCCESS;
}


//open the file in read mode using fopen()
    //if textfile == null, print message "PIZZA FILE IS EMPTY", return EXIT_FAILURE
//use fgets to read the next line
//while the next line is not "DONE" -> means is a name
    //malloc memory for a pizza struct pointer
    //malloc memory for the values name and ppd
    //read the next two lines and store them 

//close textfile using fclose
// return 0 in case of sucess 