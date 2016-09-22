//                     kernel.c file:

//             YOUR kernel.c file as in LAB3
//
//	     MODIFY kfork() to

////////MODIFY KFORK TO HANDLE FILENAME /////////////////
PROC *kfork(char *filename)  // create a child process, begin from body()
{
	// Variables
    int i, segment;
    
    // Get the new proc
    PROC *p = get_proc(&freeList);
    if (p == 0)
    {
        // if there were no procs, report kfork's failure
        printf("No available free procs\n");
        return 0;
    }
    nproc++;
    // initialize the proc...
    p->status = READY; // it must be ready to run...
    p->priority = 1; // it has no particular preference on when to run...
    p->ppid = running->pid; // its parent is the current processor, of course!
    p->parent = RUNNING;
    
    // setup kstack
    // clear registers by setting them to 0.
    for (i = 1; i<12; i++)
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
    return p;
}

int do_kfork()
{
	PROC *p = kfork();
	if (p == 0)
	{
		printf("kfork failed\n");
		return -1;
	}
	printf("PROC %d kfork a child %d\n", running->pid, p->pid);
	return p->pid;
}

int do_tswitch()
{
	tswitch();
}

int do_umode()
{
	goUmode();
}

int do_exit()
{
	int event;
	printf ("Enter exit code: ");
	event = geti();
	printf ("%d\n", event);
	kexit(event);
}

int do_stop()
{

}

int do_continue()
{

}

int do_sleep()
{
	int event;
	printf ("Enter sleep code: ");
	event = geti();
	sleep(event);
}

int do_wakeup()
{
	int event;
	printf ("Enter wakeup code: ");
	event = geti();
	kwakeup(event);
}

int reschedule ()
{
	PROC *p, *tempQ = 0;
	while ( (p=dequeue(&readyQueue)))	// reorder readyQueue
	{
		enqueue(&tempQ, p);
	}

	readyQueue = tempQ;
	rflag = 0;		// global reschedule flag
	if (running->priority < readyQueue->priority)
		rflag = 1;
}

int chpriority (int pid, int pri)
{
	PROC *p; int i, ok = 0, reQ = 0;
	if (pid == running->pid)
	{
		running->priority = pri;
		if (pri < readyQueue->priority)
		{
			rflag = 1;
		}
		return 1;
	}
	// if not for running, for both READY and SLEEP procs
	for (i = 1; i<NPROC; i++)
	{
		p = &proc[i];
		if (p->pid == pid && p->status != FREE)
		{
			p->priority = pri;
			ok = 1;
			if (p->status == READY)
				reQ = 1;
		}
	}
	if (!ok)
	{
		printf ("chpriority failed\n");
		return -1;
	}
	if (reQ)
		reschedule(p);
}

int do_chpriority()
{
	int pid, pri;
	printf("input pid ");
	pid = geti();
	printf("input new priority ");
	pri = geti();
	if (pri < 1) pri = 1;
	chpriority(pid, pri);
}

int body()
{
	char c;
	while(1)
	{
		if (rflag)
		{
			printf("proc %d: reschedule\n", running->pid);
			rflag = 0;
			tswitch();
		}
		printList("freelist", freeList);
		printList("readyQueue", readyQueue);
		printf("proc%d running: priority = %d parent = %d enter a char [s | f | w | q | u]: ", running->pid, running->priority, running->ppid);
		c = getc();
		printf("%c\n", c);
		switch(c)
		{
			case 'f' : do_kfork();		break;
			case 's' : do_tswitch();	break;
			case 'w' : do_wait();		break;
			case 'q' : do_exit();		break;
			case 'u' : goUmode();		break;
			default: printf("invalid command\n"); break;
		}
	}
}

int geti()
{
	char s[32];
	gets(s);
	return atoi(s);
}

void enqueue(PROC **queue, PROC *p)
{
	PROC *current = *queue, *next;
	// Empty queue
	if ((*queue) == NULL)
	{
		*queue = p;
		(*queue)->next = NULL;
	}

	// new proc goes at head of queue
	else if (p->priority > (*queue)->priority)
	{
		p->next = (*queue);
		(*queue) = p;
	}

	// new process inserted somewhere within queue according to priority
	else
	{
		current = (*queue);
		next = current->next;

		while (next != NULL && p->priority <= next->priority)
		{
			current = next;
			next = current->next;
		}

		current->next = p;
		p->next = next;
	}
}

PROC *dequeue (PROC **queue)
{
	// find correct process
	PROC *p = *queue;

	if (*queue != NULL)
		*queue = (*queue)->next;

	return p;
}

void printList(char *name, PROC *queue)
{
    PROC *p;
    
    // check for empty queue
    if (queue == NULL)
    {
    	printf("%s: ", name);
        printf("There are no processes in this queue.\n");
    }
    // if not empty, print the queue!
    else
    {
        p = queue->next;
        printf("%s: ", name);
        printf("[%d]", queue->pid);
        while (p != NULL)
        {
            printf(" -> [%d]", p->pid);
            p = p->next;
        }
        printf(" -> NULL\n");
    }
}
