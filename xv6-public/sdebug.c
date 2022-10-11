#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5                      //프로세스 개수
#define PRINT_CYCLE 10000000       //프로세스가 정보를 출력하는 주기
#define TOTAL_COUNTER 500000000     //프로세스가 종료할 때 counter 값

void sdebug_func(void){
    int n,pid,wpid;
    // int end_pid[PNUM]={}; //terminated 출력하기 위함
    int t_start,t_end;

    printf(1, "start sdebug command\n");

    for(n=0;n<PNUM;n++){
        pid=fork();
        
        if(pid<0){
            printf(1,"sdebug: fork failed\n");
            exit();
        }
        if(pid==0){
            long counter;
            weightset(n+1);
            t_start=uptime();
            
            for(counter=0;counter<TOTAL_COUNTER;counter++){
                if (counter == PRINT_CYCLE){
                    t_end = uptime();
                    procinfo((t_end - t_start) * 10);
                    // end_pid[n] = procinfo((t_end - t_start) * 10);
                    // printf(1, "PID: %d terminated\n", end_pid[n]);
                }
            }
            exit();
        }
        while ((wpid = wait()) >= 0 && wpid != pid)
            printf(1, "zombie!\n");
    }
    
    // if (n == PNUM){
    //     printf(1, "fork claimed to work %d times!\n", PNUM);
    //     exit();
    // }

    // for (; n > 0; n--){
    //     if (wait() < 0){
    //         printf(1, "wait stopped early\n");
    //         exit();
    //     }
    // }

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