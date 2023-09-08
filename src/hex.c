#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hex.h"
#include "info.h"

static void read_data(FILE *fp_in, FILE *fp_out, FILE *fp_info);
static uint8_t chechsum(struct hex_type *hex);
static void readHexData(struct hex_type *hex, FILE *fp_out);
static void seve_info(FILE *fp_info);

static info_t *info_p;
static uint64_t addr = 0;                       // 记录当前地址，用于判断是否连续
static uint64_t addr_last = 0xFFFFFFFFFFFFFFFF; // 记录上一次地址，用于判断是否连续
static uint64_t base_addr = 0;                  // 记录基地址，用于判断是否连续

void hex_info(char *file)
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
    read_data(fp_in, fp_out, fp_info);
    // 关闭文件
    fclose(fp_in);
    fclose(fp_out);
    fclose(fp_info);
}

static void read_data(FILE *fp_in, FILE *fp_out, FILE *fp_info)
{
    char buf[1024];
    char *p;
    uint8_t data[512];
    struct hex_type hex;
    // 读一行数据
    while (fgets(buf, sizeof(buf), fp_in) != NULL)
    {
        // 写入 .bin 文件
        if (buf[0] == ':')
        {
            // 将字符串转换为十六进制数
            int i = 0;
            p = buf + 1;
            sscanf(p, "%02hhx", &data[0]);
            p += 2;
            hex.len = data[0];
            for (i = 1; i < hex.len + 5; i++)
            {
                sscanf(p, "%02hhx", &data[i]);
                p += 2;
            }
            // 将数据解析为 hex 结构体
            hex.type = data[3];
            hex.addr = (data[1] << 8) | data[2];
            hex.data.len = hex.len;
            memcpy(hex.data.data, data + 4, hex.len);
            hex.checksum = data[4 + hex.len];
            // 检验校验和
            if (chechsum(&hex) == 0)
            {
                return;
            }
            // 解析数据
            readHexData(&hex, fp_out);
        }
    }
    // 写入 .info 文件
    seve_info(fp_info);
}

static uint8_t chechsum(struct hex_type *hex)
{
    uint32_t sum = 0;
    uint8_t i;
    sum += hex->len;
    sum += (hex->addr >> 8) & 0xff;
    sum += hex->addr & 0xff;
    sum += hex->type;
    for (i = 0; i < hex->len; i++)
    {
        sum += hex->data.data[i];
    }
    i = 0x100 - (sum & 0xff);
    if (i != hex->checksum)
    {
        printf("hex file Checksum error !\n");
        return 0;
    }
    return 1;
}

static void readHexData(struct hex_type *hex, FILE *fp_out)
{
    switch (hex->type)
    {
    case 0x02: // 扩展段地址, 16 位,后续数据的地址加上该地址左移 4 位
        base_addr = info_p->addr + ((hex->data.data[0] << 8) | hex->data.data[1]) << 4;
        break;
    case 0x04: // 扩展线性地址, 32 位,后续数据的地址加上该地址左移 16 位
        base_addr = ((hex->data.data[0] << 24) | (hex->data.data[1] << 16));
        break;
    case 0x00: // 开始线性地址,
        addr = base_addr + hex->addr;
        if (addr_last != addr)
        {
            // 开始新段
            info_p->next = (info_t *)malloc(sizeof(info_t));
            info_p = info_p->next;
            info_p->addr = addr;
            info_p->size = 0;
            info_p->next = NULL;
            fwrite(hex->data.data, hex->len, 1, fp_out);
            // for (int i = 0; i < hex->len; i++)
            // {
            //     fwrite(&hex->data.data[i], 1, 1, fp_out);
            // }
            addr_last = addr + hex->len;
            info_p->size += hex->len;
        }
        else
        {
            // 连续段
            // for (int i = 0; i < hex->len; i++)
            // {
            //     fwrite(&hex->data.data[i], 1, 1, fp_out);
            // }
            fwrite(hex->data.data, hex->len, 1, fp_out);
            addr_last += hex->len;
            info_p->size += hex->len;
        }
        break;
    default:
        break;
    }
}

static void seve_info(FILE *fp_info)
{
    info_t *p = &info;
    do
    {
        p = p->next;
        fprintf(fp_info, "addr: 0x%08lx, size: 0x%08lx\n", p->addr, p->size);
        printf("addr: 0x%08lx, size: 0x%08lx\n", p->addr, p->size);
    } while (p->next != NULL);
}
