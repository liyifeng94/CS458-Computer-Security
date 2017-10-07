#include<stdio.h>

static void testFormatString(char** argv)
{
    char buffer[640];
    /*
    sprintf(buffer, "Usage: %s [options]\n"
      "Randomly generates a password, optionally writes it to /etc/shadow\n"
      "\n"
      "Options:\n"
      "-s, --salt <salt>  Specify custom salt, default is random\n"
      "-e, --seed [file]  Specify custom seed from file, default is from stdin\n"
      "-t, --type <type>  Specify different encryption method\n"
      "-w, --write        Update the /etc/shadow\n"
      "-v, --version      Show version\n"
      "-h, --help         Show this usage message\n"
      "\n"
      "Encryption types:\n"
      "  0 - DES (default)\n"
      "  1 - MD5\n"
      "  2 - Blowfish\n"
      "  3 - SHA-256\n"
            "  4 - SHA-512\n", argv[1]);
    printf("size: %d\n",strlen(buffer));
    */
    int num;
    printf("Usage:  \x3e\xdb\xbf\xff\x3c\xdb\xbf\xff%n",&num );
    printf("length: %d\n",num);
}

int main(int argc, char** argv)
{
    testFormatString(argv);
    return 0;
}
