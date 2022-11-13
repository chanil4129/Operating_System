#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define BUFMAX 4096
#define PAGE_STR 30
#define REF_STR 500
#define NOT_ESC 0
#define ESC 1

int reference_string[REF_STR];
char ESC_reference_modify[REF_STR]; //ESC R,W(D) 표시

void print_reference_string(int index,int isESC){
    for(int i=0;i<REF_STR;i++){
        if(isESC) {
            if(ESC_reference_modify[i]=='r')
                printf("R ");
                // printf("%dR ",);
            else
                printf("W ");
                // printf("%dW ",reference_string[i]);
        }
        else printf("%d ",reference_string[i]);
    }
    printf("\n");
}

void create_stream(){
    srand((unsigned int)time(NULL));
    for (int i = 0; i < REF_STR; i++){
        reference_string[i] = (int)(rand() % PAGE_STR + 1);
    }
    for (int i = 0; i < REF_STR; i++){
        if (rand() % 2){
            ESC_reference_modify[i] = 'r'; // reference bit
        }
        else{
            ESC_reference_modify[i] = 'd'; // dirty bit
        }
    }
}

void to_file(){

}

int main(void){
    create_stream();
    to_file();
    print_reference_string(0,NOT_ESC);
    print_reference_string(0,ESC);
}

