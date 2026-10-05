#include <stdio.h>

/* ------------------------------------------------------------
 * 空 A：把 pa、pb 指向的两个数交换（你刚在 swap_exercise.c 做过一遍）
 *       不用我说了，自己写 3 行
 * ------------------------------------------------------------ */
void swap(int *pa, int *pb)
{
    int t = *pa;
    *pa = *pb;
    *pb = t;
}


/* ------------------------------------------------------------
 * 选择排序（从小到大）：每轮从"剩下的数"里挑最小的，放到最前面
 *   arr 是数组首地址，len 是元素个数（为什么必须传 len？看下面第四节）
 * ------------------------------------------------------------ */
void selection_sort(int arr[], int len)
{
    for (int i = 0; i < len - 1; i++)
    {
        int min_index = i;              /* 空 B：一开始先假设谁最小？ */

        for (int j = i + 1; j < len; j++)
        {
            if (arr[j] < arr[min_index])        /* 空 C：拿 j 位置的数，和谁比？ */
            {
                min_index = j;          /* 空 D：发现更小的了，记住它的下标 */
            }
        }

        swap(&arr[i], &arr[min_index]);                  /* 空 E：把 i 位置和 min_index 位置交换 */
    }
}


void print_array(int arr[], int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


int main(void)
{
    int a[] = {5, 2, 9, 1, 7, 3};
    int len = sizeof(a) / sizeof(a[0]);   /* 求元素个数的标准写法，下面要讲 */

    printf("before: ");
    print_array(a, len);

    selection_sort(a, len);

    printf("after : ");
    print_array(a, len);

    return 0;
}