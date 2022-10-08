#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5                      //프로세스 개수
#define PRINT_CYCLE 100000000       //프로세스가 정보를 출력하는 주기
#define TOTAL_COUNTER 500000000     //프로세스가 종료할 때 counter 값

void sdebug_func(void){
    int pid;

    printf(1, "start sdebug command\n");

    for(int i=0;i<PNUM;i++){
        pid=fork();
        if(pid<0){
            printf(1,"sdebug: fork failed\n");
            exit();
        }
        if(pid==0){

        }
    }

    printf(1,"end of sdebug command\n");
}

int main(void){
    //weightset() 써야함
    sdebug_func();
    exit();
}