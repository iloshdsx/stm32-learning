# C 语言练习

配合 STM32 学习刷的 C 题，全部用 gcc + `-Wall` 编译，零警告通过。

| 文件 | 题目 | 涉及知识点 |
|---|---|---|
| `max_index.c` | 求数组最大值及其下标 | 数组、循环、`len` 的用法 |
| `max_index_fill.c` | 同上的填空版（学习用） | 同上 |
| `test_max.c` | 给上面的函数写测试用例 | 断言式自测、边界值（全等/负数/单元素） |

编译运行：

```bash
gcc max_index.c -o t.exe -Wall && ./t.exe
gcc test_max.c -o tt.exe -Wall && ./tt.exe
```

## 学习顺序（已掌握）
1. `len` 是数组长度（元素个数），不是字节数
2. 比较时用 `arr[i] > max`，别忘更新 `index`
3. 一个函数只做一件事：找最大值 vs 打印结果，分开写更好测