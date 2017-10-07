#ifndef DNS_H
#define DNS_H
#include <cstdint>

struct dns_header
{
    u_int16_t id;
    u_int16_t flags;
    u_int16_t qdcount;
    u_int16_t ancount;
    u_int16_t nscount;
    u_int16_t arcount;
};

#endif //DNS_H
