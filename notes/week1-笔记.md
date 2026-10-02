# 第 1 周笔记

> 时间：2026-10-02（Day 0，提前开工）～ 10-11（第 1 周结束）
> 目标：在 STM32F103C8T6 最小系统板上跑通 点灯 / 按键 / 串口 / 外部中断

---

## 一、Day 0（10-02）：环境搭建 + 第一个工程

### 1. 环境清单

| 项目 | 内容 |
|---|---|
| IDE | STM32CubeIDE **1.19.0**，装在 `D:\STM32\CubeIDE_1x\STM32CubeIDE` |
| 工作区 workspace | `D:\STM32\CubeIDE_1x`（不规范但能用，以后可改） |
| 固件包 | STM32Cube MCU Package for STM32F1 Series **V1.8.7**（163 MB） |
| 测试工程 | `D:\STM32\CubeIDE_1x\test_led`，芯片 STM32F103C8Tx |
| 工具链 | arm-none-eabi-gcc 13.3 + make（都是 IDE 自带的，不用另装） |

### 2. 点灯工程完整流程（以后照这个走）

1. `File → New → STM32 Project` → 选芯片 `STM32F103C8Tx`
2. 在 `.ioc` 引脚图上点 **PC13** → 选 `GPIO_Output`
3. 给 PC13 设名字：右键引脚 → `Enter User Label` → 输入 `LED`
4. `Ctrl + S` → 弹窗问是否生成代码 → 选 `Yes`
5. 打开 `Core/Src/main.c`，在下面两行**之间**写代码：

```c
  /* USER CODE BEGIN 3 */
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(500);
  /* USER CODE END 3 */
```

6. `Ctrl + B` 编译 → 出现 `Build Finished. 0 errors` 就成功

### 3. 关键理解

- **`USER CODE BEGIN / END` 是 CubeMX 的"保留区"**：写在里面的代码，改 `.ioc` 重新生成时不会被删；写在框外会被冲掉。
- **PC13 是蓝药丸的板载 LED，低电平点亮**（给 0 亮、给 1 灭）。`.ioc` 里 `GPIO output level = Low` 这个默认值正好就是"点亮"。
- 编译产物：`Debug/test_led.elf`（真正要烧进芯片的文件）、`test_led.map`、`test_led.list`（看内存分配和反汇编用）。
- 本次代码体积：Flash 4588 字节，RAM 1572 字节。F103C8T6 有 64KB Flash / 20KB RAM，够用。

---

## 二、知识点

### 1. GPIO
- 8 种模式：输入（浮空/上拉/下拉/模拟）、输出（推挽/开漏/复用推挽/复用开漏）
- 点灯三步：使能时钟 → 配置模式 → 写 ODR
- `HAL_GPIO_WritePin(port, pin, state)` 写电平；`HAL_GPIO_TogglePin(port, pin)` 翻转电平

### 2. 电路基础
- 欧姆定律 U = I × R
- LED 限流电阻：R = (Vcc − Vf) / I
- 上拉/下拉电阻常用 4.7k ~ 10k

### 3. Linux 命令
- ls / cd / pwd / cp / mv / rm / mkdir
- chmod / grep / 管道 |
- 👉 完整命令速查表见 [Linux命令速查.md](Linux命令速查.md)

---

## 三、踩过的坑（现象 → 原因 → 解决）

| 现象 | 原因 | 解决 |
|---|---|---|
| `Help → STM32Cube updates → Check for Embedded Software Packages Updates` 打开的窗口里列表是空的，说"已是最新"，但工程其实缺 F1 固件包 | 那个窗口只负责"查有没有新版本"，不负责"安装你缺的包"，所以缺包它也不显示 | 真正入口是 `Help → Configuration Tool → Manage Embedded Software Packages`，展开 `STM32F1`，勾 `STM32Cube MCU Package for STM32F1 Series`，再点 `Install` |
| CubeIDE 2.2.0 建工程时找不到"选芯片"的经典向导 | 2.x 把经典向导禁用了（解包 `fragment.xml` 看到整段被 HTML 注释掉），只能建 CMake 空工程，没有 HAL 和 `.ioc` | 换回 1.19.0，经典向导还在 |
| 编译报错 `expected '=', ',', ';', 'asm' or '__attribute__' before '.' token` | `main.c` 第 132 行行首多打了一个字母 `S` | 删掉多余的字符 |
| 编译报错 `invalid storage class for function 'MX_GPIO_Init'` + `expected declaration or statement at end of input` | `main()` 结尾的 `}` 被误删，后面的函数被编译器当成写在 `main()` 里面 | 补上 `}`，让 `{` `}` 配平 |
| 编译 0 errors，但灯不亮（板子还没到，先记着） | 代码写在 `while (1) { }` 的**外面**，属于"永远执行不到的死代码" | 挪进 `while (1)` 大括号内，且写在 `USER CODE BEGIN 3` 之后 |

---

## 四、编译报错"为什么错"（重点复盘）

### 坑 1：行首多了一个字母

报错原文：
```
error: expected '=', ',', ';', 'asm' or '__attribute__' before '.' token
  132 | S  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
```
为什么错：
- C 语言里每条语句以 `;` 结尾，编译器从行首往后一个词一个词地读。
- 这行开头多了个 `S`，编译器先看到 `S`，把它当成一个"名字"（标识符）；
- 紧接着又是一个名字 `RCC_ClkInitStruct`，两个名字挨在一起、中间既没有运算符也没有分号 → 它读不懂。
- 于是它说："在 `.` 这个符号之前，我期待的是 `=`、`,`、`;` 之类的东西。"
- **人话翻译**：这一段语法断了，你自己往前看看是不是多打了字符。→ 删掉 `S` 就通了。

补充：明明只错一处，却报 4 个 error —— C 编译器一次会把能发现的都报出来，**第一条是根因，后面多是连锁反应**。所以**永远先看第一条 error 的行号**。

### 坑 2：行尾少了一个 `}`

报错原文：
```
error: invalid storage class for function 'MX_GPIO_Init'
  144 | static void MX_GPIO_Init(void)
error: expected declaration or statement at end of input
  186 | }
```
为什么错：
- `main()` 结尾的 `}` 被一起删掉了；
- 编译器于是认为后面的 `SystemClock_Config`、`MX_GPIO_Init` 这些函数是写在 `main()` **内部**的；
- C 语言**不允许函数套函数**：`static` 出现在函数体内部的位置上，编译器不认识 → 报 `invalid storage class`（"存储类"用在了不该出现的地方）；
- 同时它读文件读到末尾，还没等到本该闭合的那个 `}` → 报 `expected declaration or statement at end of input`（"一直到输入结束都没读到该有的声明或语句"）。
- **判断技巧**：这两条一起出现，99% 是**少了一个大括号**。查法：数 `{` 和 `}` 个数是否相等；或把光标点在一个 `}` 上，IDE 会高亮与它配对的 `{`；缩进突然变乱也是信号。

### 坑 3：编译通过，但灯不会亮（最隐蔽的一类）

错误写法：
```c
  while (1)
  {
    /* USER CODE BEGIN 3 */
  }                                 /* ← 循环到这里就"转完了" */
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);   /* ← 永远执行不到 */
  HAL_Delay(500);
```
为什么错：
- `while (1) { }` 是**死循环**：程序进去以后就一直在里面转，不会往下走；
- 循环**外面**的语句语法完全合法，所以**编译是 0 errors**，但**永远轮不到执行**；
- 这叫"死代码"。**编译通过 ≠ 功能正确**，这类 bug 编译器帮不了你，只能靠想清楚程序怎么走。
- 正确位置：写在 `{ }` **里面**，并且在 `/* USER CODE BEGIN 3 */` 之后。

---

## 五、还讲不清楚的地方（下周补）

1. `HAL_Delay(500)` 的 500ms 是怎么数出来的？（SysTick 中断 / `HAL_IncTick`）
2. 时钟树：HSI / HSE / PLL 是什么，为什么默认 8MHz 能变成 72MHz
3. `GPIO_MODE_OUTPUT_PP`（推挽）和 `GPIO_MODE_OUTPUT_OD`（开漏）到底差在哪
4. `HAL_GPIO_TogglePin` 里面是怎么实现的（BSRR 寄存器）