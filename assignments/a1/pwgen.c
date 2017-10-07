/*
 * A program that randomly generates a password and sets that as your password
 *
 */

#include <crypt.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <openssl/rand.h>
#include <pwd.h>
#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PW_DES 0
#define PW_MD5 1
#define PW_BLOWFISH 2
#define PW_SHA256 3
#define PW_SHA512 4
#define FILENAME "/tmp/pwgen"
#define BUFF_SZ 1024
#define PASSWD_SZ 8
#define SALT_SZ 32

typedef struct {
  unsigned char write;
  unsigned char version;
  unsigned char type;
  char salt[SALT_SZ];
  char filename[512];
} pwgen_args;

extern int errno;
extern char *optarg;
int help;

// Returns numeric uid of user
static uid_t get_uid() {
  char* user;
  struct passwd* pw;
  uid_t uid;

  uid = 0;
  user = getenv("USER");
  setpwent();
  while ((pw = getpwent()) != NULL) {
    if (strcmp(pw->pw_name, user) == 0) {
      uid = pw->pw_uid;
      break;
    }
  }
  endpwent();
  return uid;
}

// Returns numeric gid of user
static gid_t get_gid() {
  char* user;
  struct passwd* pw;
  gid_t gid;

  gid = 0;
  user = getenv("USER");
  setpwent();
  while ((pw = getpwent()) != NULL) {
    if (strcmp(pw->pw_name, user) == 0) {
      gid = pw->pw_gid;
      break;
    }
  }
  endpwent();
  return gid;
}

// Returns username of current user
static char* get_username() {
  struct passwd* pw;
  pw = getpwuid(get_uid());
  return pw->pw_name;
}

// Checks if the tmp file exists, and if it has the correct permissions
// Returns 0 if successful, -1 if unsuccessful
static int check_perms() {
  struct stat buf;
  FILE* fd;

  if (stat(FILENAME, &buf) != 0) {
    unlink(FILENAME);
  }
  fd = fopen(FILENAME, "w");
  fclose(fd);
  chown(FILENAME, get_uid(), get_gid());
  return 0;
}

// A terrible way to get entropy based on input from the user
//TODO: Fix this, this is a terrible hack
static void get_entropy(char* buffer) {
  int i, c;

  printf("Type stuff so I can gather entropy, Ctrl-D to end:\n");
  i = 0;
  c = getc(stdin);
  while (i < BUFF_SZ) {
    if (c == EOF) return;
    buffer[i] = c;
    c = getc(stdin);
    i++;
  }
  buffer[i] = '\0';
}

// Write entropy to a temporary file, because we are lazy
static void fill_entropy() {
  char buffer[BUFF_SZ];
  FILE* fd;

  get_entropy(buffer);
  fd = fopen(FILENAME, "w");
  fwrite(buffer, strlen(buffer), sizeof(char), fd);
  fclose(fd);
}

// Converts buffer into something sort of meaningful
//TODO: Fix this, this is a terrible hack
static void convert_uc_c(unsigned char* buf, char* res, size_t size) {
  int i;
  unsigned char c;
  for (i = 0; i < size; i++) {
    c = (buf[i] >> 2);
    if (c < 10)
      res[i] = c + '0';
    else if (c < 36)
      res[i] = (c-10) + 'A';
    else if (c < 62)
      res[i] = (c-36) + 'a';
    else if (c < 63)
      res[i] = '-';
    else
      res[i] = '_';
  }
}

// Generate a password
static void gen_passwd(pwgen_args args, char* passwd) {
  unsigned char buffer[PASSWD_SZ];

  RAND_load_file(args.filename, -1);
  RAND_bytes(buffer, PASSWD_SZ);
  convert_uc_c(buffer, passwd, PASSWD_SZ);
}

// Generate a pseudo salt
static void gen_salt(char* salt) {
  unsigned char buffer[SALT_SZ];

  RAND_pseudo_bytes(buffer, SALT_SZ);
  convert_uc_c(buffer, salt, SALT_SZ);
}

// Generate the hash of the password
static char* gen_crypt(pwgen_args args, char* password) {
  char salt[4+SALT_SZ+1], *ptr;
  size_t salt_sz = SALT_SZ;

  ptr = salt;
  memset(salt, 0, sizeof(salt));
  switch(args.type) {
    case PW_MD5:
      ptr += sprintf(ptr, "$1$");
      break;
    case PW_BLOWFISH:
      ptr += sprintf(ptr, "$2a$");
      break;
    case PW_SHA256:
      ptr += sprintf(ptr, "$5$");
      break;
    case PW_SHA512:
      ptr += sprintf(ptr, "$6$");
      break;
    default:
      salt_sz = 2;
  }

  memcpy(ptr, args.salt, salt_sz);
  return crypt(password, salt);
}

// Update entry in /etc/shadow
static void update_spent(char* crypt) {
  FILE *old, *new;
  struct spwd *spw, spw_copy;
  char* username;

  link("/etc/shadow", "/etc/shadow~");
  unlink("/etc/shadow");
  lckpwdf();
  old = fopen("/etc/shadow~", "r");
  new = fopen("/etc/shadow", "w");

  printf("opening shadow files\n");
  username = get_username();
  spw = fgetspent(old);
  while (spw != NULL) {
    if (strcmp(username, spw->sp_namp) == 0) {
      memcpy(&spw_copy, spw, sizeof(struct spwd));
      spw_copy.sp_pwdp = crypt;
      putspent(&spw_copy, new);
      memset(&spw_copy, 0, sizeof(struct spwd));
    } else {
      putspent(spw, new);
    }
    spw = fgetspent(old);
  }
  fclose(old);
  fclose(new);
  unlink("/etc/shadow~");

  ulckpwdf();
}

// Parses arguments from command line
// Returns struct with the appropriate flags set
static pwgen_args parse_args(int argc, char* argv[]) {
  int c, res;
  pwgen_args args;
  struct option long_options[] = {
    {"salt", 1, NULL, 's'},
    {"seed", 2, NULL, 'e'},
    {"type", 1, NULL, 't'},
    {"write", 0, NULL, 'w'},
    {"version", 0, NULL, 'v'},
    {"help", 0, NULL, 'h'},
    {0, 0, 0, 0}
  };

  help = 0;
  memset(&args, 0, sizeof(pwgen_args));
  c = 0;
  while (1) {
    c = getopt_long(argc, argv, "s:e:t:wvh", long_options, NULL);
    if (c == -1) break;

    switch (c) {
      case 'e':
        if (optarg) {
          strncpy(args.filename, optarg, sizeof(args.filename));
        } else {
          res = check_perms();
          if (res) {
            fprintf(stderr, "%s already exists, and is owned by a different user :(\n", FILENAME);
            exit(1);
          }
          else {
            strcpy(args.filename, FILENAME);
            fill_entropy();
          }
        }
        break;
      case 's':
        strcpy(args.salt, optarg);
        break;
      case 't':
        args.type = atoi(optarg);
        break;
      case 'h':
        help = 1;
        break;
      case 'w':
        args.write = 1;
        break;
      case 'v':
        args.version = 1;
        break;
      default:
        help = 1;
        break;
    }
    if (help) break;
  }
  if (strlen(args.salt) == 0) gen_salt(args.salt);
  return args;
}

// Prints usage
static void print_usage(char* arg) {
  char buffer[640];
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
      "  4 - SHA-512\n", arg);
  printf(buffer);
}

// Main
int main(int argc, char* argv[]) {
  pwgen_args args;
  char passwd[PASSWD_SZ+1], *crypt;

  args = parse_args(argc, argv);

  if (help != 0) {
    print_usage(argv[0]);
    return 1;
  }

  memset(passwd, 0, sizeof(passwd));
  gen_passwd(args, passwd);
  printf("Generated password: %s\n", passwd);

  if (args.write == 0) {
    memset(passwd, 0, sizeof(passwd));
  } else {
    crypt = gen_crypt(args, passwd);
    memset(passwd, 0, sizeof(passwd));
    printf("Updating /etc/shadow...\n");
    update_spent(crypt);
  }

  return 0;
}
