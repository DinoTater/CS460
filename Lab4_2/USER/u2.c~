#include "ucode.c"

main()
{ 
    char name[64];
    int pid, cmd, out;
    printf("in u2.c\n");

    while(1)
    {
        pid = getpid();

        color = 0x0C;

        printf("----------------------------------------------\n");
        printf("Eso es el proceso numero %d en en el segundo modo de U: segmento=%x\n", pid, getcs());
        show_menu();
        printf("Your Order ? ");
        gets(name); 
        if (name[0]==0) 
            continue;

        cmd = find_cmd(name);

        if (cmd >= 0)
        {
            out = commands[cmd].f();
        }
        else
        {
            invalid(name);
        }
    }
}
