#include "type.h"

/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/
int kcinth()
{
   	int a,b,c,d, r;
//==> WRITE CODE TO GET get syscall parameters a,b,c,d from ustack 
    a = get_word(running->uss, running->usp + 26);
    b = get_word(running->uss, running->usp + 28);
    c = get_word(running->uss, running->usp + 30);
    d = get_word(running->uss, running->usp + 32);
	
   switch(a)
   {
       case 0 : r = kgetpid();        break;
       case 1 : r = kps();            break;
       case 2 : r = kchname(b);       break;
       case 3 : r = kkmode();         break;
       case 4 : r = ktswitch();       break;
       case 5 : r = kkwait(b);        break;
       case 6 : r = kkexit(b);        break;
       case 7 : r = kkfork(b);         break;
       case 8 : r = kexec(b);         break;

       case 90: r = getc();           break;
       case 91: r = putc(b);          break;
       case 99: kkexit(b);            break;
       default: printf("invalid syscall # : %d\n", a); 
   }

//==> WRITE CODE to let r be the return value to Umode
    put_word(r, running->uss, running->usp + 16);
    return r;
}

//============= WRITE C CODE FOR syscall functions ======================

int kkmode()
{
    body();
}

int kexec(char* y)
{
	int length = 0;
	int j = 0;
	int k = 0;
	int i;
	int offset = 0;
	char command[100];
	int found_file = 0;

	char file[64];
	char *cp = command;

	
	// same segment
	u16 segment = running->uss; 
	
	/* get filename from U space with a length limit of 64 */
	while ((*cp++ = get_byte(running->uss, y++)) && length++ < 100)
	{
		if (!found_file)
		{
			file[j] = command[j];

			//Parse on space
			if (command[j] == ' ')
			{
				found_file = 1;
				file[j] = 0;
			}
		}
		j++;
	}
	// Null Terminator 
	command[j] = 0;
	length++;
	
	// put the bytes in their right location!
	for( k = 0; k < length; k++)
	{
		put_byte(command[k], segment, (length - k) *-1 );
	}
	
	// Make string even
	if (length % 2 != 0)
	{
		put_byte(42, segment, (++length) *-1);
	}

	// place the pointer to the string in the stack
	put_word( (-1*length ), segment, (-1*(length+2)));
	
	// load filename to segment
	if (!load(file, segment)) 
	{
		return -1;
	}

	/* re-initialize process ustack for it return to VA=0 */
	for (i = 1; i <= 12; i++)
		put_word(0, segment, (-2 * i) - (2+length));

	running->usp = - 24 - ( 2+length); // new usp = -24
	/* -1 -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 ustack layout 
	
	/* flag uCS uPC ax bx cx dx bp si di uES uDS  */
	put_word(segment, segment, (-2 * 12) - ( 2 + length )); // saved uDS=segment
	put_word(segment, segment, (-2 * 11) - (2 + length )); // saved uES=segment
	put_word(segment, segment, (-2 * 2) - (2 + length )); // uCS=segment; uPC=0
	put_word(0x0200, segment,  (-2 * 1) - (2 + length )); // Umode flag=0x0200 
	
}

int fork()
{
	int pid; u16 segment;
	PROC *p = kfork(0); // kfork() a child, do not load image file
	if (p == 0) return -1; // kfork failed
	segment = (p->pid + 1) * 0x1000; // child segment
	copyImage(running->uss, segment, 32 * 1024); // copy 32K words
	p->uss = segment; // child’s own segment
	p->usp = running->usp; // same as parent's usp
						   //*** change uDS, uES, uCS, AX in child's ustack ****
	put_word(segment, segment, p->usp); // uDS=segment
	put_word(segment, segment, p->usp + 2); // uES=segment
	put_word(0, segment, p->usp + 2 * 8); // uax=0
	put_word(segment, segment, p->usp + 2 * 10); // uCS=segment
	return p->pid;
}

int copyImage(u16 pseg, u16 cseg, u16 size)
{
	u16 i;
	for (i = 0; i < size; i++) {
		put_word(get_word(pseg, 2 * i), cseg, 2 * i);
	}
}

// returns the current process id
int kgetpid()
{
    return running->pid;
}

// WRITE C code to print PROC information
int kps()
{
    int i, count;
    bool showmoreinfo = false;
    bool showevent = false;
    bool showexitcode = false;
    PROC *p;
    
    printf("=======================================================================\n");
    printf("   name       status       pid       ppid       event       exitcode\n");
    printf("-----------------------------------------------------------------------\n");
    
    for (i = 0; i < NPROC; i++)
    {   // initialize all procs
        showmoreinfo = true;
        showevent = false;
        showexitcode = false;
        p = &proc[i];
        count = 14 - strlen(p->name);
        printf("%s", p->name);
        while (count-- > 0)
            printf(" ");
        
        // write the status and set the information vars!
        switch (p->status)
        {
            case FREE:
                printf("FREE         ");
                showmoreinfo = false;
                showexitcode = true;
                break;
            case READY:
                printf("READY        ");
                break;
            case RUNNING:
                printf("RUNNING      ");
                break;
            case STOPPED:
                printf("STOPPED      ");
                showexitcode = true;
                break;
            case SLEEP:
                printf("SLEEP        ");
                showevent = true;
                break;
            case ZOMBIE:
                printf("ZOMBIE       ");
                showevent = true;
                break;
        }
        
        // show pid and ppid?
        if (showmoreinfo == true)
            printf("%d         %d         ", p->pid, p->ppid);
        else
            printf("                    ");
            
        // show event num?
        if (showevent == true)
            printf("%d             ", p->event);
        else
            printf("              ");
        
        // show exit code?
        if (showexitcode == true)
            printf("%d\n", p->exitCode);
        else
            printf("\n");
    }
    printf("-----------------------------------------------------------------------\n");
    return true;
}

// changes a process name
int kchname(char *name)
{
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

// forks a process
int kkfork()
{
    return kfork("/bin/u1");
}

// switches a process
int ktswitch()
{
    int i;
    running->status = READY;
    i = tswitch();
    running->status = RUNNING;
    
    return i;
}

// waits a process
int kkwait(int *status)
{
    int pid, c;
    pid = kwait(&c);
    put_word(c, running->uss, status);
    return pid;
}

// exits a process
int kkexit(int value)
{
    // use your kexit() in LAB3. do NOT let P1 die
    return kexit(value);
}

// gets a string and returns it!
char* getstring(char *str)
{
    // vars
    char temp[INPUTBUFFER];
    int i;
	u16 segment = (running->pid +1) * 0x1000;
    
    // while we've not yet reached the end of the string, cycle...
    for (i = 0; i < INPUTBUFFER; i++)
    {
        // get the next char and make sure we've not reached the end of the string.
        temp[i] = get_byte(segment, str + i);
        if (temp[i] == '\0')
            break;
    }
    
    // make sure that we end on a nullchar and return the string
    temp[i] = '\0';
    return temp;
}

