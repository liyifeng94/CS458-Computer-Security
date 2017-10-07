
#ifndef NTP_H
#define NTP_H

#include <sys/types.h>

#define MON_GETLIST_1   42

struct ntp_header
{
    u_int8_t flags;
    u_int8_t seqNum;
    u_int8_t imple;
    u_int8_t reqCode;
};

#endif //NTP_H
