#include <stdio.h>

void print_array(int arr[], int len)
{
    for (int i = 0; i < len; i++) printf("%d ", arr[i]);
    printf("\n");
}

void swap(int *pa, int *pb)
{
    int t = *pa;
    *pa = *pb;
    *pb = t;
}

/* 带"沿途打印"的选择排序：把每一步的 i / j / min_index 都打出来 */
void selection_sort_trace(int arr[], int len)
{
    for (int i = 0; i < len - 1; i++)
    {
        int min_index = i;
        printf("--- i=%d：分界线在 %d 号格，假设最小也在 %d 号格 ---\n", i, i, min_index);

        for (int j = i + 1; j < len; j++)
        {
            printf("    j=%d: arr[%d]=%d 比 arr[min_index=%d]=%d ?  ",
                   j, j, arr[j], min_index, arr[min_index]);
            if (arr[j] < arr[min_index])
            {
                min_index = j;
                printf("是 -> min_index 改成 %d\n", min_index);
            }
            else
            {
                printf("否 -> min_index 不动\n");
            }
        }

        printf("  本轮结束：最小在 %d 号格，交换 arr[%d] 和 arr[%d]  => ", min_index, i, min_index);
        swap(&arr[i], &arr[min_index]);
        print_array(arr, len);
    }
}

int main(void)
{
    int arr[3] = {5, 1, 3};
    int len = 3;

    printf("初始：");
    print_array(arr, len);

    selection_sort_trace(arr, len);

    printf("结果：");
    print_array(arr, len);
    return 0;
}