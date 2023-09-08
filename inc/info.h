#ifndef INFO_H
#define INFO_H

#include <stdint.h>

typedef struct info info_t;

struct info
{
    uint64_t addr;
    uint64_t size;
    info_t *next;
};

extern struct info info;

#endif
