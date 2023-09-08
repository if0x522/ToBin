
#include <stdio.h>
#include <string.h>
#include "hex.h"
#include "srec.h"
#include "help.h"
#include "info.h"

struct info info = {
    .addr = 0,
    .size = 0,
    .next = NULL,
};

int main(int arg, char **argc)
{
    // 判定命令格式是否正确
    // 解析第二个参数
    if (strcmp(argc[1], "-?") == 0 || strcmp(argc[1], "--help") == 0)
    {
        help();
    }
    else if (strcmp(argc[1], "-v") == 0 || strcmp(argc[1], "--version") == 0)
    {
        printf(" Version: 1.0.0\n");
    }
    else if (strcmp(argc[1], "-h") == 0)
    {
        printf(" Input hex file: %s\n", argc[2]);
        hex_info(argc[2]);
    }
    else if (strcmp(argc[1], "-s") == 0)
    {
        printf(" Input S-record file: %s\n", argc[2]);
        srec_info(argc[2]);
    }
    else
    {
        printf(" Command format error !\n");
        help();
    }
}
