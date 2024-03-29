#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

//20182601
#define TIME_SLICE 10000000     //time_slice
#define NULL ((void *)0)        //널 포인터(임시로 포인터 초기화 할 때 NULL로 초기화하기 위함)

int weight=1; //weight는 1부터 시작(weight갑싱 0이면 error처리 sysproc.c 97번째 줄)

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
  long min_priority; //관리하고 있는 프로세스의 priority 값 중 가장 작은 값을 부여하기 위해 필요 //20182601
} ptable;

//20182601
//ssu_schedule : 실행할 프로세스 찾기(priority가 가장 작은 프로세스). 프로세스의 proc 구조체 정보는 ret 변수에 넣어 ret 리턴
struct proc *ssu_schedule(){ 
  struct proc *p; //순회할 프로세스들
  struct proc *ret=NULL; //순회한 프로세스들 중 최소 priority를 가질 값
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) { //실행가능한 프로세스들을 찾기 위해 ptable 순회하면서 ret값 찾기
    if (p->state == RUNNABLE) { //실행 가능한 프로세스만
      if (ret == NULL || (p->priority<ret->priority)) 
        ret = p;
    }
  }
  //xv6 빌드 시 "debug=1" 매개변수 전달이 오면, 다음 내용 출력
  #ifdef DEBUG
    if (ret) //실행될 프로세스 정보 출력
      cprintf("PID: %d, NAME: %s, WEIGHT: %d, PRIORITY: %d\n", ret->pid, ret->name, ret->weight, ret->priority);
  #endif
    return ret; //실행할 프로세스 proc 구조체 리턴
}

//인자로 들어온 proc 구조체의 priority 업데이트 => new_priority=old_priority+(TIME_SLICE/weight)
void update_priority(struct proc *proc){
  proc->priority = proc->priority + (TIME_SLICE/proc->weight);
}

// 실행가능한 프로세스들 중에서 가장 작은 우선순위 갖는 프로세스를 선택하여 그 priority를 ptable의 min_priority에 대입(갱신)
// 나중에 assign_min_priority 함수를 실행하기 위해 필요
void update_min_priority(){
  struct proc *min=NULL; //순회한 프로세스들 중 최소 priority를 가질 값
  struct proc *p; //순회할 프로세스들
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++){ //ptable 순회하면서 실행 가능한 프로세스들 중에서 가장 작은 priority값 찾기
    if (p->state == RUNNABLE){ //실행 가능한 프로세스만
      if (min == NULL || (p->priority<min->priority))
        min = p;
    }
  }
  //min이 NULL이 아니면 ptable의 min_priority 갱신
  if(min!=NULL)
    ptable.min_priority=min->priority;
}

// 프로세스 생성 또는 wake up 시 priority 값을 0부터 부여하게 되면
// 해당 프로세스가 독점 실행될 수 있으므로 관리하고 있는 프로세스의 priority 값 중 가장 작은 값을 부여
// 인자로 proc을 넣으면 그 proc의 priority 값을 갱신
void assign_min_priority(struct proc *proc){
   proc->priority = ptable.min_priority;
}

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  //20182601
  //weight 값은 프로세스 생성 순서에 따라 1부터 차례대로 증가시키며 부여
  p->weight=weight;
  weight++;

  p->state = EMBRYO;
  p->pid = nextpid++;

  //20182601
  assign_min_priority(p); //관리하고 있는 프로세스의 priority 값 중 가장 작은 값을 부여

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  //20182601
  ptable.min_priority=3;

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  // 부모프로세스는 아마 wait()함수를 호출해서 UNUSED(sleep) 상태이기 때문에 깨워준다.
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  // 버려진 자식을 init에 전달(좀비 프로세스 관리)
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
// 자식프로세스가 종료될때까지 부모프로세스 기다리기
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
// 가장 낮은 priority를 가진 프로세스를 탐색하고 실행
// context swtiching 하기
//20182601
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu(); //현재 cpu에 있는 레지스터 정보 가져오기
  c->proc = 0; //proc 초기화
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    // 프로세스간 상호배제를 위한 lock 설정
    acquire(&ptable.lock);

    p=ssu_schedule(); //가장 낮은 priority를 가진 프로세스를 탐색
    //실행할 프로세스가 없다면 lock 해제하고, for문 continue
    if(p==NULL){
      release(&ptable.lock);
      continue;
    }
    // Switch to chosen process.  It is the process's job
    // to release ptable.lock and then reacquire it
    // before jumping back to us.
    // context swtiching 하기
    // 레지스터에 실행할 프로세스 정보 입력
    c->proc = p; 
    switchuvm(p);
    p->state = RUNNING; //프로세스 상태 Runnable에서 Running으로 변경

    // 스택에 현재 레지스터를 저장하고 구조체 컨텍스트를 생성하고 해당 주소를 &(c->scheduler)에 저장
    // 스택을 새 레지스터로 전환하고 이전에 저장한 레지스터를 팝
    swtch(&(c->scheduler), p->context); 
    switchkvm();

    update_priority(p); // 우선순위 갱신
    update_min_priority(); // 실행가능한 프로세스들 중에서 가장 작은 우선순위 갖는 프로세스를 선택하여 그 priority를 ptable의 min_priority에 대입(갱신)

    // Process is done running for now.
    // It should have changed its p->state before coming back.
    c->proc = 0;
    release(&ptable.lock); //lock 해제
  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.up1
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
// SLEEPING 상태인 프로세스들을 모두 RUNNABLE 상태로 바꿈.
// 여러 경우중 자식프로세스의 exit()함수가 호출될 때 wakeup1() 함수가 호출되고 그러면서 부모프로세스의 상태를 RUNNABLE로 바꾸는 케이스가 있다.
// 프로세스가 exit() 함수를 호출할 때 해당 프로세스가 종료되고,
// 다음 실행될 프로세스를 찾아야 하기 때문에 상태를 새로 업데이트 해줘야함.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == SLEEPING && p->chan == chan){
      p->state = RUNNABLE;
      //20182601
      assign_min_priority(p); //프로세스의 priority 값 중 가장 작은 값을 부여
    }
  }
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

//20182601
// 인자 weight 값을 현재 실행중인 프로세스->weight에 저장
void do_weightset(int weight){
  acquire(&ptable.lock); //프로세스간 상호배제를 위한 lock 설정
  myproc()->weight=weight; //현재 실행중인 프로세스->weight에 인자 weight값 저장
  release(&ptable.lock); //lock 해제
}