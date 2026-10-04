/* ============================================================
 * 第 1 周作业（填空版）：用指针找出数组最大值和它的下标
 * ------------------------------------------------------------
 * 规则：只填 6 个空，每个空只填【一个数字】或【一个变量名】。
 * 不要复制整个答案，就把 ____ 换成你的答案，然后跑：
 *
 *     cd work\c-practice
 *     gcc max_index_fill.c -o t.exe -Wall
 *     .\t.exe
 *
 * 期望输出：max = 9, index = 1
 * ============================================================ */

#include <stdio.h>

int max_index(const int *arr, int len, int *out_index)
{
    int max = arr[0];        /* 空1：先假装第几个元素最大？填一个数字 */

    *out_index = 0;          /* 空2：下标也先记成几？填一个数字 */

    for (int i = 0; i < len; i++) {   /* 空3：从第几个开始比？
                                             空4：一直比到哪儿？（提示：数组里有几个数） */
        if (arr[i] > max) {    /* 空5：这个元素跟谁比？填一个变量名 */
            max = arr[i];
            *out_index = i;
        }
    }

    return max;                /* 空6：要交出去的是哪个值？填一个变量名 */
}

int main(void)
{
    int data[5] = {3, 9, 2, 7, 5};
    int idx = -1;

    int m = max_index(data, 5, &idx);
    printf("max = %d, index = %d\n", m, idx);

    return 0;
}