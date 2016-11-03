/************** syscall routing table ***********/
int kcinth() 
{
  u16 x, y, z, w, r; 
  u16 seg, off;

  seg = running->uss; off = running->usp;

  x = get_word(seg, off+13*2);
  y = get_word(seg, off+14*2);
  z = get_word(seg, off+15*2);
  w = get_word(seg, off+16*2);
  
   switch(x){
       case 0 : r = running->pid;    break;
       case 1 : r = kps();           break;
       case 2 : r = chname(y);       break;
       case 3 : r = kmode();         break;
       case 4 : r = tswitch();       break;
       case 5 : r = kwait();         break;
       case 6 : r = kexit();         break;
       case 7 : r = fork();          break;
       case 8 : r = kexec(y);        break;


       // FOCUS on ksin() nd ksout()
       case 9 : r = ksout(y);        break;
       case 10: r = ksin(y);         break;

       case 99: r = kexit();         break;

       default: printf("invalid syscall # : %d\n", x);

   }
   put_word(r, seg, off+2*8);
}


int ksin(char *y)
{
  // get a line from serial port 0; write line to Umode
  int len;
  int i = 0;
  char in[64];

  len = sgetline(0, in);

  do
  {
    put_byte(in[i++], running->uss, y++);
  } while (in[i] != '\0');

  return len;
}

int ksout(char *y)
{
  // get line from Umode; write line to serial port 0
  int i = 0;
  short c;
  char out[64];
  do
  {
    c = get_byte(running->uss, y);
    out[i] = c;
    y++;
    i++;
  } while (c != '\0');

  sputline(0, out);
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

int kkmode()
{
    body();
}

int kexec(char* y)
{
    int i = 0, length = 0;
    char filename[64];
    char *cp = filename;
    char *file;
    int start = *cp;

    
    // same segment
    u16 segment = running->uss; 
    
    //get filename from U space with a length limit of 64
    while ((*cp++ = get_byte(running->uss, y++)) && length++ < 64);
    
    printf("filename: %s\n", filename);
    if (!load(filename, segment))
        return -1;
    
    // put the bytes in their right location!
    for( i = 1; i <= 12; i++)
    {
        put_word(0, segment, -2 * i);
    }
    
    running->usp = -24;

    // -1    -2  -3 -4 -5 -6 -7 -8 -9 -10 -11 -12
    // flag uCS uPC ax bx cx dx bp si di  uES  uDS 
    put_word(segment, segment, (-2 * 12)); // saved uDS=segment
    put_word(segment, segment, (-2 * 11)); // saved uES=segment
    put_word(segment, segment, (-2 * 2));  // uCS=segment; uPC=0
    put_word(0x0200, segment,  (-2 * 1));  // Umode flag=0x0200
}