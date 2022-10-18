#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//20182601
// sdebug 명령어에 의해 생성되는 프로세스의 weight 값 부여
int sys_weightset(void){
  int weight; // 인자로 입력 받을 weight
  
  if(argint(0,&weight)<0||weight==0) //인자 에러처리(0값이 들어와도 에러처리)
    return -1;
  do_weightset(weight); // 인자 weight 값을 현재 실행중인 프로세스->weight에 저장
  return 1;
}

//20182601
// sdebug 명령어에 의해 생성되는 프로세스 정보 출력(PID,WEIGHT,TIMES)
int sys_procinfo(void){
  int time; //인자로 받을 시간
  struct proc *p=myproc(); //현재 실행중인 프로세스 정보
  if(argint(0,&time)<0) //인자 에러처리
    return -1;
  cprintf("PID: %d, WEIGHT: %d, TIMES : %d ms\n",p->pid,p->weight,time); // 정보 출력
  return 1; 
}