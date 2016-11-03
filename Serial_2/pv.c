struct semaphore{
  int value;
  PROC *queue;
};

int P(struct semaphore *s)
{
  // write YOUR C code for P()
	if (--s->value < 0)
	{
		running->status = BLOCK;
		wait(&s->queue);
	}
}

int V(struct semaphore *s)
{
  // write YOUR C code for V()
	if(++s->value <= 0)
	{
		
	}
}
