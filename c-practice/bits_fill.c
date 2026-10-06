#include <stdio.h>

/* ============================================================
 * 用位运算实现 4 个操作。每个空只填"一个运算符"（也许是两个字符）。
 * 不许用 if，一行搞定。
 *
 * 测试数据：x = 0xB1
 *   十六进制一个数字 = 4 个位，所以 0xB1 = 1011 0001
 *     位7=1  位6=0  位5=1  位4=1  位3=0  位2=0  位1=0  位0=1
 * ============================================================ */

/* 空 1：把 x 的第 n 位置 1 */
unsigned char set_bit(unsigned char x, int n)
{
    return x ___ (1 << n);
}

/* 空 2：把 x 的第 n 位清 0 */
unsigned char clear_bit(unsigned char x, int n)
{
    return x ___ ~(1 << n);
}

/* 空 3：把 x 的第 n 位翻转 */
unsigned char toggle_bit(unsigned char x, int n)
{
    return x ___ (1 << n);
}

/* 空 4：读 x 的第 n 位，返回 0 或 1 */
int read_bit(unsigned char x, int n)
{
    return (x ___ n) & 1;
}

int main(void)
{
    unsigned char x = 0xB1;          /* 1011 0001 */

    printf("x        = %d\n", x);
    printf("set 1    = %d\n", set_bit(x, 1));
    printf("clear 4  = %d\n", clear_bit(x, 4));
    printf("toggle 0 = %d\n", toggle_bit(x, 0));
    printf("read 4   = %d\n", read_bit(x, 4));
    printf("read 3   = %d\n", read_bit(x, 3));
    return 0;
}

/* ============================================================
 * 预期输出：
 *   x        = 177
 *   set 1    = 179
 *   clear 4  = 161
 *   toggle 0 = 176
 *   read 4   = 1
 *   read 3   = 0
 *
 * 编译运行（在 work\c-practice 目录下）：
 *   gcc bits_fill.c -o output\bits_fill.exe -Wall ; .\output\bits_fill.exe
 * ============================================================ */
