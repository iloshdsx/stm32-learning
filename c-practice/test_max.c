#include <stdio.h>

/* 这是你刚写好的函数，原样搬过来 */
int max_index(const int *arr, int len, int *out_index)
{
    int max = arr[0];
    *out_index = 0;
    for (int i = 1; i < len; i++) {
        if (arr[i] > max) {
            max = arr[i];
            *out_index = i;
        }
    }
    return max;
}

static void test(const char *name, const int *arr, int len, int expected_max, int expected_idx)
{
    int idx = -999;
    int m = max_index(arr, len, &idx);
    int ok = (m == expected_max && idx == expected_idx);
    printf("%-16s -> max = %4d, index = %d    [%s]\n", name, m, idx, ok ? "PASS" : "FAIL");
}

int main(void)
{
    int t1[5] = {3, 9, 2, 7, 5};
    int t2[3] = {-5, -1, -9};
    int t3[1] = {42};
    int t4[6] = {7, 7, 7, 7, 7, 7};
    int t5[4] = {100, 2, 3, 4};

    test("normal",     t1, 5,   9, 1);
    test("all-negative", t2, 3,  -1, 1);
    test("single",     t3, 1,  42, 0);
    test("all-equal",  t4, 6,   7, 0);
    test("max-first",  t5, 4, 100, 0);
    return 0;
}