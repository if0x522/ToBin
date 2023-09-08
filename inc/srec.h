#ifndef SREC_H
#define SREC_H

#include <stdint.h>

struct SREC_data
{
    uint8_t data[256];
    uint8_t len;
};

struct SREC_type
{
    uint8_t type;
    uint8_t len;
    struct SREC_data data;
    uint8_t checksum;
};

void srec_info(char *file);

#endif