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

#ifdef GETPPID
int
sys_getppid(void)
{
    int ppid = 1;

    if (myproc()->parent) {
        ppid = myproc()->parent->pid;
    }
    return ppid;
}
#endif 

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

#ifdef KDEBUG
int
sys_kdebug(void)
{
    extern uint debugState;

    int val = 0;

    if (argint(0, &val) < 0)
        return -1;
    debugState = val;

    cprintf("%s %s %d: set debugState = %d\n"
            , __FILE__, __FUNCTION__, __LINE__, val);

    return 0;
}
#endif // KDEBUG

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

int
sys_halt(void)
{
    outb(0xf4, 0x00);
    return 0;
}

// #ifdef KTHREADS
// # error You need to complete the following sys_kthread_ functions.

int
sys_kthread_create(void)
{
  void(*fcn)(void*);
	void *arg;
	void *tstack;

	if(argptr(0, (void*)&fcn, sizeof(fcn) < 0))
		return -1;

	if(argptr(1, (void*)&arg, sizeof(arg) < 0))
			return -1;

	if(argptr(2, (void*)&tstack, sizeof(tstack) < 0))
			return -1;

  // argptr(int n, char **pp, int size)
  // if ((argptr(0, (void*)&buf, sizeof(*buf)) < 0) || (argint(1, (int*)&size) < 0))
  // kthread_create(void (*func)(void*), void *arg_ptr, void *tstack)

  return kthread_create(fcn, arg, tstack);

}

int
sys_kthread_join(void)
{
  int tid;
  if(argint(0, &tid) < 0){
    return -1;
  }

  return kthread_join(tid);
}

int
sys_kthread_exit(void)
{
  int exitVal;
  if(argint(0, &exitVal) < 0){
    return -1;
  }

  kthread_exit(exitVal);
  return 0;
}

// #endif // KTHREADS
