#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define ALGOMAX 7
#define PAGE_FRME_MAX 10
#define ARGMAX 11
#define BUFMAX 4096
#define PAGE_STR 30
#define REF_STR 500
#define FILE_ARGMAX 1000
#define NOT_ESC 0
#define ESC 1

//page_frame을 표현할 노드
typedef struct node{
    int data;
    struct node *next;
} node;
//page_frame을 표현할 자료구조(큐)
typedef struct queue{
    node *front;
    node *rear;
    int count;
} queue;

int split(char *string, char *seperator, char *argv[]);//string에 있는 내용을 seperator를 기준으로 토큰 분리해서 argv에 담는 함수
void init_reference_string(int data_input,int total_algorithm);//페이지 스트링 생성. data_input이 1이면 랜덤 생성 2이면 지정된 파일로부터 읽어 들이기
int count_algorithm(int selected_algorithm[]);//사용자가 몇개의 알고리즘을 선택했는지 확인하여 그 개수를 리턴하는 함수
void print_reference_string(int index,int isESC);//해당 알고리즘의 reference_string을 출력하는 함수. isESC가 ESC이면 RW도 출력
void page_replacement(int selected_algorithm[]);//seleced_algorithm[]에 해당되는 page 교체 알고리즘 실행
void printAndSave_page_frame(int page_frame[], int algorithm_index, int i,int isESC);//page_frame 표준출력 및 파일에 저장
void optimal(int algorithm_index);//optimal 알고리즘
void fifo(int algorithm_index);//fifo 알고리즘
void lifo(int algorithm_index);//lifo 알고리즘
void lru(int algorithm_index);//lru 알고리즘
void lfu(int algorithm_index);//lfu 알고리즘
void sc(int algorithm_index);//sc 알고리즘
void esc(int algorithm_index);//esc 알고리즘
void initQ(queue *q);//q 초기화
int emptyQ(queue *q);//q 비어있는지 확인
int selectQ_LRU(queue *q,int data);//LRU victim 찾기
void insertQ(queue *q,int data);//q에 data 삽입
int sizeQ(queue *q);//q 크기 확인하여 그 개수 리턴
void deleteQ(queue *q);//q victim 삭제
int isHit(queue *q,int data);//hit인지 확인
void deleteQ_LFU(queue *q, int node_index);//LFU q victim 삭제
int victimQ_LFU(queue *q,int frequent[]);//LFU victim 찾기
void printAndSaveQ(queue *q,int algorithm_index,int index);//리스트 결과 표준출력 및 파일에 저장. 단계-reference string-page frame 순으로 출력
void printAndSave_algorithm_list(char *kind_algorithm, int isESC);//배열 결과 표준출력 및 파일에 저장. 단계-reference string-page frame 순으로 출력
void printandSave_pageFault(int page_fault);//page fault 개수 표준출력 및 파일에 저장
int toBit(int index);//referece string에서 Read면 2, Write면 3 리턴 
void getOptimalPageFault();//optimal 알고리즘을 실행했을 때 page_fault 값 출력 및 파일에 저장

int reference_string[ALGOMAX][REF_STR];//reference string
char ESC_reference_modify[REF_STR]; //ESC R,W(D) 표시
int page_frame_size;//사용자가 입력한 page frame 크기
int rw_bit[PAGE_FRME_MAX];

int main(void){
    int A_argc;
    int B_argc;
    int C_argc;
    char *A_argv[ARGMAX];
    char *B_argv[ARGMAX];
    char *C_argv[ARGMAX];
    char input[BUFMAX];
    int selected_algorithm[ALGOMAX];
    int total_algorithm;
    int data_input;
    //Page Replacement 알고리즘 시뮬레이터 선택
    while(1){
        printf("A. Page Replacement 알고리즘 시뮬레이터를 선택하시오 (최대 3개)\n");
        printf("(1) Optimal   (2) FIFO   (3) LIFO   (4) LRU   (5) LFU   (6) SC   (7) ESC   (8) All\n");
        int A_exception=0;
        memset(selected_algorithm,0,sizeof(selected_algorithm)); //0으로 초기화
        //사용자 입력 받기
        fgets(input,sizeof(input),stdin);
        input[strlen(input)-1]='\0';
        A_argc=split(input," ",A_argv);
        //2개 이상인데 8을 선택했을 경우. 1보다 작거나 8보다 큰 수를 선택한 경우
        for(int i=0;i<A_argc;i++){
            if((A_argc>1&&!strcmp(A_argv[i],"8"))||(atoi(A_argv[i])<1||atoi(A_argv[i])>8))
                A_exception=1;
        }//예외처리
        if(A_argc>3||A_argc==0||A_exception){
            printf("input error.\nUsage : 1 2 5\n      | 6\n      | 2 4\n      | 8\n");
            continue;
        }
        //8을 입력하면 selected_algorithm의 모든 비트들 1로 켜주기
        if(!strcmp(A_argv[0],"8")){
            for(int i=0;i<7;i++)
                selected_algorithm[i]=1;
            break;
        }
        //8이 아닌 경우 selected_algorithm에 해당되는 비트들 1로 켜주기
        for(int i=0;i<A_argc;i++){
            selected_algorithm[atoi(A_argv[i])-1]=1;
        }
        break;
    }
    //페이지 프레임 개수 입력
    while(1){
        printf("B. 페이지 프레임의 개수를 입력하시오.(3~10)\n");
        //사용자 입력
        fgets(input,sizeof(input),stdin);
        input[strlen(input)-1]='\0';
        B_argc=split(input," ",B_argv);
        //입력이 1개가 아닌경우 경우 에러처리
        if(B_argc!=1){
            printf("input error\n");
            continue;
        }
        page_frame_size=atoi(B_argv[0]);
        //3보다 작거나 10보다 큰 경우 에러처리
        if(page_frame_size<3||page_frame_size>10){
            printf("input error. Usage : page frame is between 3 and 10\n");
            continue;
        }
        break;
    }
    //데이터 입력 방식
    while(1){
        printf("C. 데이터 입력 방식을 선택하시오.(1,2)\n");
        //사용자 입력
        fgets(input,sizeof(input),stdin);
        input[strlen(input)-1]='\0';
        C_argc=split(input," ",C_argv);
        //입력이 1개가 아니거나 입력 값이 1 또는 2가 아니면 에러처리
        if(C_argc!=1||strcmp(C_argv[0],"1")&&strcmp(C_argv[0],"2")){
            printf("input error.\nUsage : 1\n      | 2\n");
            continue;
        }
        data_input=atoi(C_argv[0]);
        break;
    }

    total_algorithm=count_algorithm(selected_algorithm);//몇개의 알고리즘이 선택됐는지 total_algorithm에 저장
    init_reference_string(data_input,total_algorithm);//페이지 스트링 생성. data_input이 1이면 랜덤 생성 2이면 지정된 파일로부터 읽어 들이기
    getOptimalPageFault();//optimal 알고리즘을 실행했을 때 page_fault 값을 얻는 함수
    page_replacement(selected_algorithm);//selected_algorithm에서 선택된 알고리즘들 실행

    printf("D. 종료\n");
    exit(0);
}

//selected_algorithm에서 선택된 알고리즘들 실행
void page_replacement(int selected_algorithm[]){
    int algorithm_index=0;
    if(selected_algorithm[0]) optimal(algorithm_index++);
    if(selected_algorithm[1]) fifo(algorithm_index++);
    if(selected_algorithm[2]) lifo(algorithm_index++);
    if(selected_algorithm[3]) lru(algorithm_index++);
    if(selected_algorithm[4]) lfu(algorithm_index++);
    if(selected_algorithm[5]) sc(algorithm_index++);
    if(selected_algorithm[6]) esc(algorithm_index++);
}

//배열 결과 표준출력 및 파일에 저장. 단계-reference string-page frame 순으로 출력
void printAndSave_algorithm_list(char *kind_algorithm, int isESC){
    FILE *fp=NULL;
    char *filename="result.txt";
    //표준 출력
    printf("\n%s\n",kind_algorithm);
    if(isESC) printf("단계 : (reference string)    - (page frame)(RD Bit)\n");
    else printf("단계 : (reference string)    - (page frame)\n");
    //파일 오픈
    if((fp=fopen(filename,"a+"))==NULL){
        fprintf(stderr,"file open error\n");
        exit(1);
    }
    //파일에 저장
    fprintf(fp,"\n%s\n",kind_algorithm);
    if(isESC) fprintf(fp,"단계 : (reference string)    - (page frame)(RD Bit)\n");
    else fprintf(fp,"단계 : (reference string)    - (page frame)\n");
    fclose(fp);
}
//page fault 개수 표준출력 및 파일에 저장
void printandSave_pageFault(int page_fault){
    FILE *fp=NULL;
    char *filename="result.txt";
    //표준 출력
    printf("Page fault : %d\n",page_fault);
    //파일 오픈
    if((fp=fopen(filename,"a+"))==NULL){
        fprintf(stderr,"file open error\n");
        exit(1);
    }
    //파일에 저장
    fprintf(fp,"Page fault : %d\n",page_fault);
    fclose(fp);
}

//page_frame 표준출력 및 파일에 저장
void printAndSave_page_frame(int page_frame[], int algorithm_index, int i, int isESC){
    FILE *fp=NULL;
    char *filename="result.txt";

    //표준 출력
    if(isESC) //ESC인 경우
        printf("%4d : %16d%c     -  ", i+1,reference_string[algorithm_index][i],ESC_reference_modify[i]);
    else //나머지 알고리즘의 경우
        printf("%4d : %17d     -  ", i+1,reference_string[algorithm_index][i]);
    for(int i=0;i<page_frame_size;i++){
        if(page_frame[i]==-1)
            ;
        else{
            if(isESC){ 
            if(rw_bit[i]==0) printf("%4d (00) ", page_frame[i]);
            if(rw_bit[i]==1) printf("%4d (01) ", page_frame[i]);
            if(rw_bit[i]==2) printf("%4d (10) ", page_frame[i]);
            if(rw_bit[i]==3) printf("%4d (11) ", page_frame[i]);
            }
            else printf("%-4d ",page_frame[i]);
        }
    }
    printf("\n");
    //파일 오픈
    if((fp=fopen(filename,"a+"))==NULL){
        fprintf(stderr,"file open error\n");
        exit(1);
    }
    if(isESC) //ESC인 경우
        fprintf(fp,"%4d : %16d%c     -  ", i+1, reference_string[algorithm_index][i],ESC_reference_modify[i]);
    else //나머지 알고리즘의 경우
        fprintf(fp,"%4d : %17d     -  ", i+1,reference_string[algorithm_index][i]);
    for(int i=0;i<page_frame_size;i++){
        if(page_frame[i]==-1)
            ;
        else{
            if(isESC){ 
            if(rw_bit[i]==0) fprintf(fp,"%4d (00) ", page_frame[i]);
            if(rw_bit[i]==1) fprintf(fp,"%4d (01) ", page_frame[i]);
            if(rw_bit[i]==2) fprintf(fp,"%4d (10) ", page_frame[i]);
            if(rw_bit[i]==3) fprintf(fp,"%4d (11) ", page_frame[i]);
            }
            else fprintf(fp,"%-4d ",page_frame[i]);
        }
    }
    fprintf(fp,"\n");
    fclose(fp);
}

//optimal 알고리즘
void optimal(int algorithm_index){
    int page_frame[PAGE_FRME_MAX];
    int fault_count=0;
    int i,j;
    printAndSave_algorithm_list("Optimal",NOT_ESC);
    memset(page_frame,-1,sizeof(page_frame)); //비어있는 page_frame은 -1로 표현
    for(i=0;i<REF_STR;i++){ //모든 reference string을 돌면서
        int replace=1; //교체를 해야하면 1 아니면 0
        for(j=0;j<page_frame_size;j++){ //page_frame을 돌면서
            if(page_frame[j]==reference_string[algorithm_index][i]){//Hit인 경우
                replace=0;
                break;
            }
            else if(page_frame[j]==-1){//비어있는 경우
                replace=0;
                page_frame[j]=reference_string[algorithm_index][i];//page_frame에 값 넣기
                fault_count++;//page_fault 카운팅
                break;
            }
        }
        if(replace){ //교체를 해야한다면
            int long_time[PAGE_FRME_MAX];
            int max_time=-1;
            int max_index=0;
            int k,t;
            fault_count++;//page_fault 카운팅
            for(k=0;k<page_frame_size;k++){//모든 page frame 돌면서
                for(t=i+1;t<REF_STR;t++){ //다음 reference string을 돌면서
                    if (page_frame[k] == reference_string[algorithm_index][t]) //page frame에 있는 값이랑 reference string이 처음한번 같을 때까지 반복
                        break;
                }
                long_time[k]=t;//얼마나 오래 걸리는지 체킹
            }
            for(k=0;k<page_frame_size;k++){//page frame 돌면서
                if(long_time[k]>max_time){//가장 오래 걸리는값(victim) 찾기
                    max_time=long_time[k];
                    max_index=k;
                }
            }
            page_frame[max_index]=reference_string[algorithm_index][i];//victim 교체
        }
        printAndSave_page_frame(page_frame,algorithm_index,i,NOT_ESC);//결과 출력
    }
    printandSave_pageFault(fault_count);//page fault 개수 출력
}

//optimal 알고리즘을 실행했을 때 page_fault 값을 얻는 함수
void getOptimalPageFault(){
    FILE *fp = NULL;
    char *filename="result.txt";
    int page_frame[PAGE_FRME_MAX];
    int fault_count = 0;
    int i, j;
    memset(page_frame,-1,sizeof(page_frame)); //비어있는 page_frame은 -1로 표현
    for(i=0;i<REF_STR;i++){ //모든 reference string을 돌면서
        int replace=1; //교체를 해야하면 1 아니면 0
        for(j=0;j<page_frame_size;j++){ //page_frame을 돌면서
            if(page_frame[j]==reference_string[0][i]){//Hit인 경우
                replace=0;
                break;
            }
            else if(page_frame[j]==-1){//비어있는 경우
                replace=0;
                page_frame[j]=reference_string[0][i];//page_frame에 값 넣기
                fault_count++;//page_fault 카운팅
                break;
            }
        }
        if(replace){ //교체를 해야한다면
            int long_time[PAGE_FRME_MAX];
            int max_time=-1;
            int max_index=0;
            int k,t;
            fault_count++;//page_fault 카운팅
            for(k=0;k<page_frame_size;k++){//모든 page frame 돌면서
                for(t=i+1;t<REF_STR;t++){ //다음 reference string을 돌면서
                    if (page_frame[k] == reference_string[0][t]) //page frame에 있는 값이랑 reference string이 처음한번 같을 때까지 반복
                        break;
                }
                long_time[k]=t;//얼마나 오래 걸리는지 체킹
            }
            for(k=0;k<page_frame_size;k++){//page frame 돌면서
                if(long_time[k]>max_time){//가장 오래 걸리는값(victim) 찾기
                    max_time=long_time[k];
                    max_index=k;
                }
            }
            page_frame[max_index]=reference_string[0][i];//victim 교체
        }
    }
    printf("Optimal page fault : %d\n",fault_count);
    if((fp=fopen(filename,"a+"))==NULL){
        fprintf(stderr,"file open error\n");
        exit(1);
    }
    fprintf(fp,"Optimal page fault : %d\n",fault_count);
    fclose(fp);
}

//fifo 알고리즘
void fifo(int algorithm_index){
    int page_frame[PAGE_FRME_MAX];
    int page_frame_idx=0;
    int fault_count=0;
    int i,j;
    printAndSave_algorithm_list("FIFO",NOT_ESC);
    memset(page_frame,-1,sizeof(page_frame));//비어있는 page_frame은 -1로 표현
    for(i=0;i<REF_STR;i++){//모든 reference string을 돌면서
        int replace=1;//교체를 해야하면 1 아니면 0
        for(j=0;j<page_frame_size;j++){//page_frame을 돌면서
            if(page_frame[j]==reference_string[algorithm_index][i]){//Hit인 경우
                replace=0;
                break;
            }
            else if(page_frame[j]==-1){//비어있는 경우
                replace=0;
                page_frame[j]=reference_string[algorithm_index][i];
                fault_count++;//page_fault 카운팅
                break;
            }
        }
        if(replace){//교체를 해야한다면
            fault_count++;//page_fault 카운팅
            page_frame[page_frame_idx%page_frame_size]=reference_string[algorithm_index][i];//먼저 넣은 page를 교체하기
            page_frame_idx++;//다음 page를 가리킬 수 있도록(오래된 victim 가리키는 포인터)
        }
        printAndSave_page_frame(page_frame,algorithm_index,i,NOT_ESC);//결과 출력
    }
    printandSave_pageFault(fault_count);//page fault 개수 출력
}

//lifo 알고리즘
void lifo(int algorithm_index){
    int page_frame[PAGE_FRME_MAX];
    int fault_count=0;
    int i,j;
    printAndSave_algorithm_list("LIFO",NOT_ESC);
    memset(page_frame,-1,sizeof(page_frame));//비어있는 page_frame은 -1로 표현
    for(i=0;i<REF_STR;i++){//모든 reference string을 돌면서
        int replace=1;//교체를 해야하면 1 아니면 0
        for(j=0;j<page_frame_size;j++){//page_frame을 돌면서
            if(page_frame[j]==reference_string[algorithm_index][i]){//Hit인 경우
                replace=0;
                break;
            }
            else if(page_frame[j]==-1){//비어있는 경우
                replace=0;
                page_frame[j]=reference_string[algorithm_index][i];//page_frame에 값 넣기
                fault_count++;//page_fault 카운팅
                break;
            }
        }
        if(replace){//교체를 해야한다면
            fault_count++;//page_fault 카운팅
            page_frame[page_frame_size-1]=reference_string[algorithm_index][i]; //가장 최근에 바꾼 page를 교체
        }
        printAndSave_page_frame(page_frame,algorithm_index,i,NOT_ESC);//결과 출력
    }
    printandSave_pageFault(fault_count);//page fault 개수 출력
}

//lru 알고리즘
void lru(int algorithm_index){
    int fault_count=0;
    int i,j;
    queue q; //리스트
    printAndSave_algorithm_list("LRU",NOT_ESC);
    initQ(&q);//q 초기화
    for(i=0;i<REF_STR;i++){//모든 reference string을 돌면서
        if(!selectQ_LRU(&q,reference_string[algorithm_index][i])){ //못 찾으면
            fault_count++;//page_fault 카운팅
            if (sizeQ(&q) >= page_frame_size) //page frame이 꽉 찼다면
                deleteQ(&q); //가장 오래된 page 삭제
            insertQ(&q,reference_string[algorithm_index][i]); //리스트 추가
        }
        printAndSaveQ(&q,algorithm_index,i);//결과 출력
    }
    printandSave_pageFault(fault_count);//page_fault 카운팅
}

void initQ(queue *q){//q 초기화
    q->front=q->rear=NULL;
    q->count=0;
}

int emptyQ(queue *q){ //q가 비어있는지 확인
    return q->count==0;
}

int selectQ_LRU(queue *q,int data){
    //같은 페이지 스트링 찾기. hit면 rear로 보내고 1리턴. falut면 0리턴
    node *tail;
    node *ptr;
    if(emptyQ(q)){//비어있다면
        return 0;
    }
    tail=q->rear;
    ptr=q->front;
    //hit
    if(ptr->data==data){ //첫 노드가 hit일때
        q->front=ptr->next;
        q->count--;
        insertQ(q,data);
        return 1;
    }
    //두번째 노드부터 확인하기
    if(ptr->next==NULL||ptr->next->next==NULL)
        return 0;
    while(ptr->next->next!=NULL){//리스트 돌면서 hit인지 확인
        if(ptr->next->data==data){//hit이면
            node *cur=ptr->next;
            ptr->next=ptr->next->next;
            q->count--;
            insertQ(q,data); //rear에 추가
            return 1;
        }
        ptr=ptr->next; //다음 노드로 바꾸기
    }
    //fault
    return 0;
}

void insertQ(queue *q,int data){ //q의 rear에 노드 추가
    //노드 생성
    node *new_node=(node *)malloc(sizeof(node));
    new_node->data=data;
    new_node->next=NULL;
    //q가 비어있으면 
    if(emptyQ(q))
        q->front=new_node;
    //q가 비어있지 않으면 rear에 추가
    else
        q->rear->next=new_node;
    q->rear=new_node; //리스트 연결
    q->count++;
}

//리스트에 몇개의 노드들이 있는지 확인
int sizeQ(queue *q){
    return q->count;
}

//front에 있는 노드 삭제
void deleteQ(queue *q){
    node *ptr;
    //비어있다면
    if(emptyQ(q)){
        printf("Error : Queue is empty\n");
        return;
    }
    ptr=q->front;
    q->front=ptr->next;
    free(ptr);
    q->count--;
}

//lfu 알고리즘
void lfu(int algorithm_index){
    int frequent[PAGE_STR+1];//page 참조 횟수 기록
    int fault_count=0;
    int node_index;
    int i,j;
    queue q;
    printAndSave_algorithm_list("LFU",NOT_ESC);
    initQ(&q);//q 초기화
    memset(frequent,0,sizeof(frequent));//page 참조 횟수 기록 초기화
    for(i=0;i<REF_STR;i++){//모든 reference string을 돌면서
        if(!isHit(&q,reference_string[algorithm_index][i])){ //못 찾으면
            fault_count++;//page_fault 카운팅
            if (sizeQ(&q) >= page_frame_size){//page frame이 꽉 찼다면
                node_index=victimQ_LFU(&q,frequent);//victim 찾기
                deleteQ_LFU(&q,node_index);//해당 victim 삭제
            }
            insertQ(&q,reference_string[algorithm_index][i]);//노드 추가
        }
        frequent[reference_string[algorithm_index][i]]++;//page 참조 횟수 기록
        printAndSaveQ(&q,algorithm_index,i);//결과 출력
    }
    printandSave_pageFault(fault_count);//page fault 개수 출력
}

void printAndSaveQ(queue *q,int algorithm_index,int index){
    FILE *fp=NULL;
    char *filename="result.txt";
    node *ptr=q->front;
    node *saveptr=q->front;
    //표준 출력
    printf("%4d : %17d     -  ",index+1,reference_string[algorithm_index][index]);
    //리스트 돌면서 출력
    while(ptr->next!=NULL){
        printf("%-4d ",ptr->data);
        ptr=ptr->next;
    }
    printf("%d\n",ptr->data);
    //파일 오픈
    if((fp=fopen(filename,"a+"))==NULL){
        fprintf(stderr,"file open error\n");
        exit(1);
    }
    //파일 저장
    fprintf(fp, "%4d : %17d     -  ",index+1,reference_string[algorithm_index][index]);
    //리스트 돌면서 저장
    while(saveptr->next!=NULL){
        fprintf(fp,"%-4d ",saveptr->data);
        saveptr=saveptr->next;
    }
    fprintf(fp,"%d\n",saveptr->data);
    fclose(fp);
}

//victim 찾아서 몇번째 노드인지를 리턴
int victimQ_LFU(queue *q,int frequent[]){
    node *ptr=q->front;
    int min_page=ptr->data;
    int i=0;
    int node_index=0;
    ptr=ptr->next;
    i++;
    while(ptr->next!=NULL){//노드들 돌면서
        // printf("fre :%d %d\n", min_page, frequent[min_page]);
        // printf("fre :%d %d\n", ptr->data, frequent[ptr->data]);
        if(frequent[min_page]>frequent[ptr->data]){//가장 작은 frequent이면 그 인덱스 저장
            min_page=ptr->data;
            node_index=i;
        }
        ptr=ptr->next;
        i++;
    }
    // printf("fre :%d %d\n", min_page, frequent[min_page]);
    // printf("fre :%d %d\n", ptr->data, frequent[ptr->data]);
    if(frequent[min_page]>frequent[ptr->data]){ //마지막도 비교. 가장 작은 frequent이면 그 인덱스 저장
            min_page=ptr->data;
            node_index=i;
    }
    return node_index;//리스트에서 victim 몇번째 노드인지 리턴
}

int isHit(queue *q,int data){
    //같은 페이지 스트링 찾기. hit면 1리턴. falut면 0리턴
    node *ptr;
    int node_index=0;
    if(emptyQ(q)){
        return 0;
    }
    ptr=q->front;
    //hit
    while(ptr->next!=NULL){
        if(ptr->data==data){
            deleteQ_LFU(q,node_index);
            insertQ(q,data);
            return 1;
        }
        ptr=ptr->next;
        node_index++;
    }
    if(ptr->data==data){
        deleteQ_LFU(q,node_index);
        insertQ(q,data);
        return 1;
    }
    //fault
    return 0;
}

void deleteQ_LFU(queue *q, int node_index){
    node *ptr;
    if(emptyQ(q)){
        printf("Error : Queue is empty\n");
        return;
    }
    ptr=q->front;
    if(node_index==0){
        q->front=ptr->next;
    }
    else{
        node *preptr=ptr;
        ptr=ptr->next;
        for(int i=0;i<node_index-1;i++){
            preptr=preptr->next;
            ptr=ptr->next;
        }
        preptr->next=preptr->next->next;
        if(preptr->next==NULL)
            q->rear=preptr;
    }
    free(ptr);
    q->count--;
}

//sc 알고리즘
void sc(int algorithm_index){
    int page_frame[PAGE_FRME_MAX];
    int bit_frame[PAGE_FRME_MAX];
    int page_frame_idx=0;
    int fault_count=0;
    int i,j;
    printAndSave_algorithm_list("SC",NOT_ESC);
    memset(page_frame,-1,sizeof(page_frame));//비어있는 page_frame은 -1로 표현
    memset(bit_frame,0,sizeof(bit_frame));//기회 주는 비트 처음에는 0으로 초기화
    for(i=0;i<REF_STR;i++){//모든 reference string을 돌면서
        int replace=1;//교체를 해야하면 1 아니면 0
        for(j=0;j<page_frame_size;j++){//page_frame을 돌면서
            if(page_frame[j]==reference_string[algorithm_index][i]){//Hit인 경우
                bit_frame[j]=1;//한번더 기회주기
                replace=0;
                break;
            }
            else if(page_frame[j]==-1){
                replace=0;
                page_frame[j]=reference_string[algorithm_index][i];//page_frame에 값 넣기
                bit_frame[j]=1;//처음부터 기회를 한번 더주기
                fault_count++;//page_fault 카운팅
                break;
            }
        }
        if(replace){//교체를 해야한다면
            int temp=page_frame_idx; //현재 page frame 포인터
            fault_count++;//page_fault 카운팅
            for(int t=0;t<page_frame_size;t++){//bit_frame이 모두1이면 한바퀴 돌기
                if(temp==page_frame_size){//page frame 맨끝에 도달하면 다시 처음으로
                    temp=0;
                }
                if(bit_frame[temp]==0){//한번더 기회가 없으면 바로 victim 선택
                    break;
                }
                bit_frame[temp]=0;//기회 줬으니깐 0으로 바꾸기
                temp++;
            }
            if(temp==page_frame_size){//page frame 맨끝에 도달하면 다시 처음으로
                temp=0;
            }
            page_frame[temp]=reference_string[algorithm_index][i]; //victim 바꾸기
            bit_frame[temp]=1;//마찬가지로 기회 한번 더
            //현재 page frame 포인터
            page_frame_idx=temp+1;
            page_frame_idx=page_frame_idx%page_frame_size;
        }
        printAndSave_page_frame(page_frame,algorithm_index,i,NOT_ESC);//결과 출력
    }
    printandSave_pageFault(fault_count);//page fault 개수 출력
}

//esc 알고리즘
void esc(int algorithm_index){
    int page_frame[PAGE_FRME_MAX];
    int fault_count=0;
    int i,j;
    printAndSave_algorithm_list("ESC",ESC);
    memset(page_frame,-1,sizeof(page_frame));//비어있는 page_frame은 -1로 표현
    memset(rw_bit,0,sizeof(rw_bit));//rw_bit 0으로 초기화
    for(i=0;i<REF_STR;i++){//모든 reference string을 돌면서
        int replace=1;//교체를 해야하면 1 아니면 0
        int left_bit_off=1;//rw_bit의 왼쪽비트를 꺼야한다면 1 아니면 0

        for(j=0;j<page_frame_size;j++){
            if(page_frame[j]==reference_string[algorithm_index][i]){//Hit인 경우
                replace=0;
                rw_bit[j]=toBit(i);//R면 2,W면 3저장
                break;
            }
            else if(page_frame[j]==-1){//비어있는 경우
                replace=0;
                rw_bit[j]=toBit(i);//R면 2,W면 3저장
                page_frame[j]=reference_string[algorithm_index][i];//page_frame에 값 넣기
                fault_count++;//page_fault 카운팅
                break;
            }
        }

        if(replace){//교체를 해야한다면
            int min_rwbit=4;
            int min_index;
            fault_count++;//page_fault 카운팅

            for (j = 0; j < page_frame_size; j++){ //비트 전부 확인
                if ((rw_bit[j] & 2) == 0){ //왼쪽 비트가 0이 하나라도 있으면 left bit off 할필요 x
                    left_bit_off = 0;
                }
            }
            if (left_bit_off){
                for (j = 0; j < page_frame_size; j++){ //왼쪽 비트 끄기
                    rw_bit[j] &= 1;
                }
            }
            //우선순위 가장 높은거 victim 선택
            //만약 rw_bit가 같은게 여러개라면 page frame에서 가장 앞에 있는 page 선택
            for(int t=0;t<page_frame_size;t++){ 
                if(min_rwbit>rw_bit[t]){
                    min_rwbit=rw_bit[t];
                    min_index=t;
                }
            }
            page_frame[min_index]=reference_string[algorithm_index][i];//victim 골랐던것 page 교체
            rw_bit[min_index]=toBit(i);//rw_bit도 설정
        }
        printAndSave_page_frame(page_frame,algorithm_index,i,ESC);//결과 출력
    }
    printandSave_pageFault(fault_count);//page fault 개수 출력
}

//referece string에서 Read면 2, Write면 3 리턴 
int toBit(int index){
    if(ESC_reference_modify[index]=='R')
        return 2;
    else
        return 3;
}
//페이지 스트링 생성. data_input이 1이면 랜덤 생성 2이면 지정된 파일로부터 읽어 들이기
void init_reference_string(int data_input,int total_algorithm){
    if(data_input==1){
        srand((unsigned int)time(NULL)); //랜덤 시드
        for(int i=0;i<REF_STR;i++){ //reference string 크기만큼 돌면서
            reference_string[0][i]=(int)(rand()%PAGE_STR+1); //랜덤 생성한값 넣기
        }
        for(int i=0;i<REF_STR;i++){ //RW도 랜덤
            if(rand()%2){
                ESC_reference_modify[i]='R'; //reference bit
            }
            else{
                ESC_reference_modify[i]='W'; //dirty bit
            }
        }
        for(int i=1;i<total_algorithm;i++){
            memcpy(reference_string[i],reference_string[0],sizeof(reference_string[0]));
        }
        print_reference_string(0,ESC);
    }
    else{
        FILE *stream_fp=NULL;
        FILE *bit_fp=NULL;
        char *stream_filename="samplestream20.txt";
        char *bit_filename="samplebit20.txt";
        char streambuf[BUFMAX];
        char bitbuf[BUFMAX];
        int stream_argc;
        int bit_argc;
        char *stream_argv[FILE_ARGMAX];
        char *bit_argv[FILE_ARGMAX];
        //읽어들일 파일 두개 오픈
        if((stream_fp=fopen(stream_filename,"r"))==NULL||(bit_fp=fopen(bit_filename,"r"))==NULL){
            fprintf(stderr,"file open error\n");
            exit(1);
        }
        //버퍼에 모두 저장
        fread(streambuf,1,BUFMAX,stream_fp);
        fread(bitbuf,1,BUFMAX,bit_fp);
        //토큰 분리
        stream_argc=split(streambuf," ",stream_argv);
        bit_argc=split(bitbuf," ",bit_argv);
        //reference_string에 저장
        for(int i=0;i<stream_argc;i++){
            reference_string[0][i]=atoi(stream_argv[i]);
        }
        //ESC에 사용할 Read, write 저장
        for(int i=0;i<bit_argc;i++){
            ESC_reference_modify[i]=bit_argv[i][0];
        }

        for(int i=1;i<total_algorithm;i++){
            memcpy(reference_string[i],reference_string[0],sizeof(reference_string[0]));
        }
        print_reference_string(0,ESC); //reference string 출력
        fclose(stream_fp);
        fclose(bit_fp);
    }
}

//사용자가 몇개의 알고리즘을 선택했는지 확인하여 그 개수를 리턴하는 함수
int count_algorithm(int selected_algorithm[]){
    int count=0;
    for(int i=0;i<ALGOMAX;i++)
        if(selected_algorithm[i]==1) 
            count++;
    return count;
}

//해당 알고리즘의 reference_string을 출력하는 함수. isESC가 ESC이면 RW도 출력
void print_reference_string(int index,int isESC){
    FILE *fp=NULL;
    char *filename="result.txt";
    
    printf("-----Reference string-----\n");
    for(int i=0;i<REF_STR;i++){
        if(isESC) {
            if(ESC_reference_modify[i]=='R')
                printf("%dR ",reference_string[index][i]);
            else
                printf("%dW ",reference_string[index][i]);
        }
        else printf("%d ",reference_string[index][i]);
    }
    printf("\n--------------------------\n");

    //파일 오픈
    if((fp=fopen(filename,"a+"))==NULL){
        fprintf(stderr,"file open error\n");
        exit(1);
    }
    fprintf(fp,"-----Reference string-----\n");
    for(int i=0;i<REF_STR;i++){
        if(isESC) {
            if(ESC_reference_modify[i]=='R')
                fprintf(fp,"%dR ",reference_string[index][i]);
            else
                fprintf(fp,"%dW ",reference_string[index][i]);
        }
        else fprintf(fp,"%d ",reference_string[index][i]);
    }
    fprintf(fp,"\n--------------------------\n");
    
    fclose(fp);
}

//string에 있는 내용을 seperator를 기준으로 토큰 분리해서 argv에 담는 함수
int split(char *string, char *seperator, char *argv[]){
    int argc=0;
    char *ptr=NULL;

    ptr=strtok(string,seperator); //seprator를 기준으로 
    while(ptr!=NULL){
        argv[argc++]=ptr;
        ptr=strtok(NULL," ");
    }

    return argc;
}

