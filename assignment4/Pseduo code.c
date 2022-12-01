page_frame[page_frame_size];
reference_string[reference_string_size];

void optimal(){
    page_frame[page_frame_size];
    reference_string[reference_string_size];
    page_fault_count=0;
    for(i : reference_string 돌면서){
        bool replace=true;
        for(j : page_frame 돌면서){
            page_frame[j]==reference_string[i]이면 Hit이므로 replace=false;
            page_frame[j]가 비어있으면 {
                replace=false;
                page_frame[j]에 reference_string[i]값 넣기;
                page_fault_count++;
            }
        }
        replace가 true이면{
            long_time[page_frame_size];
            page_fault_count++;
            for(k : page_frame_size 돌면서){
                for(t : 다음 reference_string 인덱스부터 끝까지 돌면서){
                    page_frame[k]와 reference_string[t]가 같으면 탈출.
                }
                long_time[k]=t; //다음에 나오는게 얼마나 걸리는지 체크
            }
            for(k : page_frame_size 돌면서){
                for(t : 다음 reference_string 인덱스부터 끝까지 돌면서){
                    victim=가장 오래 걸리는 인덱스 찾기
                }
            }
            page_frame[victim]=reference_string[i]; //교체
        }
    }
}

void fifo(){
    page_frame[page_frame_size];
    reference_string[reference_string_size];
    page_fault_count=0;
    page_frame_idx=0;
    for(i : reference_string 돌면서){
        bool replace=true;
        for(j : page_frame 돌면서){
            page_frame[j]==reference_string[i]이면 Hit이므로 replace=false;
            page_frame[j]가 비어있으면 {
                replace=false;
                page_frame[j]에 reference_string[i]값 넣기;
                page_fault_count++;
            }
        }
        replace가 true이면{
            page_fault_count++;
            page_frame[page_frame_idx%page_frame_size]=reference_string[i]; //교체
            page_frame_idx++;
        }
    }
}

void lifo(){
    page_frame[page_frame_size];
    reference_string[reference_string_size];
    page_fault_count=0;
    page_frame_idx=0;
    for(i : reference_string 돌면서){
        bool replace=true;
        for(j : page_frame 돌면서){
            page_frame[j]==reference_string[i]이면 Hit이므로 replace=false;
            page_frame[j]가 비어있으면 {
                replace=false;
                page_frame[j]에 reference_string[i]값 넣기;
                page_fault_count++;
            }
        }
        replace가 true이면{
            page_fault_count++;
            page_frame 마지막 index에 reference_string[i]값 넣기; 
        }
    }
}

void lru(){
    page_frame 연결리스트 생성;
    reference_string[reference_string_size];
    page_fault_count=0;
    for(i : reference_string 돌면서){
        if(같은 페이지가 없다면){
            page_fault_count++;
            if(page_frame이 꽉 찼다면)
                가장 오래된 페이지 page_frame에서 삭제;//연결리스트 front 부분 삭제
            page_frame에 reference_string[i] 리스트 연결;//연결리스트 rear 부분 추가
        }
    }
}

void lfu(){
    frequent[];//page 참조 횟수 기록
    page_frame 연결리스트 생성;
    reference_string[reference_string_size];
    page_fault_count=0;
    for(i : reference_string 돌면서){
        if(같은 페이지가 없다면. 만약 히트가 났다면 해당 페이지를 연결리스트 rear부분으로 옮겨주기){
            page_fault_count++;
            if(page_frame이 꽉 찼다면){
                victim 찾기(
                    1.참조횟수가 가장 적은 페이지
                    2.참조횟수가 가장 적은 페이지가 여러개일 
                      경우 가장 오랫동안 사용하지 않은 페이지(연결리스트의 rear부분)
                );
                리스트에서 victim 삭제;
                리스트에 reference_string[i] 추가;//rear부분에 추가
            }
            page_frame에 reference_string[i] 리스트 연결;//연결리스트 rear 부분 추가
        }
        frequent[reference_string[i]]++;//page 참조 횟수 기록
    }
}

void sc(){
    page_frame[page_frame_size];
    bit_frame[page_frame_size];
    reference_string[reference_string_size];
    page_fault_count=0;
    page_frame_idx=0;
    for(i : reference_string 돌면서){
        bool replace=true;
        for(j : page_frame 돌면서){
            page_frame[j]==reference_string[i]이면 {
                Hit이므로 replace=false;
                bit_frame[j]=1;//한번더 기회주기
            }
            page_frame[j]가 비어있으면 {
                replace=false;
                page_frame[j]에 reference_string[i]값 넣기;
                bit_frame[j]=1;//한번더 기회주기
                page_fault_count++;
            }
        }
        replace가 true이면{
            page_fault_count++;
            page_frame_idx이 가리키고 있는 곳부터 시작해서
            해당 페이지의 bit_frame이 0이라면 기회 없이 바로 교체.
            해당 페이지의 bit_frame이 1이라면 0으로 바꿔주고 다음 페이지 탐색
            (이 과정을 해당 페이지의 bit_frame이 0이거나 한바퀴 돌때까지 진행)
            교체한 페이지는 bit_frame[temp]=1;//마찬가지로 기회 한번 더 주기
            바꾼 페이지 다음을 page_frame_idx가 가리키도록 변경.
        }
    }
}

void esc(){
    page_frame[page_frame_size];
    rw_bit[page_frame_size];
    reference_string[reference_string_size];
    page_fault_count=0;
    for(i : reference_string 돌면서){
        bool replace=true;
        for(j : page_frame 돌면서){
            page_frame[j]==reference_string[i]이면 {
                Hit이므로 replace=false;
                rw_bit[j]에 reference_string이 modify가 있으면 3, 없으면 2 저장.
            }
            page_frame[j]가 비어있으면 {
                replace=false;
                page_frame[j]에 reference_string[i]값 넣기;
                rw_bit[j]에 reference_string이 modify가 있으면 3, 없으면 2 저장.
                page_fault_count++;
            }
        }
        replace가 true이면{
            page_fault_count++;
            page frame에 rw_bit를 확인해서 왼쪽 비트가 
            전부 1이라면 왼쪽 비트 0으로 바꾸기.
            victim 찾기 -> rw_bit가 가장 작은 인덱스.
            만약 rw_bit가 가장 작은 것들이 여러개라면 
            page frame에서 index가 가장 작은 page를 선택
            victim을 reference_string[i]로 교체.
            rw_bit[victim_index]에 reference_string이 modify가 있으면 3, 없으면 2 저장.
        }
    }
}