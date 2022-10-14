#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5                      //프로세스 개수
#define PRINT_CYCLE 10000000       //프로세스가 정보를 출력하는 주기
#define TOTAL_COUNTER 500000000     //프로세스가 종료할 때 counter 값

void sdebug_func(void){
    int n,pid;

    printf(1,"start sdebug command\n");

    for(n=0;n<PNUM;n++){ //PNUM 개수만큼 프로세스 생성(fork를 통해)
        pid=fork(); //프로세스 생성
        if(pid<0) //에러처리
            break;
        if(pid==0){ //자식 프로세스
            weightset(n+1); // 프로세스의 weight값 설정
            long counter=0; // 카운터 개수를 늘리면서 TOTAL_COUNTER값과 같아지면 프로세스 종료
            int print_counter=PRINT_CYCLE; //프로세스 정보를 출력하는 주기
            int start_ticks=uptime(); //프로세스 실행 시작 시간
            int end_ticks; //프로세스 출력할 때 걸린 시간
            int first=1; //한번만 출력할 수 있게 하기위한 임시 값

            while(counter<=TOTAL_COUNTER){ // 카운터 개수를 늘리면서 TOTAL_COUNTER값과 같아지면 프로세스 종료
                counter++; // 카운터 개수 늘리기
                print_counter--; // 출력 주기 카운트는 줄이기
                
                if (print_counter == 0){ //PRINT_CYCLE에서 0값이 됐으면
                    if (first){ //한번만 정보 출력
                        end_ticks = uptime(); // 출력하는 주기까지 걸린 시간
                        procinfo((end_ticks - start_ticks) * 10); //프로세스 정보 출력(PID,WEIGHT,TIMES)
                        first = 0; // 다음번엔 출력 못하게 하기
                    }
                    print_counter = PRINT_CYCLE; //주기 원위치
                }
            }
            printf(1,"PID: %d terminated\n",getpid()); //종료될 프로세스 정보 출력
            exit(); //프로세스 종료
        }
    }
    
    //모든 자식 프로세스 기다리기
    for (; n > 0; n--){
        if (wait() < 0){ //에러처리
            printf(1, "wait stopped early\n");
            exit();
        }
    }
    if (wait() != -1){ //에러처리
        printf(1, "wait got too many\n");
        exit();
    }

    printf(1,"end of sdebug command\n");
}

int main(void){
    sdebug_func();
    exit();
}