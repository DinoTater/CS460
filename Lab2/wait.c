int ksleep (int event)
{
	running->event = event;
	running->status = SLEEP;
	put_proc(&sleepList, running);
	tswitch();
}

int kwakeup (int event)
{
	int i, check; PROC *p;

	for (i = 1; i < NPROC; i++)
	{
		p = &proc[i];
		if (p->status == SLEEP && p->event == event)
		{
			p->event = 0;
			p->status = READY;
			check = removeProc (&sleepList, p);
			if (check == -1) printf("Awwwwww Crap!\n");
			enqueue(&readyQueue, p);
		}
	}
}

int removeProc (PROC **queue, PROC *p)
{
	int i = 0;
	PROC *curr = *queue, *prev = NULL;

	if (*queue == NULL)
		return -1;

	if (p == *queue)
		*queue = (*queue)->next;

	else
	{
		prev = curr;
		curr = curr->next;
		while (curr)
		{
			if(curr == p)
			{
				prev->next = curr->next;
				break;
			}
			else
			{
				prev = curr;
				curr = curr->next;
			}
		}
	}

	if (curr == NULL) return 1;

	return 1;
}

int ready (PROC *p)
{
	p->status = READY;
	enqueue(&readyQueue, p);
}

int kexit (int exitValue)
{
	int i, wakeupP1 = 0;
	PROC *p;
	if (running->pid==1 && nproc>2)
	{
		printf("other procs still exist, P1 can't die yet\n");
		return -1;
	}

	// send children (dead or alive) to P1's orphanage
	for (i = 1; i < NPROC; i++)
	{
		p = &proc[i];
		if (p->status != FREE && p->ppid == running->pid)
		{
			p->ppid = 1;
			p->parent = &proc[i];
			wakeupP1++;
		}
	}
	//record exitValue and become a ZOMBIE
	running->exitCode = exitValue;
	running->status = ZOMBIE;
	put_proc(&zombieList, running);
	//wakeup parent and also P1 if necessary
	kwakeup(running->parent);	//parent sleeps on its PROC address
	if (wakeupP1)
		kwakeup (&proc[1]);
	tswitch();
}

int kwait (int *status) // wait for zombie child
{
	PROC *p;
	int i, hasChild = 0;

	while (1)
	{
		for (i = 1; i < NPROC; i++)			// search PROCs for a child
		{
			p = &proc[i];					// exclude p0
			if (p->status != FREE && p->ppid == running->pid)
			{
				hasChild = 1;				// hasChild flag
				if (p->status == ZOMBIE)	// lay the dead child to rest
				{
					*status = p->exitCode;	// collect its exitCode
					p->status = FREE;		// free its PROC to freeList
					removeProc(&zombieList, p);
					put_proc(&freeList, p);
					nproc--;
					return(p->pid);
				}
			}
		}
		if (!hasChild) return -1;		// no child, return ERROR
		ksleep(running); // still has kids alive: sleep on PROC address
	}
}

int do_wait()
{
    int pid, status;
    pid = kwait(&status);
    if (pid == -1)
    {
        printf("\nProcess has no children!\n");
        return -1;
    }
    else
    {
        printf("\npid=%d, status=%d\n", pid, status);
    }
    return 1;
}
