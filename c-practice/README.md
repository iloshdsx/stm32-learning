# C 语言练习

配合 STM32 学习刷的 C 题，全部用 gcc + `-Wall` 编译，零警告通过。

| 文件 | 题目 | 涉及知识点 |
|---|---|---|
| `max_index.c` | 求数组最大值及其下标 | 数组、循环、`len` 的用法 |
| `max_index_fill.c` | 同上的填空版（学习用） | 同上 |
| `test_max.c` | 给上面的函数写测试用例 | 断言式自测、边界值 |
| `swap_exercise.c` | 写一个交换函数 | **指针入门**：`&` 取地址、`*` 解引用、传值 vs 传地址 |
| `sort_exercise.c` | 选择排序（从小到大） | 双重循环、下标区间 `[i, len-1]`、`sizeof` 求长度 |
| `sort_trace.c` | 带"逐步打印"的选择排序 | **调试方法：打印中间状态** |
| `min_max_fill.c` | 一个函数同时求最小值和最大值（填空版） | **指针出参**：`*pmin` / `*pmax`、`&min` 传地址、地址验证实验 |

编译运行：

```bash
gcc swap_exercise.c -o t.exe -Wall ; ./t.exe
gcc sort_exercise.c -o t.exe -Wall ; ./t.exe
gcc sort_trace.c    -o t.exe -Wall ; ./t.exe
gcc min_max_fill.c  -o t.exe -Wall ; ./t.exe
```

中文乱码的话：源文件是 UTF-8，Windows 控制台是 GBK。要么 printf 用英文，
要么编译时加 `-fexec-charset=GBK`，要么运行前 `chcp 65001`。

---

## 一、C 语言参数传递：只有"传值"一种

| 你传什么 | 抄的是什么 | 函数里能不能改到原件 |
|---|---|---|
| `swap(x, y)` | 抄了**值** | ❌ 改的是抄件 |
| `swap(&x, &y)` | 抄了**门牌号**（地址） | ✅ 门牌号指向同一间房 |

**面试话术**：问"C 语言参数传递有几种方式？"
答"传值和传地址两种" → 及格；答"**只有一种：传值。传指针也是传值，只是抄的是地址**" → 加分。

## 二、`sizeof` 求数组长度（面试陷阱）

```c
int len = sizeof(a) / sizeof(a[0]);   /* 只在定义数组的那个作用域里有效 */
```

- 在函数里，`int arr[]` 参数会**退化成 `int *arr`**（指针），`sizeof(arr)` 得到的是指针大小，不是数组大小。
- **这就是为什么数组长度必须当参数传进来。**

## 三、选择排序的下标区间怎么推（不是背的）

两条推理，写完区间自己就掉出来：

1. 第 `i` 轮开始时，**左边已经定好了 `i` 个**（下标 `0` 到 `i-1`）
2. 所以这一轮要在下标 **`i` 到 `len-1`** 里找最小 → 区间 `[i, len-1]`

代码里被拆成两半：

```c
int min_index = i;                       /* 负责区间的"头" */
for (int j = i + 1; j < len; j++)        /* 负责区间的"尾" */
```

外层只跑到 `i < len - 1`：因为最后剩 1 个元素时它自动就位，不用排。

## 四、指针出参：一个函数怎么"带回"两个结果

C 的函数 **只能 `return` 一个值**。想要两个结果，就让调用者把**地址**递进来，
函数往那个地址里写。

```c
void min_max(int a[], int len, int *pmin, int *pmax)
{
    int mn = a[0], mx = a[0];
    for (int i = 1; i < len; i++)
    {
        if (a[i] < mn) mn = a[i];      /* 发现更小的，记下来 */
        if (a[i] > mx) mx = a[i];      /* 发现更大的，记下来 */
    }
    *pmin = mn;                        /* 往"最小值的家"写 */
    *pmax = mx;                        /* 往"最大值的家"写 */
}

int main(void)
{
    int a[] = {7, 3, 9, 1, 8, 2};
    int len = sizeof(a) / sizeof(a[0]);
    int min = 0, max = 0;

    min_max(a, len, &min, &max);       /* 传的是门牌号 */
    printf("min = %d, max = %d\n", min, max);   /* min = 1, max = 9 */
}
```

三点一定要记牢：

1. `&min` 是"问 min 的地址"，`pmin` 接住的就是这个地址。
2. `*pmin = mn` 是"**上门**，把 mn 放进 pmin 指的那间房"——改的是 main 里的 `min`。
3. 如果写成 `pmin = &mn`，那只是把**抄件**改成指向别人家，`main` 的 `min` 一点不动。
   这跟"传值改不了原件"是同一个坑，换了个马甲。

**为什么嵌入式天天见这套路？** HAL 都是这么写的：

```c
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
```

第一个参数传句柄地址（既是输入也是输出），返回值只留给**错误码**。
看懂 `*pData` 这种写法，才算能读 HAL 源码。

**验证实验（做过这道题的人一定自己跑一遍）**：

```c
/* main 里，调用 min_max 之前 */
printf("&min = %p, &max = %p\n", (void *)&min, (void *)&max);

/* min_max 里，*pmax = mx; 下面 */
printf("pmin = %p, pmax = %p\n", (void *)pmin, (void *)pmax);
```

两组地址**打印出来是同一个数** —— 这才是"抄的是门牌号"这句话的铁证，
不是背下来的结论。

## 五、踩过的坑

### 1. `min_index = 0` 而不是 `i`（结果 `9 1 2 3 5 7`）

- **现象**：5 个空填了 4 个对，输出"像是排好了却整体右移一位"
- **原因**：每轮都拿 `arr[0]` 当参照，而 `arr[0]` 从第 2 轮起已经是排好的最小值，
  比所有候选都小 → 内层永远不成立 → `min_index` 停在 0 → 把排好的值挤回后面
- **解决**：`int min_index = i;`
- **教训**：这类"**看着几乎对、就是错一点**"的 bug 最危险，它骗得过眼睛

### 2. printf 中文乱码（数字对、中文乱）
- **原因**：源文件 UTF-8 字节，Windows 控制台按 GBK 解释
- **解决**：printf 用英文（推荐，和以后串口调试一致）/ `-fexec-charset=GBK` / `chcp 65001`
- **教训**：**数字对、只有中文乱 → 一定是编码问题，不是逻辑问题**

---

## 六、两条心法

1. **看不懂代码时，不要盯着它"想"，要"手动跑程序"。** 拿一张纸，把每一行执行一遍，
   写下每一次 i / j / min_index / 数组内容。走三遍，理解自己就长出来了。
2. **卡住的时候，先加 printf 打印中间状态。** 这比盯着代码猜快十倍，而且这正是
   简历里"我怎么排查问题"能写出来的真实经历。
