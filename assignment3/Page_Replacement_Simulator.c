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
#define BUFMAX 1024
#define PAGE_STR 30
#define REF_STR 20
#define NOT_ESC 0
#define ESC 1

// #ifndef max
// #define max(a,b) (((a) > (b)) ? (a) : (b))
// #endif

typedef struct node{
    int data;
    struct node *next;
} node;

typedef struct queue{
    node *front;
    node *rear;
    int count;
} queue;

int split(char *string, char *seperator, char *argv[]);
void init_reference_string(int data_input,int total_algorithm);
int count_algorithm(int selected_algorithm[]);
void print_reference_string(int index,int isESC);
void page_replacement(int selected_algorithm[]);
void optimal(int algorithm_index);
void fifo(int algorithm_index);
void lifo(int algorithm_index);
void lru(int algorithm_index);
void lfu(int algorithm_index);
void sc(int algorithm_index);
void esc(int algorithm_index);
void initQ(queue *q);
int emptyQ(queue *q);
int selectQ(queue *q,int data);
void insertQ(queue *q,int data);
int sizeQ(queue *q);
void deleteQ(queue *q);

int reference_string[ALGOMAX][REF_STR];
char ESC_reference_modify[REF_STR]; //ESC R,W(D) 표시
int page_frame_size;

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
    
    while(1){
        printf("A. Page Replacement 알고리즘 시뮬레이터를 선택하시오 (최대 3개)\n");
        printf("(1) Optimal   (2) FIFO   (3) LIFO   (4) LRU   (5) LFU   (6) SC   (7) ESC   (8) All\n");
        int A_exception=0;
        memset(selected_algorithm,0,sizeof(selected_algorithm));
        fgets(input,sizeof(input),stdin);
        input[strlen(input)-1]='\0';
        A_argc=split(input," ",A_argv);
        //2개 이상인데 8을 선택했을 경우
        for(int i=0;i<A_argc;i++){
            if((A_argc>1&&!strcmp(A_argv[i],"8"))||(atoi(A_argv[i])<1||atoi(A_argv[i])>8))
                A_exception=1;
        }
        if(A_argc>3||A_argc==0||A_exception){
            printf("input error.\nUsage : 1 2 5\n      | 6\n      | 2 4\n      | 8\n");
            continue;
        }
        if(!strcmp(A_argv[0],"8")){
            for(int i=0;i<7;i++)
                selected_algorithm[i]=1;
            break;
        }
        for(int i=0;i<A_argc;i++){
            selected_algorithm[atoi(A_argv[i])-1]=1;
        }
        break;
    }
   
    while(1){
        printf("B. 페이지 프레임의 개수를 입력하시오.(3~10)\n");
        fgets(input,sizeof(input),stdin);
        input[strlen(input)-1]='\0';
        B_argc=split(input," ",B_argv);
        if(B_argc!=1){
            printf("input error\n");
            continue;
        }
        page_frame_size=atoi(B_argv[0]);
        if(page_frame_size<3||page_frame_size>10){
            printf("input error. Usage : page frame is between 3 and 10\n");
            continue;
        }
        break;
    }

    while(1){
        printf("C. 데이터 입력 방식을 선택하시오.(1,2)\n");
        fgets(input,sizeof(input),stdin);
        input[strlen(input)-1]='\0';
        C_argc=split(input," ",C_argv);
        if(C_argc!=1||strcmp(C_argv[0],"1")&&strcmp(C_argv[0],"2")){
            printf("input error\n");
            continue;
        }
        data_input=atoi(C_argv[0]);
        break;
    }

    total_algorithm=count_algorithm(selected_algorithm);
    init_reference_string(data_input,total_algorithm);
    page_replacement(selected_algorithm);

    printf("D. 종료\n");
    exit(0);
}

void page_replacement(int selected_algorithm[]){
    int algorithm_index=0;
    printf("selected_algorithm : %d\n",selected_algorithm[1]);
    if(selected_algorithm[0]) optimal(algorithm_index++);
    if(selected_algorithm[1]) fifo(algorithm_index++);
    if(selected_algorithm[2]) lifo(algorithm_index++);
    if(selected_algorithm[3]) lru(algorithm_index++);
    if(selected_algorithm[4]) lfu(algorithm_index++);
    if(selected_algorithm[5]) sc(algorithm_index++);
    if(selected_algorithm[6]) esc(algorithm_index++);
}

void optimal(int algorithm_index){
    int page_frame[PAGE_FRME_MAX];
    int fault_count=0;
    int i,j;
    memset(page_frame,-1,sizeof(page_frame));
    for(i=0;i<REF_STR;i++){
        int replace=1;
        for(j=0;j<page_frame_size;j++){
            if(page_frame[j]==reference_string[algorithm_index][i]){
                replace=0;
            }
            else if(page_frame[j]==-1){
                replace=0;
                page_frame[j]=reference_string[algorithm_index][i];
                fault_count++;
                break;
            }
        }
        if(replace){
            int long_time[PAGE_FRME_MAX];
            int max_time=-1;
            int max_index=0;
            int k,t;
            fault_count++;
            for(k=0;k<page_frame_size;k++){
                for(t=i+1;t<REF_STR;t++){
                    if (page_frame[k] == reference_string[algorithm_index][t])
                        break;
                }
                long_time[k]=t;
            }
            for(k=0;k<page_frame_size;k++){
                if(long_time[k]>max_time){
                    max_time=long_time[k];
                    max_index=k;
                }
            }
            page_frame[max_index]=reference_string[algorithm_index][i];
        }
        // for(int i=0;i<3;i++){
        //     printf("%d ", page_frame[i]);
        // }
        // printf("\n");
    }
    // for(int i=0;i<3;i++){
    //         printf("%d ", page_frame[i]);
    //     }
    //     printf("\n");
}

void fifo(int algorithm_index){
    int page_frame[PAGE_FRME_MAX];
    int page_frame_idx=0;
    int fault_count=0;
    int i,j;
    memset(page_frame,-1,sizeof(page_frame));
    for(i=0;i<REF_STR;i++){
        int replace=1;
        for(j=0;j<page_frame_size;j++){
            if(page_frame[j]==reference_string[algorithm_index][i]){
                replace=0;
            }
            else if(page_frame[j]==-1){
                replace=0;
                page_frame[j]=reference_string[algorithm_index][i];
                fault_count++;
                break;
            }
        }
        if(replace){
            fault_count++;
            page_frame[page_frame_idx%page_frame_size]=reference_string[algorithm_index][i];
            page_frame_idx++;
        }
        // for(int i=0;i<3;i++){
        //     printf("%d ", page_frame[i]);
        // }
        // printf("\n");
    }
    // for(int i=0;i<3;i++){
    //         printf("%d ", page_frame[i]);
    //     }
    //     printf("\n");
}

void lifo(int algorithm_index){
    int page_frame[PAGE_FRME_MAX];
    int page_frame_idx=0;
    int fault_count=0;
    int i,j;
    memset(page_frame,-1,sizeof(page_frame));
    for(i=0;i<REF_STR;i++){
        int replace=1;
        for(j=0;j<page_frame_size;j++){
            if(page_frame[j]==reference_string[algorithm_index][i]){
                replace=0;
            }
            else if(page_frame[j]==-1){
                replace=0;
                page_frame[j]=reference_string[algorithm_index][i];
                fault_count++;
                break;
            }
        }
        if(replace){
            fault_count++;
            page_frame[page_frame_size-(page_frame_idx%page_frame_size)-1]=reference_string[algorithm_index][i];
            page_frame_idx++;
        }
        // for(int i=0;i<3;i++){
        //     printf("%d ", page_frame[i]);
        // }
        // printf("\n");
    }
    // for(int i=0;i<3;i++){
    //         printf("%d ", page_frame[i]);
    //     }
    //     printf("\n");
}

void lru(int algorithm_index){
    int fault_count=0;
    int i,j;
    queue q;
    initQ(&q);
    for(i=0;i<REF_STR;i++){
        if(!selectQ(&q,reference_string[algorithm_index][i])){ //못 찾으면
            fault_count++;
            if (sizeQ(&q) >= page_frame_size)
                deleteQ(&q);
            insertQ(&q,reference_string[algorithm_index][i]);
        }
        // if(i>1) 
        //     printf("%d : %d %d %d\n",reference_string[algorithm_index][i],q.front->data,q.front->next->data,q.rear->data);
        // else if(i==0)
        //     printf("%d : %d\n",reference_string[algorithm_index][i],q.front->data);
        // else if(i==1)
        //     printf("%d : %d %d\n",reference_string[algorithm_index][i],q.front->data,q.front->next->data);
    }
}

void initQ(queue *q){
    q->front=q->rear=NULL;
    q->count=0;
}

int emptyQ(queue *q){
    return q->count==0;
}

int selectQ(queue *q,int data){ //***hit면 rear로 가야됨***
    //같은 페이지 스트링 찾기. hit면 rear로 보내고 1리턴. falut면 0리턴
    node *tail;
    node *ptr;
    if(emptyQ(q)){
        return 0;
    }
    tail=q->rear;
    ptr=q->front;
    //hit
    if(ptr->data==data){
        q->front=ptr->next;
        q->count--;
        insertQ(q,data);
        return 1;
    }
    if(ptr->next==NULL||ptr->next->next==NULL)
        return 0;
    while(ptr->next->next!=NULL){
        if(ptr->next->data==data){
            node *cur=ptr->next;
            ptr->next=ptr->next->next;
            q->count--;
            insertQ(q,data);
            return 1;
        }
        ptr=ptr->next;
    }
    //fault
    return 0;
}

void insertQ(queue *q,int data){
    node *new_node=(node *)malloc(sizeof(node));
    new_node->data=data;
    new_node->next=NULL;
    
    if(emptyQ(q))
        q->front=new_node;
    else
        q->rear->next=new_node;
    q->rear=new_node;
    q->count++;
}

int sizeQ(queue *q){
    return q->count;
}

void deleteQ(queue *q){
    node *ptr;
    if(emptyQ(q)){
        printf("Error : Queue is empty\n");
        return;
    }
    ptr=q->front;
    q->front=ptr->next;
    free(ptr);
    q->count--;
}

void lfu(int algorithm_index){
    int fault_count=0;
    int i,j;
    queue q;
    initQ(&q);
    for(i=0;i<REF_STR;i++){
        if(!selectQ(&q,reference_string[algorithm_index][i])){ //못 찾으면
            fault_count++;
            if (sizeQ(&q) >= page_frame_size)
                deleteQ(&q);
            insertQ(&q,reference_string[algorithm_index][i]);
        }
        // if(i>1) 
        //     printf("%d : %d %d %d\n",reference_string[algorithm_index][i],q.front->data,q.front->next->data,q.rear->data);
        // else if(i==0)
        //     printf("%d : %d\n",reference_string[algorithm_index][i],q.front->data);
        // else if(i==1)
        //     printf("%d : %d %d\n",reference_string[algorithm_index][i],q.front->data,q.front->next->data);
    }
}

void sc(int algorithm_index){
    
}

void esc(int algorithm_index){
    
}

void init_reference_string(int data_input,int total_algorithm){
    if(data_input==1){
        srand((unsigned int)time(NULL));
        for(int i=0;i<REF_STR;i++){
            reference_string[0][i]=(int)(rand()%PAGE_STR+1);
        }
        for(int i=0;i<REF_STR;i++){
            if(rand()%2){
                ESC_reference_modify[i]='r'; //reference bit
            }
            else{
                ESC_reference_modify[i]='d'; //dirty bit
            }
        }
        for(int i=1;i<total_algorithm;i++){
            memcpy(reference_string[i],reference_string[0],sizeof(reference_string[0]));
        }
        print_reference_string(0,ESC);
        }
    else{
        //생성된 파일 입력을 받는 모듈
        reference_string[0][0]=7;
        reference_string[0][1]=0;
        reference_string[0][2]=1;
        reference_string[0][3]=2;
        reference_string[0][4]=0;
        reference_string[0][5]=3;
        reference_string[0][6]=0;
        reference_string[0][7]=4;
        reference_string[0][8]=2;
        reference_string[0][9]=3;
        reference_string[0][10]=0;
        reference_string[0][11]=3;
        reference_string[0][12]=2;
        reference_string[0][13]=1;
        reference_string[0][14]=2;
        reference_string[0][15]=0;
        reference_string[0][16]=1;
        reference_string[0][17]=7;
        reference_string[0][18]=0;
        reference_string[0][19]=1;
        for(int i=1;i<total_algorithm;i++){
            memcpy(reference_string[i],reference_string[0],sizeof(reference_string[0]));
        }
        print_reference_string(0,ESC);
    }
}

int count_algorithm(int selected_algorithm[]){
    int count=0;
    for(int i=0;i<ALGOMAX;i++)
        if(selected_algorithm[i]==1) 
            count++;
    return count;
}

void print_reference_string(int index,int isESC){
    for(int i=0;i<REF_STR;i++){
        if(ESC) {
            if(ESC_reference_modify[i]=='r')
                printf("%dR ",reference_string[index][i]);
            else
                printf("%dW(D) ",reference_string[index][i]);
        }
        else printf("%d ",reference_string[index][i]);
    }
    printf("\n");
}

int split(char *string, char *seperator, char *argv[]){
    int argc=0;
    char *ptr=NULL;

    ptr=strtok(string,seperator);
    while(ptr!=NULL){
        argv[argc++]=ptr;
        ptr=strtok(NULL," ");
    }

    return argc;
}

