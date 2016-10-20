#include "type.h"

// function to create a process DYNAMICALLY
int kfork(char *filename)
{
    // Variables
    int i;
    u16 segment;
    
    // Get the new proc
    PROC *p = get_proc(&freeList);
    if (p == 0)
    {
        // if there were no procs, report kfork's failure
        printf("No available free procs!\n");
        return 0;
    }
    
    // initialize the proc...
    p->status = READY;
    p->priority = 1;
    p->ppid = running->pid;
    p->parent = running;
    for (i = 1; i < 12; i++)
        p->kstack[SSIZE - i] = 0;
    
   p->kstack[SSIZE - 1] = (int)body; // now we need to make sure to call tswitch from body when the proc runs...
    p->ksp = &(p->kstack[SSIZE - 11]); // set the ksp to point to the top of the stack
    p->kstack[SSIZE-11] = 0x1000;
	p->kstack[SSIZE-10] = 0x1000;


    // set it up to load umode properly
    segment = (p->pid + 1) * 0x1000;
    printf("segment = %d\n", segment);
    if (filename)
    {
    	load(filename, segment);
    }

    put_word(segment, segment, segment-24);
    put_word(segment, segment, segment-22);
    for(i = 3; i < 11; i++)
        put_word(0,segment, segment-(i * 2));
    
    put_word(segment, segment, segment-4);
    put_word(0, segment, segment-2);

    p->uss = segment;
    p->usp = segment-24;
    printf("p->uss = %d\n", p->uss);
    printf("p->usp = %d\n", p->usp);
    
    // enter the proc into the readyQueue, since it's now ready
    enqueue(&readyQueue, p);
    printf("Proc[%d] forked child Proc[%d] at segment %x\n", running->pid, p->pid, segment);
    // return the new proc
    return p->pid;
}

bool kcopy(u16 parent, u16 child, u16 size)
{
    int i;
    
    for (i = 0; i < size; i += 2)
        put_word(get_word(parent, i), child, i);
    
    return true;
}

u8 get_byte(u16 segment, u16 offset){
 	 u8 byte;
 	 setds(segment);
 	 byte = *(u8 *)offset;
 	 setds(MTXSEG);
 	 return byte;
}

int get_word(u16 segment, u16 offset){
	u16 word;
	setds(segment);
	word = *(u16 *)offset;
	setds(MTXSEG);
	return word;
}

int put_byte(u8 byte, u16 segment, u16 offset){
  	setds(segment);
  	*(u8 *)offset = byte;
  	setds(MTXSEG);
}

int put_word(u16 word, u16 segment, u16 offset){
	setds(segment);
	*(u16 *)offset = word;
	setds(MTXSEG);
}

////////////////////// "do" functions ///////////////

// fork
int do_kfork()
{
    kfork();
}

// fork new file
int do_kforkcustom()
{
    char filename[INPUTBUFFER];
    printf("Filename? ");
    gets(filename);
    kfork(filename);
}

// exit
int do_exit()
{
	char exitval[INPUTBUFFER];
	printf("Exit value? ");
	gets(exitval);
	kexit(atoi(exitval));
}

// wait
int do_wait()
{
	int pid, status;
	pid = kwait(&status);
	printf("pid = %d, exitVal = %d\n", pid, status);
}

// switch
int do_switch()
{
    ktswitch();
}

// umode
int do_umode()
{
    goUmode();
}

