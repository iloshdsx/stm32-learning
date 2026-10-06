#include <stdio.h>

/* ============================================================
 * 任务：写一个函数，一次性"带回"两个结果
 *   输入：数组 a、长度 len
 *   输出：最小值写到 *pmin，最大值写到 *pmax
 *
 * 为什么要用指针？
 *   C 的函数只能 return 一个值。想带回两个，就得把"地址"传进来，
 *   函数往那个地址里写 —— 这就是 HAL 里
 *   HAL_UART_Receive(&huart, buf, ...) 传缓冲区的同一套路子。
 * ============================================================ */
void min_max(int a[], int len, int *pmin, int *pmax)
{
    int mn = a[0];
    int mx = a[0];

    for (int i = 1; i < len; i++)
    {
        if (a[i] < mn)
        {
            mn = a[i];          /* 空 1：发现更小的了，记下这个数 */
        }
        if (a[i] > mx)
        {
            mx = a[i];          /* 空 2：发现更大的了，记下这个数 */
        }
    }

    *pmin = mn;                 /* 空 3：把最小值"送回"调用者，写给哪个指针？ */
    *pmax = mx;                 /* 空 4：把最大值送回去 */

    printf("func : pmin = %p, pmax = %p\n", (void *)pmin, (void *)pmax);
}

/* 注意：上面那个 } 表示 min_max 的地盘到此为止。
 * 下面才是 main 的地盘。两个函数各住各的房间，互相看不见对方的变量。 */

int main(void)
{
    int a[] = {7, 3, 9, 1, 8, 2};
    int len = sizeof(a) / sizeof(a[0]);
    int min = 0;
    int max = 0;

    /* 调用之前打印 &min / &max —— 这是 main 里的门牌号 */
    printf("main : &min = %p, &max = %p\n", (void *)&min, (void *)&max);

    min_max(a, len, &min, &max);     /* 空 5、空 6：函数要的是地址，这里该传什么？ */

    printf("min = %d, max = %d\n", min, max);
    return 0;
}

/* ============================================================
 * 预期输出（三行，地址数字每次运行可能不同，重点是第 1、2 行要一一对应）：
 *   main : &min = 000000000061FE0C, &max = 000000000061FE08
 *   func : pmin = 000000000061FE0C, pmax = 000000000061FE08
 *   min = 1, max = 9
 *
 * 编译运行（在 work\c-practice 目录下）：
 *   gcc min_max_fill.c -o t.exe -Wall ; ./t.exe
 *
 * 结论：&min == pmin，&max == pmax —— 函数拿到的就是 main 的地址，
 *       所以 *pmin = mn 改的就是 main 的 min。
 * ============================================================ */
