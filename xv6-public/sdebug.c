#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5                      //프로세스 개수
#define PRINT_CYCLE 100000000       //프로세스가 정보를 출력하는 주기
#define TOTAL_COUNTER 500000000     //프로세스가 종료할 때 counter 값

void sdebug_func(void){
    int n,pid;
    int t_start,t_end;

    printf(1, "start sdebug command\n");

    for(n=0;n<PNUM;n++){
        pid=fork();
        t_start=uptime();
        if(pid<0){
            printf(1,"sdebug: fork failed\n");
            exit();
        }
        if(pid==0){
            long counter=0;
            weightset(n+1);
            for(counter=0;counter<TOTAL_COUNTER;counter++){
                if (counter == PRINT_CYCLE){
                    t_end = uptime();
                    procinfo((t_end - t_start) * 10);
                }
            }
            exit();
        }

    }
    
    for(; n > 0; n--){
        if((pid=wait())<0){
            printf(1, "wait stopped early\n");
            exit();
        }
        else
            printf(1,"PID: %d terminated\n",pid);
    }

    if (wait() != -1){
        printf(1, "wait got too many\n");
        exit();
    }
    
    printf(1,"end of sdebug command\n");
}

int main(void){
    sdebug_func();
    exit();
}