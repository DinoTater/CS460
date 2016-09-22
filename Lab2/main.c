#include "type.h"
#include "wait.c"
#include "kernel.c"
#include "my_io.c"


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

PROC *get_proc(PROC **list)			// return a FREE PROC pointer from list
{
	if (freeList != NULL)
		return dequeue(&freeList);
	return 0;
}

int put_proc (PROC **list, PROC *p)	// enter p into list
{
	// Free the process
	p->status = FREE;

	// Check list for other items, otherwise list = p
	if (list == NULL)
	{
		*list = p;
		p->next = NULL;
	}
	else
	{
		p->next = (*list)->next;
		(*list)->next = p;
	}
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

int sleep (int event)
{
	running->event = event;
	running->status = SLEEP;
	tswitch();
}

int wakeup (int event)
{
	int i; PROC *p;

	for (i = 1; i < NPROC; i++)
	{
		p = &proc[i];
		if (p->status == SLEEP && p->event == event)
		{
			p->event = 0;
			p->status = READY;
			enqueue(&readyQueue, p);
		}
	}
}

int init()							// initialize PROC structures
{
	PROC *p; int i;

	printf("init ....\n");

	for (i=0; i<NPROC; i++) {	 	// initialize all PROCs
		p = &proc[i];
		p->pid = i;
		p->status = FREE;
		p->priority = 0;
		p->next = &proc[i+1];
	}
	proc[NPROC-1].next = 0;
	freeList = &proc[0];
	readyQueue = sleepList = 0;
	/******* create P0 as running *******/
	p = get_proc(&freeList);
	p->ppid = 0;
	p->status = READY;
	running = p;					// P0 is now running
	nproc = 1;
	printf("init complete\n");
}
			
int scheduler ()
{
	if (running->status == READY)		// if running is still READY
		enqueue(&readyQueue, running);	// enter it into readyQueue
	running = dequeue(&readyQueue);		//new running
	rflag = 0;
}

void main ()
{
	printf("MTX starts in main()\n");
	init();								// initialize and create P0 as running
	kfork();							// P0 creates child P1
	
	while(1)							// P0 switches if readyQueue not empty
	{
		while (!readyQueue)
			kfork();
		tswitch();
	}
}