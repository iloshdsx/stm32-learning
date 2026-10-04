# 第 1 周实验：GPIO 点灯 / 按键 / 串口 / 外部中断

日期：2026-10-05 ~ 10-11　开发板：STM32F103C8T6 最小系统板（蓝药丸）
IDE：STM32CubeIDE 1.19.0　固件包：STM32Cube FW_F1 V1.8.7

## 实验清单

| 实验 | 文件/工程 | 做了什么 | 状态 |
|---|---|---|---|
| 1. 点灯 | `LED/` | PC13 板载 LED 每 500ms 翻转一次 | ✅ 代码 + 编译通过（板子未到，待上机） |
| 2. 按键 | `KEY/` | 按键边沿检测 + 20ms 软件消抖，按一下翻转 LED | ✅ 代码 + 编译通过（板子未到，待上机） |
| 3. 串口 | `UART/` | USART1 收发 + printf 重定向到串口助手 | ⬜ |
| 4. 外部中断 | `EXTI/` | 按键触发 EXTI 中断翻转 LED | ⬜ |

## 接线记录

| 功能 | MCU 引脚 | 接到哪里 | 备注 |
|---|---|---|---|
| 板载 LED | PC13 | 板载 | **低电平点亮**（PC13 给 0 亮、给 1 灭） |
| 按键 | PA0 | 按键一端；按键另一端接 GND | 内部上拉，故**松开=1、按下=0** |
| ST-Link SWDIO | PA13 | ST-Link SWDIO | |
| ST-Link SWCLK | PA14 | ST-Link SWCLK | |
| 串口 TX | PA9 | CH340 RX | 交叉连接 |
| 串口 RX | PA10 | CH340 TX | 交叉连接 |
| 共地 | GND | CH340 GND / ST-Link GND | 必须共地 |
| 启动模式 | BOOT0 | GND | **必须接 0**，否则程序不跑 |

## 关键代码：按键边沿检测 + 消抖

```c
/* USER CODE BEGIN 2 */
/* 初值取开机那一刻的真实电平，避免"上电时手刚好按着键"被误判成按下 */
uint8_t last = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin);
/* USER CODE END 2 */

    /* USER CODE BEGIN 3 */
    uint8_t now = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin);

    if (last == GPIO_PIN_SET && now == GPIO_PIN_RESET)   /* 1 → 0 = 按下那一瞬 */
    {
      HAL_Delay(20);                                     /* 等抖动过去 */

      if (HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin) == GPIO_PIN_RESET)
      {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);      /* 再确认仍是 0 才翻转 */
      }
    }

    last = now;
    HAL_Delay(10);
    /* USER CODE END 3 */
```

三句话讲清：
1. **记忆上次值**（`last`）才能发现"变化"，只看当前电平只能做"按住亮、松手灭"。
2. **按下 = 1→0**（上拉 + 接地），别把 `Reset` 当成"松手"，它说的是引脚电压 0V。
3. **抖动 5~20ms**，一次按下会被读成多次；延时 20ms 再读一次确认，就能挡住。

## 本周遇到的问题与解决

### 1. 期望"按住亮"，实际按住灭、松手亮
- **现象**：亮灭和预期完全相反
- **排查**：先分离硬件和软件——推出"手指动作 → PA0 电平"对照表
- **原因**：`if (HAL_GPIO_ReadPin(...))` 只问"读到的是不是非 0"。上拉接法下松开=1，所以条件成立发生在**松开**时
- **解决**：条件写成 `== GPIO_PIN_RESET`

### 2. 做边沿检测时，动作跑到"松手"上了
- **现象**：按下没反应，一松手灯变
- **原因**：条件写成 `last == 0 && now == 1`（0→1，上升沿=松手）；按下是 1→0（下降沿）
- **解决**：`last == GPIO_PIN_SET && now == GPIO_PIN_RESET`

### 3. 一次按下灯乱翻，偶尔"按一下没反应"
- **现象**：有时翻 2 次（看起来没变）、有时 3 次
- **原因**：机械按键触点弹跳 5~20ms；**偶数次翻转 = 回到原样**，所以偶发
- **解决**：检测到下降沿后 `HAL_Delay(20)` 再读一次确认

### 4. `last` 的初值怎么给
- **现象**：一开始想写 `uint8_t last = 0`
- **原因**：开机没按时 PA0 读到 1；写 0 虽能靠第一圈自我修正，但"上电时手正按着键"会漏判
- **解决**：`uint8_t last = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin);`

### 5. 变量写在 `while(1)` 里面
- **现象**：边沿永远检测不到
- **原因**：局部变量每圈重新创建，`last` 恒等于当圈读到的值
- **解决**：定义在 `while(1)` 外，且放在 `USER CODE BEGIN 2` 里（框外会被 CubeMX 重新生成时冲掉）

## 编译产物（按键版）

```
text    data    bss     dec     hex
4748      12   1572    6332    18bc   test_led.elf
```
比纯点灯版（text 4592）只多 156 字节。