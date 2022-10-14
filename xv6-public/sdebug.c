#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5                      //프로세스 개수
#define PRINT_CYCLE 10000000       //프로세스가 정보를 출력하는 주기
#define TOTAL_COUNTER 500000000     //프로세스가 종료할 때 counter 값

void sdebug_func(void){
    int n,pid;


    printf(1,"start sdebug command\n");

    for(n=0;n<PNUM;n++)
    {
        pid=fork();
        if(pid<0)
            break;
        if(pid==0)
        {
            int counter=0;
            int end_ticks;
            int print_counter=PRINT_CYCLE;
            int start_ticks=uptime();
            weightset(n+1);
            int first=1;

            while(counter<=TOTAL_COUNTER)
            {
                if(counter==TOTAL_COUNTER)
                    break;
                counter++;
                if (print_counter-- == 0)
                {
                    if (first)
                    {
                        end_ticks = uptime();
                        procinfo((end_ticks - start_ticks) * 10);

                        first = 0;
                    }
                    print_counter = PRINT_CYCLE;
                }
            }

            printf(1,"PID: %d terminated\n",getpid());
            exit();
        }
    }
    
    for (; n > 0; n--)
    {
        if (wait() < 0)
        {
            printf(1, "wait stopped early\n");
            exit();
        }
    }

    if (wait() != -1)
    {
        printf(1, "wait got too many\n");
        exit();
    }

    printf(1,"end of sdebug command\n");
}

int main(void){
    sdebug_func();
    exit();
}