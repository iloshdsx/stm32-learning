#include <stdio.h>

/* Print the low 8 bits of `value` as binary.
 * Example: 9 -> 00001001  (bit7 ... bit0) */
void print_bits(unsigned char value)
{
    for (int i = 7; i >= 0; i--)
    {
        printf("%d", (value >> i) & 1);
    }
}

int main(void)
{
    unsigned char x = 0;

    printf("start : ");
    print_bits(x);
    printf("   (x = %d)\n", x);

    /* 把第 0 位置 1 */
    x = x | (1 << 0);
    printf("set 0 : ");
    print_bits(x);
    printf("   (x = %d)\n", x);

    /* 把第 3 位置 1 */
    x = x | (1 << 3);
    printf("set 3 : ");
    print_bits(x);
    printf("   (x = %d)\n", x);

    /* 把第 0 位清 0 */
    x = x & ~(1 << 0);
    printf("clr 0 : ");
    print_bits(x);
    printf("   (x = %d)\n", x);

    /* 把第 3 位翻转 */
    x = x ^ (1 << 3);
    printf("tog 3 : ");
    print_bits(x);
    printf("   (x = %d)\n", x);

    return 0;
}
