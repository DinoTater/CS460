  // ucode.c file
int color;

char *cmd[]={"getpid", "ps", "chname", "kmode", "switch", "wait", "exit", "fork", "exec", 0};

#define LEN 64

int show_menu()
{
   printf("********************** Menu **************************\n");
   printf("* ps  chname  kmode  switch  wait  exec  fork  exit *\n");
   printf("******************************************************\n");
}

int find_cmd(char *name)
{
  // return command index
    int i;

    for (i = 0; cmd[i]; i++)
    {
        if (!strcmp(cmd[i], name))
            return i;
    }
    
    return -1;
}

int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
  printf("kernel: ps...\n");
   return syscall(1, 0, 0);
}

int chname()
{
    char s[32];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int fork()
{   
  int child, pid;
  pid = getpid();
  printf("kernel: fork child\n");
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(7, 0, 0, 0);
  printf("proc %d kforked a child %d\n", pid, child);
  return child;
}    

int kswitch()
{
  printf("kernel: switch...\n");
  return syscall(4,0,0);
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
} 

int geti()
{
  // return an input integer
  char s[16];
  return atoi(gets(s));
}

int exit()
{
   int exitValue;
   printf("enter an exitValue: ");
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  return syscall(6,exitValue,0);
}

int getc()
{
    return syscall(90, 0, 0) & 0x7F;
}

int putc(char c)
{
    return syscall(91, c, 0, 0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}

int kmode()
{
  printf("Proc[%d] going into KMode ....\n", getpid());
      syscall(3, 0, 0);
  printf("Proc[%d] back from the Kernel!\n", getpid());
}

int exec(char *s) { return syscall(8,s,0,0); }

int uexec()
{
    int r, i = 0;
    char filename[64], temp[64];
    printf("enter exec filename : ");
    gets(filename);
/*
    while (filename[i] != ' ' || filename[i] != '\0')
    {
        temp[i] = filename[i];
        if (*cp == ' ' || *cp == '\0')
        {
            // null terminate
            temp[i] = 0;
            tempP = &temp[start];
            argv[argc] = tempP;
            // increment argc and start index
            argc++;
            start = i + 1;  
        }
        cp++;
        i++;

    }
*/
//    printf("entered filename is %s\n", filename);
    r = exec(filename);
    printf("exec failed\n");
}