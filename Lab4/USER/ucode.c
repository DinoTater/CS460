// ucode.c file
#include "type.h"

char *cmd[]={"getpid", "ps", "chname", "kmode", "switch", "wait",
             "exit", "fork", "exec", 0};

int show_menu()
{
   printf("******************* ALL STAR MENU *******************\n");
   printf("*****************************************************\n");
   printf("*     **     *     **     *     **     *     **     *\n");
   printf("*                                                   *\n");
   printf("* ps  chname  kmode  switch  wait  exit  fork  exec *\n");
   printf("*                                                   *\n");
   printf("*     **     *     **     *     **     *     **     *\n");
   printf("*****************************************************\n");
   printf("*****************************************************\n");
}

int find_cmd(char *name)
{
   int i;   char *p;
   i = 0;   p = cmd[0];
   while (p){
         if (strcmp(p, name)==0)
            return i;
         i++;  p = cmd[i];
   } 
   return(-1);
}


int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   syscall(1, 0, 0);
}

int chname()
{
    char s[64];
    printf("input new name : ");
    gets(s);
    syscall(2, s, 0);
}

int kmode()
{
    printf("kmode : enter Kmode via INT 80\n");
    printf("proc %d going K mode ....\n", getpid());
        syscall(3, 0, 0);
    printf("proc %d back from Kernel\n", getpid());
}    

int kswitch()
{
    printf("proc %d enter Kernel to switch proc\n", getpid());
        syscall(4,0,0);
    printf("proc %d back from Kernel\n", getpid());
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n");
    return child; 
} 

int exit()
{
   char exitValue;
   printf("enter an exitValue (0-9) : ");
   exitValue=(getc()&0x7F) - '0';
   printf("enter kernel to die with exitValue=%d\n",exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  syscall(6,exitValue,0);
}

int getc()
{
    return syscall(90, 0, 0) & 0x7F;
}

int putc(char c)
{
    return syscall(91, c, 0, 0);
}

int fork()
{
  return syscall(7,0,0,0);
}

int exec(char *s)
{
  return syscall(8,s,0,0);
}

int copyImage (u16 pseg, u16 cseg, u16 size)
{
  u16 i;
  for (i=0; i<size; i++)
    put_word(get_word(pseg, 2*i), cseg, 2*i);
}

int kfork()
{
  int pid; u16 segment;
  PROC *p = kfork(0);
  if (p==0) return -1;
  segment = (p->pid+1)*0x1000;
  copyImage(running->uss, segment, 32*1024);
  p->uss = segment;
  p->usp = running->usp;
  put_word(segment, segment, p->usp);
  put_word(segment, segment, p->usp+2);
  put_word(0, segment, p->usp+2*8);
  put_word(segment, segment, p->usp+2*10);
  return p->pid;
}

int ufork()
{
  int child = fork();
  (child) ? printf("parent ") : printf ("child ")
  printf("%d return from fork, child_pid=%d\n", getpid(), child);
}

int uexec()
{
  int r; char filename[64];
  printf("enter exec filename : ");
  gets(filename);
  r = exec(filename);
  printf("exec failed\n");
}

int kexec()
{
  int i=0, length = 0;
  char filename[64];
  u16 segment = running->uss;
  while((*cp++ = get_byte(running->uss, i++)) && length++ < 64);
  if(!load(filename, segment))
    return -1;
  for (i=1; i<=12; i++)
  {
    put_word(0, segment, -2*i);
  }
  running->usp=-24;
  put_word(segment, segment, -24);
  put_word(segment, segment, -22);
  put_word(segment, segment, -4);
  put_word(0x0200, segment, -2);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}
