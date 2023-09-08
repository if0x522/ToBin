#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "info.h"
#include "srec.h"

static void readSREC_data(FILE *fp_in, FILE *fp_out, FILE *fp_info);
static uint8_t Srecchechsum(struct SREC_type *srec);
static void readSrecData(struct SREC_type *srec, FILE *fp_out);
static void srecseve_info(FILE *fp_info);

static info_t *info_p;
static uint64_t addr = 0;                       // 记录当前地址，用于判断是否连续
static uint64_t addr_last = 0xFFFFFFFFFFFFFFFF; // 记录上一次地址，用于判断是否连续
static char fileinfo[128];

void srec_info(char *file)
{
    FILE *fp_in;
    FILE *fp_out;
    FILE *fp_info;
    char filename_out[64];
    char filename_info[64];
    memcpy(filename_out, file, strlen(file) + 1);
    info_p = &info;
    // 将文件名后缀改为 .info
    if (strcat(filename_out, ".bin") == NULL)
    {
        printf("The file name is too long!\n");
    }
    // 将文件名后缀改为 .out
    memcpy(filename_info, file, strlen(file) + 1);
    if (strcat(filename_info, ".info") == NULL)
    {
        printf("The file name is too long!\n");
    }
    // 打开文件
    fp_in = fopen(file, "r");
    fp_out = fopen(filename_out, "wb");
    fp_info = fopen(filename_info, "w");
    if (fp_in == NULL || fp_out == NULL || fp_info == NULL)
    {
        printf(" File open error !\n");
        return;
    }
    printf("file check OK\n");
    // 读取文件
    readSREC_data(fp_in, fp_out, fp_info);
    // 关闭文件
    fclose(fp_in);
    fclose(fp_out);
    fclose(fp_info);
}

static void readSREC_data(FILE *fp_in, FILE *fp_out, FILE *fp_info)
{
    char buf[1024];
    char *p;
    uint8_t data[512];
    struct SREC_type srec;
    // 读一行数据
    while (fgets(buf, sizeof(buf), fp_in) != NULL)
    {
        if (buf[0] == 's' || buf[0] == 'S')
        {
            // 将字符串转化成16进制数
            int i = 0;
            buf[0] = '0';
            p = buf;
            sscanf(p, "%02hhx", &data[0]);
            p += 2;
            sscanf(p, "%02hhx", &data[1]);
            p += 2;
            srec.len = data[1];
            for (i = 2; i < srec.len + 3; i++)
            {
                sscanf(p, "%02hhx", &data[i]);
                p += 2;
            }
            // 将数据解析为srec 结构体
            srec.type = data[0];
            memcpy(srec.data.data, data + 2, srec.len - 1);
            srec.data.len = srec.len - 1;
            srec.checksum = data[1 + srec.len];
            if (Srecchechsum(&srec) == 0)
            {
                return;
            }
            // 解析数据
            readSrecData(&srec, fp_out);
        }
    }
    // 写入info文件
    srecseve_info(fp_info);
}
static uint8_t Srecchechsum(struct SREC_type *srec)
{
    uint32_t sum = 0;
    uint8_t i;
    sum += srec->len;
    for (i = 0; i < srec->data.len; i++)
    {
        sum += srec->data.data[i];
    }
    i = 0xFF - (sum & 0xff);
    if (i != srec->checksum)
    {
        printf("S-record file Checksum error!\n");
        return 0;
    }
    return 1;
}
static void readSrecData(struct SREC_type *srec, FILE *fp_out)
{
    uint8_t *tmpdatap;
    switch (srec->type)
    {
    case 0x00:
        memcpy(fileinfo, srec->data.data + 2, srec->data.len - 2);
        return;
        break;
    case 0x01: // 地址长度两字节
        addr = ((srec->data.data[0]) << 8) | (srec->data.data[1]);
        tmpdatap = srec->data.data + 2;
        srec->data.len -= 2;
        break;
    case 0x02: // 地址长度三字节
        addr = ((srec->data.data[0]) << 16) | ((srec->data.data[1]) << 8) | (srec->data.data[2]);
        tmpdatap = srec->data.data + 3;
        srec->data.len -= 3;
        break;
    case 0x03: // 地址长度四字节
        addr = ((srec->data.data[0]) << 24) | ((srec->data.data[1]) << 16) | ((srec->data.data[2]) << 8) | (srec->data.data[3]);
        tmpdatap = srec->data.data + 4;
        srec->data.len -= 4;
        break;
    default:
        return;
        break;
    }
    if (addr != addr_last)
    {
        // 开始新段
        info_p->next = (info_t *)malloc(sizeof(info_t));
        info_p = info_p->next;
        info_p->addr = addr;
        info_p->size = 0;
        info_p->next = NULL;
        fwrite(tmpdatap, srec->data.len, 1, fp_out);
        addr_last = addr + srec->data.len;
        info_p->size += srec->data.len;
    }
    else
    {
        // 连续段
        fwrite(tmpdatap, srec->data.len, 1, fp_out);
        addr_last += srec->data.len;
        info_p->size += srec->data.len;
    }
}
static void srecseve_info(FILE *fp_info)
{
    info_t *p = &info;
    fprintf(fp_info, "#%s\n", fileinfo);
    printf("#%s\n", fileinfo);
    do
    {
        p = p->next;
        fprintf(fp_info, "addr: 0x%08lx, size: 0x%08lx\n", p->addr, p->size);
        printf("addr: 0x%08lx, size: 0x%08lx\n", p->addr, p->size);
    } while (p->next != NULL);
}