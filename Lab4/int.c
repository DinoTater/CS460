
#define PA 13
#define PB 14
#define PC 15
#define PD 16
#define AX  8
 
int kcinth()
{
   u16    segment, offset;
   int    a,b,c,d, r;

   segment = running->uss; 
   offset = running->usp;

   a = get_word(segment, offset + 2*PA);
   b = get_word(segment, offset + 2*PB);
   c = get_word(segment, offset + 2*PC);
   d = get_word(segment, offset + 2*PD);

   switch(a){
       case 0 : r = running->pid;     break;
       case 1 : r = kps();          break;
       case 2 : r = chname(b);        break;
       case 3 : r = kmode();          break;
       case 4 : r = tswitch();        break;
       case 5 : r = do_wait(b);       break;
       case 6 : r = do_exit(b);       break;
        
       case 7 : r = fork();           break;
       case 8 : r = exec(b);          break;
       
       case 90: r =  getc();          break;
       case 91: color=running->pid+11;
                r =  putc(b);         break;       
       case 99: do_exit(b);           break;
       default: printf("invalid syscall # : %d\n", a); 
   }
   put_word(r, segment, offset + 2*AX);
}

int kgetpid()
{
    //WRITE YOUR C code
  return running->pid;
}

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

int kchname(char *name)
{
    //WRITE C CODE to change running's name string;
    char c;
    int i = 0;

    while (i < 32)
    {
        c = get_byte(running->uss, name + i);
        running->name[i] = c;
        if (c == '\0') { break; }
        i++;
    }
}

int kkfork()
{
  return kfork();
}

int ktswitch()
{
    return tswitch();
}

int kkwait(int *status)
{
    int pid, c;
    pid = kwait(&c);
    put_word(c, running->uss, status);
    return pid;
}

int kkexit(int value)
{
    kexit(value);
}

int get_word(u16 segment, u16 offset)
{
  u16 word;
  setds(segment);
  word = *(u16 *)offset;
  setds(0x1000);
  return word;
}

int put_byte(u8 byte, u16 segment, u16 offset)
{
    setds(segment);
    *(u8 *)offset = byte;
    setds(0x1000);
}
