#include "ucode.c"
int color;

main(int argc, char *argv[])
{ 
  char name[64]; int pid, ppid, cmd, segment, i;
 
  while(1){
       pid = getpid();
       ppid = getppid();
       color = 0x000B + (pid % 5);
       segment = (pid+1)*0x1000;

       printf("Proc %d : Parent=%d\n", pid, ppid);
       while(1);

       printf("==============================================\n");
       printf("I am proc %d in U mode: segment=%x\n, ppid = %d", pid, segment, ppid);
       show_menu();
       printf("Command ? ");
       gets(name); 
       if (name[0]==0) 
           continue;

       cmd = find_cmd(name);

       switch(cmd){
           case 0 : getpid();   break;
           case 1 : ps();       break;
           case 2 : chname();   break;
           case 3 : kmode();    break;
           case 4 : kswitch();  break;
           case 5 : wait();     break;

           case 6 : exit();     break;
           case 7 : fork();     break;
           case 8 : exec();     break;
           case 9 : do_vfork(); break;

           default: invalid(name); break;
       } 
  }
}


