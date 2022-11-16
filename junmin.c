#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#define max 1024
#define FILE_NAME 100
#define STR_REF_NUM 500

// void OPT_algorithm();
// void FIFO_algorithm();
//void LIFO_algorithm();
// void LRU_algorithm();
//void LFU_algorithm();
//void SecondChance_algorithm();
//void EnhanceSecondChance_algorithm();
void selected_replace_algorithm(char *argv_1[]);
int split(char *string, char *seperator, char *argv[]);
// int CheckExist (int *frame_buf, int frame_cnt, int refer_string);
// int CalcFrameLongest (int idx, int element_cnt, int frame_element, int *refer_buf);

char ReferenceString[STR_REF_NUM];

int main (void) {
    
    int total_algorithm;
   char input_algorithm[max];

   /* ------ 페이지 프레임 변수 ------ */
   FILE *fp;
   char fname[FILE_NAME];
    int frame_cnt = 0; // page frame 저장 변수
   int refer_buf[31] = { 0, }; // page reference string를 정수형으로 저장할 배열
   int element_cnt = 0; //정수형 배열의 인덱스 카운터 변수

    /* ------ 시뮬레이터 선택 변수 ------ */
    int argc_1;
    char *argv_1[10];
    int exception = 0;
    int selected[max];

    /* ------ frame 선택 변수 ------ */
    int argc_2;
    char *argv_2[10];
    int page_frame_size;

   /* ------ 데이터 입력 방식 ------ */
    int argc_3;
    char *argv_3[10];
    int input_frame_data;


   if((fp=fopen(fname, "r")) == NULL){
      fprintf(stderr, "Open error for %s\n",fname);
      exit(1);
   }

    fread(fname, sizeof(fname), 1, fp); // input.txt 읽는 작업
   fclose(fp); //file close

   
   /* ------ used method ------ */
    while(1){
    exception = 0;
    memset(selected,0,sizeof(selected));   
   printf("A.Choose the Page Replacement Algorithm simulation (max:3) :\n");
    printf("(1) Optimal (2) FIFO (3) LIFO (4) LRU (5) LFU (6) SC (7) ESC (8) All\n");
   printf("write only number not use ()\n");

    fgets(input_algorithm,sizeof(input_algorithm),stdin);
        input_algorithm[strlen(input_algorithm)-1] = '\0';
        argc_1 = split(input_algorithm," ",argv_1);
    
    for(int i=0; i<argc_1; i++){
        if((argc_1>1 && !strcmp(argv_1[i],"8")) || (atoi(argv_1[i])<1 || atoi(argv_1[i])>8))
        exception = 1;
    }
    if(argc_1>3 || argc_1==0 || exception){
      printf("Input error\n Usag : Choose the Page Replacement Algorithm simulation (max:3) \n");
      continue;
   }

    if(!strcmp(argv_1[0], "8")){
        for(int i = 0; i<7; i++) selected[i] = 1;
        break;
    }
    for(int i = 0; i<argc_1; i++){
        selected[atoi(argv_1[i])-1] = 1;
    }
    break;
    }

while(1){
    printf("B.Put the frame number.(3~10)\n");

    fgets(input_algorithm,sizeof(input_algorithm),stdin);
        input_algorithm[strlen(input_algorithm)-1]='\0';
        argc_2 = split(input_algorithm," ",argv_2);
      
        if(argc_2!=1){
            printf("Input error\n Usag: frame number(3~10)\n");
            continue;
        }
        page_frame_size = atoi(argv_2[0]);
        if(page_frame_size <3 || page_frame_size > 10){
         printf("Input error\n Usag: Put the frame number.(3~10)\n");
            continue;
        }
        break;
}

 while(1){
        printf("C.Choose the method of data input.(1,2)\n");

        fgets(input_algorithm,sizeof(input_algorithm),stdin);
        input_algorithm[strlen(input_algorithm) -1 ]='\0';
        argc_3=split(input_algorithm," ",argv_3);

        if(argc_3!=1||strcmp(argv_3[0],"1")&&strcmp(argv_3[0],"2")){
            printf("Input error \n Usag:Choose the method of data input.(1,2)\n");
            continue;
        }
        input_frame_data=atoi(argv_3[0]);
        break;
    }

    if(input_frame_data == 1){ // rand() 함수 사용
    

    }

    else {
    char *ptr = strtok(fname, "\n"); //frame_cnt & page reference string 토큰화
   frame_cnt = page_frame_size; //frame_cnt int
   
   ptr = strtok(NULL, "\n");
   strncpy(ReferenceString, ptr, strlen(ptr)); //page reference string 저장
   int temp = strlen(ReferenceString);
   char *intptr = strtok(ReferenceString, " "); //공백자를 기준으로 토큰화(문자열 배열 -> 정수형 배열)
   
   while(intptr != NULL) {
      refer_buf[element_cnt] = atoi(intptr);
      intptr = strtok(NULL, " ");
      element_cnt++;
       } 

    }

    printf("D.END\n");
    exit(0);

    // selected_replace_algorithm(argv_1);

}

/* ------ 옵션에 따른 알고리즘 기법 적용 ------ */
// void selected_replace_algorithm(char *argv_1[]){

//    // if (strcmp(argv_1[0], "1") == 0) {
//    //    //OPT_algorithm();
//    // } // OPT

//    // if (strcmp(argv_1[0], "2") == 0) {
//    //    FIFO_algorithm();
//    // } // FIFO

//     // if (strcmp(argv_1[0], "3")== 0){
//     //     LIFO_algorithm();
//     // }// LIFO

//    // if (strcmp(argv_1[0], "4") == 0) {
//    //    LRU_algorithm();
//    // } // LRU

//     if (strcmp(argv_1[0], "5") == 0){
//         LFU_algorithm();
//     }// LFU

//    if (strcmp(argv_1[0], "6") == 0) {
//       SecondChance_algorithm();
//    } // Second-Chance
    
//     if (strcmp(argv_1[0], "7") == 0){
//         EnhanceSecondChance_algorithm();
//     } // Enhance-Second-Chance


//     if (strcmp(argv_1[0], "8") == 0){

//     } // All

//    else {
//       printf("You can use only <OPT, FIFO, LIFO, LRU, LFU, SC, ESC>\n");
//    }
// }

// /* ------ Optimal ------ */
// void Optimal()
// {
//     int no_of_frames, no_of_pages, frames[10], pages[30], temp[10], flag1, flag2, flag3, i, j, k, pos, max_count, faults = 0;
//     printf("Enter number of frames: ");
//     scanf("%d", &no_of_frames);
    
//     printf("Enter number of pages: ");
//     scanf("%d", &no_of_pages);
    
//     printf("Enter page reference string: ");
    
//     for(i = 0; i < no_of_pages; ++i){
//         scanf("%d", &pages[i]);
//     }
    
//     for(i = 0; i < no_of_frames; ++i){
//         frames[i] = -1;
//     }
    
//     for(i = 0; i < no_of_pages; ++i){
//         flag1 = flag2 = 0;
        
//         for(j = 0; j < no_of_frames; ++j){
//             if(frames[j] == pages[i]){
//                    flag1 = flag2 = 1;
//                    break;
//                }
//         }
        
//         if(flag1 == 0){
//             for(j = 0; j < no_of_frames; ++j){
//                 if(frames[j] == -1){
//                     faults++;
//                     frames[j] = pages[i];
//                     flag2 = 1;
//                     break;
//                 }
//             }    
//         }
        
//         if(flag2 == 0){
//          flag3 =0;
        
//             for(j = 0; j < no_of_frames; ++j){
//              temp[j] = -1;
            
//                 for(k = i + 1; k < no_of_pages; ++k){
//                 if(frames[j] == pages[k]){
//                    temp[j] = k;
//                    break;
//                 }
//             }
//         }
            
//             for(j = 0; j < no_of_frames; ++j){
//                 if(temp[j] == -1){
//                 pos = j;
//                 flag3 = 1;
//                 break;
//             }
//         }
            
//             if(flag3 ==0){
//              max_count = temp[0];
//              pos = 0;
            
//              for(j = 1; j < no_of_frames; ++j){
//              if(temp[j] > max){
//              max_count = temp[j];
//              pos = j;
//              }
//          }            
//             }
// frames[pos] = pages[i];
// faults++;
//         }        
//         printf("\n");
        
//         for(j = 0; j < no_of_frames; ++j){
//             printf("%d\t", frames[j]);
//         }
//     }
    
//     printf("\n\nTotal Page Faults = %d", faults);   
//     return 0;
// }

// /* ------ FIFO ------ */
// void FIFO(){
//    int referenceString[10], pageFaults = 0, m, n, s, pages, frames;
//    printf("\n What are the total number of frames:\t");
// {
//    scanf("%d", &frames);
// }
//    printf("\nEnter the number of Pages:\t");
//    scanf("%d", &pages);
//    printf("\nEnter reference string values:\n");

//    for( m = 0; m < pages; m++){

//    printf("Value No. [%d]:\t", m + 1);

//    scanf("%d", &referenceString[m]);
// }
//    int temp[frames];
//    for(m = 0; m < frames; m++){
//      temp[m] = -1;
// }

//    for(m = 0; m < pages; m++){
//      s = 0;
//      for(n = 0; n < frames; n++){
//       if(referenceString[m] == temp[n])
//          {
//             s++;
//             pageFaults--;
//          }
//    }     
//    pageFaults++;
//    if((pageFaults <= frames) && (s == 0))
//       {
//         temp[m] = referenceString[m];
//       }   
//    else if(s == 0)
//       {
//         temp[(pageFaults - 1) % frames] = referenceString[m];
//       }
//       printf("\n");
//       for(n = 0; n < frames; n++)
//        {     
//          printf("%d\t", temp[n]);
//        }
// } 
//    printf("\nTotal Page Faults:\t%d\n", pageFaults);
//    return 0;
// }

// void LRU()
// {
//     int no_of_frames, no_of_pages, frames[10], pages[30], counter = 0, time[10], flag1, flag2, i, j, pos, faults = 0;
//    printf("Enter number of frames: ");
//    scanf("%d", &no_of_frames);
//    printf("Enter number of pages: ");
//    scanf("%d", &no_of_pages);
//    printf("Enter reference string: ");

//     for(i = 0; i < no_of_pages; ++i){
//      scanf("%d", &pages[i]);
//     }
    
//    for(i = 0; i < no_of_frames; ++i){
//      frames[i] = -1;
//     }
    
//     for(i = 0; i < no_of_pages; ++i){
//      flag1 = flag2 = 0;
    
//      for(j = 0; j < no_of_frames; ++j){
     
//     if(frames[j] == pages[i]){
//     counter++;
//      time[j] = counter;
//        flag1 = flag2 = 1;

//      break;
//    }
//         }    
//      if(flag1 == 0){
//    for(j = 0; j < no_of_frames; ++j){
//         if(frames[j] == -1){
//         counter++;
//         faults++;
//         frames[j] = pages[i];
//         time[j] = counter;
//         flag2 = 1;
//      break;
//            }
//         }
//      }
    
//      if(flag2 == 0){
//      pos = findLRU(time, no_of_frames);
//      counter++;
//      faults++;
//      frames[pos] = pages[i];
//      time[pos] = counter;
//     }
    
//      printf("\n");
    
//      for(j = 0; j < no_of_frames; ++j){
//      printf("%d\t", frames[j]);
//      }
// }
// printf("\n\nTotal Page Faults = %d", faults);
    
//     return 0;
// }


/* ------ argc / argv function ------ */
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

// /* ------ For use LRU ------ */
// int findLRU(int time[], int n){
// int i, minimum = time[0], pos = 0;
 
// for(i = 1; i < n; ++i){
// if(time[i] < minimum){
// minimum = time[i];
// pos = i;
//     }
// }
// return pos;
//     }