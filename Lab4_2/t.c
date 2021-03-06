#include "type.h"

#include "kernel.c"
#include "int.c"
#include "wait.c"

/**************************************************
  bio.o, queue.o loader.o are in mtxlib
**************************************************/
/* #include "bio.c" */
/* #include "queue.c" */
/* #include "loader.c" */
// initialize

int init()
{
    PROC *p;
    int i;
    
    color = 0x0C;
    printf("init ....\n");
    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);
	    printf("proc[%d]=%s.\n", i, proc[i].name);
        p->next = &proc[i+1];
    }
    
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
}

int body()
{
    int i, out;
    char c;
    printf("Proc[%d] resuming to body()\n", running->pid);
    running->status = RUNNING;
    while(1)
    {
        printf("============================================\n");
        printList("freelist  ", freeList);
        printList("readyQueue", readyQueue);
        printList("sleepList ", sleepList);
        printf("============================================\n");

	printf("proc %d running: priority = %d parent = %d\n", running->pid, running->priority, running->ppid);

        shorthelp();
        c = getc(); printf("%c\n", c);
        
        for (i = 0; commands[i].key; i++)
        {
            if (c == commands[i].key)
            {
                printf("Executing command %s...\n", commands[i].name);
                out = commands[i].f();
                printf("\nCommand finished!\n\n");
            }
        }
    }
}

// prints the help screen
int help()
{
    int i;
    printf("\nAvailable Commands: \n");
    for (i = 0; commands[i].key; i++)
    {
        printf(" - %c: %s: %s\n", commands[i].key, commands[i].name, commands[i].help);
    }
    return 1;
}

// prints the list of commands
int shorthelp()
{
    int i;
    printf("Enter a char [");
    
    for (i = 0; commands[i].key; i++)
    {
        printf(" %c ", commands[i].key);
        if (commands[i + 1].key != NULL)
        {
            printf("|");
        }
    }
    
    printf("] : ");
    return 1;
}

int scheduler()
{
    if (running->status == READY)
        enqueue(&readyQueue, running);
     running = dequeue(&readyQueue);
     color = 0x000A + (running->pid % 6);
}

int int80h();
int set_vector(u16 vector, u16 handler)
{
  // put_word(word, segment, offset) in mtxlib
     put_word(handler, 0, vector<<2);
     put_word(0x1000,  0,(vector<<2) + 2);
}
            
main()
{
    printf("\nMTX starts in main()\n");
    init();      // initialize and create P0 as running
    set_vector(80, int80h);

    kfork("/bin/u1");     // P0 kfork() P1

    while(1)
    {
        printf("P0 running\n");
        while(!readyQueue);
        printf("P0 switch process\n\n");
        tswitch();         // P0 switch to run P1
    }
}
