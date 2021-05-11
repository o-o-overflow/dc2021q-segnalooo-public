
#define _GNU_SOURCE
#include <linux/seccomp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>


#define DEBUG


#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "buf.h"
#include "handler.h"
#include "filter.h"

#define BASECODEP 0xfbf

unsigned long HANDLERPB = 0x100000000000l;
unsigned long CODEPB    = 0x500000000000l;
unsigned long FULLHANDLERSIZE = 0x10000;

unsigned long HANDLERP;
unsigned long CODEP;


void eexit(int c){
    exit(c);
}


void readn(int fp, unsigned long n, unsigned char* buf){
    unsigned long i=0;
    long t=0;
    while(1){
        t = read(fp, &(buf[i]), n-i);
        if(t<=0){
            eexit(3);
        }
        i += t;
        if(i==n){
            break;
        }
    }
}


//https://stackoverflow.com/questions/3408706/hexadecimal-string-to-byte-array-in-c
void hex2bin(unsigned char* in, size_t len, unsigned char* out) {

  static const unsigned char TBL[] = {
     0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  58,  59,  60,  61,
    62,  63,  64,  10,  11,  12,  13,  14,  15,  71,  72,  73,  74,  75,
    76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
    90,  91,  92,  93,  94,  95,  96,  10,  11,  12,  13,  14,  15
  };

  static const unsigned char *LOOKUP = TBL - 48;

  unsigned char* end = in + len;

  while(in < end) *(out++) = LOOKUP[*(in++)] << 4 | LOOKUP[*(in++)];

}


int main(int argc, char** argv) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("Give me some code!\n");

    unsigned long rnd1;
    unsigned long rnd2;
    unsigned char buft[0x2000];
    unsigned int userinputlen = 0;
    unsigned char userinput[0x1000];
    unsigned char buf[0x2000];
    unsigned char* ts = "0123456789abcdefABCDEF";

    unsigned long rindex = 0;
    unsigned char cc;
    ssize_t t;
    while(1){
        t = read(0, &cc, 1);
        if(t<=0){
            break;
        }
        if(cc=='\n'){
            break;
        }
        unsigned int ii;
        for(ii=0; ii<strlen(ts); ii++){
            if(cc==ts[ii]){
                break;
            }
        }
        if(ii>=strlen(ts)){
            printf("invalid hex\n");
            eexit(4);
        }

        buft[rindex] = cc;
        rindex+=1;

        if(rindex>=0x2000){
            break;
        }
    }
    if(rindex%2 != 0 || rindex==0){
        printf("invalid hex\n");
        eexit(4);
    }
    userinputlen = rindex/2;
    if(rindex>0x2000 || userinputlen>0x1000){
        printf("invalid hex\n");
        eexit(4);   
    }
    hex2bin(buft, rindex, userinput);

    int urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd == -1) {
        eexit(3);
    }

    while(1){
        readn(urandom_fd, sizeof(rnd1), (unsigned char*)&(rnd1));
        HANDLERP = HANDLERPB + (rnd1 & 0x000000fffffff000l);
        if((HANDLERP & 0x80000000)==0){
            break;
        }
    }

    while(1){
        readn(urandom_fd, sizeof(rnd2), (unsigned char*)&(rnd2));
        CODEP = CODEPB + (rnd2 & 0x00000ffffffff000l);
        if((CODEP & 0x80000000)>0){
            break;
        }
    }


    unsigned long o1,o2,o3,o4,o5,o6;
    o1 = 0l;
    o2 = HANDLERP;
    o3 = HANDLERP+FULLHANDLERSIZE;
    o4 = CODEP-o3;
    o5 = CODEP+0x2000;
    o6 = 0x7ffffffff000l-o5;



    (unsigned char*) mmap((void*)(HANDLERP), (FULLHANDLERSIZE), PROT_READ | PROT_WRITE  | PROT_EXEC, MAP_FIXED |MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    unsigned char* ptr = (unsigned char*)&HANDLERCODE;
    unsigned int i = 0;

    while(i<HANDLERCODELEN){
        ((unsigned char*)HANDLERP)[i] = ptr[i];
        i++;
    }
    mprotect((void*)(HANDLERP+0x0), 0x1000*1, PROT_READ | PROT_EXEC);
    mprotect((void*)(HANDLERP+0x1000), (FULLHANDLERSIZE-0x1000), PROT_READ | PROT_WRITE);

    mmap((void*)(CODEP), (0x1000*2), PROT_READ | PROT_WRITE  | PROT_EXEC, MAP_FIXED |MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for(unsigned int i=0; i<0x1000; i++){
        buf[i] = '\x90';
    }
    for(unsigned int i=0x1000; i<0x2000; i++){
        buf[i] = '\x00';
    }
    memcpy(buf, BUF, BUFLEN);
    unsigned char* set_trap_flag_code = "\x48\xC7\x04\x24\x00\x01\x00\x00\x9D";
    unsigned int set_trap_flag_code_len = 9;

    memcpy(&(buf[BASECODEP]), set_trap_flag_code, set_trap_flag_code_len);
    memcpy(&(buf[BASECODEP+set_trap_flag_code_len]), userinput, userinputlen);
    memcpy((unsigned char*)CODEP, buf, 0x2000);

    mprotect((void*)(CODEP+0x0), 0x1000, PROT_READ | PROT_EXEC);
    mprotect((void*)(CODEP+0x1000), 0x1000, PROT_READ | PROT_WRITE);

    printf("The length of the first instruction is:\n");


    stack_t ss = {
        .ss_size = 0x1000,
        .ss_sp = &(((unsigned char*)HANDLERP)[FULLHANDLERSIZE-0x1000]),
    };
    sigaltstack(&ss, NULL);
    *(unsigned long*)(HANDLERP+FULLHANDLERSIZE-8) = CODEP;

    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_sigaction = (void*)HANDLERP;
    act.sa_flags = SA_SIGINFO | SA_NODEFER | SA_ONSTACK;
    sigaction(SIGTRAP, &act, NULL);

    struct prog {
       unsigned short len;
       unsigned char *filter;
    } rule = {
      .len = sizeof(FILTER) >> 3,
      .filter = FILTER
    };
    if(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) { perror("prctl(PR_SET_NO_NEW_PRIVS)"); eexit(2); }
    if(prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &rule) < 0) { perror("prctl(PR_SET_SECCOMP)"); eexit(2); }
    

    asm volatile(
        ".intel_syntax noprefix;\n"
        "mov r8, %0\n"::"r"(o1):
        );
    asm volatile(
        ".intel_syntax noprefix;\n"
        "mov r9, %0\n"::"r"(o2):
        );
    asm volatile(
        ".intel_syntax noprefix;\n"
        "mov r10, %0\n"::"r"(o3):
        );
    asm volatile(
        ".intel_syntax noprefix;\n"
        "mov r11, %0\n"::"r"(o4):
        );
    asm volatile(
        ".intel_syntax noprefix;\n"
        "mov r12, %0\n"::"r"(o5):
        );
    asm volatile(
        ".intel_syntax noprefix;\n"
        "mov r13, %0\n"::"r"(o6):
        );

    ((void(*)())(CODEP))();

}
