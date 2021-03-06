struct semaphore{
  int value;
  PROC *queue;
};

// Request access
int P(struct semaphore *s)
{
  // write YOUR C code for P()
	printf("Got to P\n");
	if (--s->value < 0)
	{
		running->status = BLOCK;
		enqueue(&s->queue, running);
	}
}

// Close access
int V(struct semaphore *s)
{
  // write YOUR C code for V()
	
	PROC *p;
	printf("Got to V\n");
	if(++s->value <= 0)
	{
		p = dequeue(&s->queue);
		p->status = READY;
		enqueue(p, &readyQueue);
	}
}
