int do_tswitch()
{
  printf("proc %d tswitch()\n", running->pid);
  tswitch();
  printf("proc %d resumes\n", running->pid);
}

int do_kfork()
{
  PROC *p;
  printf("proc%d kfork a child\n");
  p = kfork("/u1");
  if (p == 0)
    printf("kfork failed\n");
  else
    printf("child pid = %d\n", p->pid);
}

int do_exit(int exitValue)
{
  //int exitValue;
  if (running->pid == 1 && nproc > 2){
      printf("other procs still exist, P1 can't die yet !%c\n",007);
      return -1;
  }

  kexit(exitValue);
}

int do_wait(int *ustatus)
{
  int child, status;
  child = kwait(&status);
  if (child<0){
    printf("proc %d wait error : no child\n", running->pid);
    return -1;
  }
  printf("proc %d found a ZOMBIE child %d exitValue=%d\n", 
	   running->pid, child, status);
  // write status to Umode *ustatus
  put_word(status, running->uss, ustatus);
  return child;
}

int body()
{
  char c;
  printf("proc %d resumes to body()\n", running->pid);

  while(1){
    printf("-----------------------------------------\n");
    printList("freelist  ", freeList);
    printList("readyQueue", readyQueue);
    printList("sleepList ", sleepList);
    printf("-----------------------------------------\n");

    printf("proc %d running: parent = %d  enter a char [s|f|w|q|u] : ", 
	   running->pid, running->parent->pid);
    c = getc(); printf("%c\n", c);
    switch(c){
       case 's' : do_tswitch();   break;
       case 'f' : do_kfork();     break;
       case 'w' : do_wait();      break;
       case 'q' : do_exit();      break;
       case 'u' : goUmode();      break;
    }
  }
}

int color;
extern int loader();

extern PROC proc[];
int kmode()
{
  body();
}

char *hh[ ] = {"FREE   ", "READY  ", "RUNNING", "STOPPED", "SLEEP  ", 
               "ZOMBIE ",  0}; 
int kps()
{
    //WRITE C code to print PROC information
    int i, count;
    bool printinfo = false;
    bool printevent = false;
    bool printexitcode = false;
    PROC *p;
    
    printf("***********************************************************************\n");
    printf("   name       status       pid       ppid       event       exitcode\n");
    printf("***********************************************************************\n");
    
    for (i = 0; i < NPROC; i++)
    {
        printinfo = true;
        printevent = false;
        printexitcode = false;
        p = &proc[i];
        count = 14 - strlen(p->name);
        printf("%s", p->name);
        while (count-- > 0)
            printf(" ");
        
        // write the status and set the information variables
        switch (p->status)
        {
            case FREE:
                printf("FREE         ");
                printinfo = false;
                printexitcode = true;
                break;
            case READY:
                printf("READY        ");
                break;
            case RUNNING:
                printf("RUNNING      ");
                break;
            case STOPPED:
                printf("STOPPED      ");
                printexitcode = true;
                break;
            case SLEEP:
                printf("SLEEP        ");
                printevent = true;
                break;
            case ZOMBIE:
                printf("ZOMBIE       ");
                printevent = true;
                break;
        }
        
        // show pid and ppid?
        if (printinfo == true)
            printf("%d         %d         ", p->pid, p->ppid);
        else
            printf("                    ");
            
        // show event num?
        if (printevent == true)
            printf("%d             ", p->event);
        else
            printf("              ");
        
        // show exit code?
        if (printexitcode == true)
            printf("%d\n", p->exitCode);
        else
            printf("\n");
    }
    printf("***********************************************************************\n");
    return true;
}

#define NAMELEN 32
int chname(char *y)
{
  char buf[64];
  char *cp = buf;
  int count = 0; 

  while (count < NAMELEN){
     *cp = get_byte(running->uss, y);
     if (*cp == 0) break;
     cp++; y++; count++;
  }
  buf[31] = 0;

  printf("changing name of proc %d to %s\n", running->pid, buf);
  strcpy(running->name, buf); 
  printf("done\n");
}
