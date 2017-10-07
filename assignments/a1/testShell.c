#include <stdio.h>


int main()
{
    char *name[2];

    name[0] = "pwgen";
    name[1] = NULL;
    
    execve(name[0],name,NULL);
}
