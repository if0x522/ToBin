#ifndef HEX_H
#define HEX_H

#include <stdint.h>

struct hex_data
{
    uint8_t data[256];
    uint8_t len;
};

struct hex_type
{
    uint8_t type;
    uint8_t len;
    uint16_t addr;
    struct hex_data data;
    uint8_t checksum;
};

void hex_info(char *file);

#endif
