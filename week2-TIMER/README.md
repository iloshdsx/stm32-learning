# 第 2 周 · 第 1 课：定时器中断（TIM2，1ms 心跳）

## 1. 定时器到底是什么

一句话：**一个自己会数数的计数器，数到点就"敲你一下"。**

- 数数的速度由两个旋钮决定：`PSC`（多久数一次）、`ARR`（数到几算满）
- 数满的那一刻 → 触发一次中断 → 进回调函数
- 你只要管"数满了我要干什么"，不用管它怎么数

## 2. 三种"等时间"的方式（对比第 1 周的长按）

| 方式 | 写法 | 主循环能同时干别的吗 | 什么时候用 |
|---|---|---|---|
| 阻塞等 | `HAL_Delay(500)` | ❌ 死等，整个程序停住 | 上电初始化、随便玩玩 |
| 轮询 | `HAL_GetTick()` 查表 | ✅ 可以，但主循环每圈都得问一句 | 单任务、逻辑简单 |
| 定时器中断 | TIM + 回调函数 | ✅ 系统主动来敲门 | 要精确定时、要同时干很多事 |

第 1 周长按用轮询能做，是因为当时只有一个按键。
**但要"1ms 精确"、"同时管按键 + 刷灯 + 读传感器"，就必须上定时器。**

## 3. 公式（记住这一个就够）

```
中断频率 = 定时器时钟 ÷ (PSC + 1) ÷ (ARR + 1)
```

本项目实测：

| 项 | 值 | 从哪来 |
|---|---|---|
| TIM2 时钟 | 8 MHz | Clock Configuration 里 "APB1 Timer clocks"（因为现在没开 PLL，SYSCLK = 8 MHz） |
| PSC | 7 | 8 MHz ÷ (7+1) = 1 MHz → 每 1 µs 数一次 |
| ARR | 999 | 1 MHz ÷ (999+1) = 1000 Hz → 每 1 ms 数满一次 |

验算：`8 000 000 ÷ 8 ÷ 1000 = 1000 Hz = 1 ms` ✅

> 为什么是 `PSC+1`、`ARR+1`？因为寄存器从 **0** 开始数。
> `PSC = 7` 表示"数 0,1,2,...,7 共 8 个数"，所以是 ÷8。

**想换频率只改 ARR**：ARR = 9999 → 100 µs；ARR = 9 → 100 µs... 自己算：
`ARR = 1 000 000 ÷ 想要的频率(Hz) - 1`（前提是 PSC 已经把它降到 1 MHz）

## 4. CubeMX 配置（4 步）

1. 打开 `D:\STM32\CubeIDE_1x\test_led\test_led.ioc`
2. 左边 `Timers` → `TIM2` → `Clock Source` 选 **`Internal Clock`**
3. 下面 `Configuration` → `Parameter Settings` → `Counter Settings`：
   - `Prescaler (PSC)` = **7**
   - `Counter Mode` = `Up`
   - `Counter Period (ARR)` = **999**
4. 切到 `NVIC Settings` 标签 → 勾上 **`TIM2 global interrupt`** 的 `Enabled`
5. 右上角 `GENERATE CODE`

> 放心 Generate：`/* USER CODE BEGIN */` 和 `/* USER CODE END */` 之间的东西不会被冲掉。

## 5. 代码（3 处要填）

### 5.1 启动定时器 —— `USER CODE BEGIN 2` 区

```c
/* USER CODE BEGIN 2 */
  EXTI0_Init();                 /* 第 1 周留下的 */
  HAL_TIM_Base_Start_IT(&htim2); /* 启动 TIM2，并且"数满就进中断" */
/* USER CODE END 2 */
```

`&htim2` 也是 CubeMX 自动生成的句柄，不用自己声明。

### 5.2 数满后干什么 —— `USER CODE BEGIN 4` 区

```c
/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)      /* 空1：哪个定时器数满了？ */
  {
    static uint32_t cnt = 0;
    cnt++;
    if (cnt >= 1000)               /* 空2：1 ms × 多少次 = 1 秒？ */
    {
      cnt = 0;
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
  }
}
/* USER CODE END 4 */
```

要点：
- `static` 和中断里那课一样：这个变量在两次中断之间会**保留自己的值**
- 回调函数名 `HAL_TIM_PeriodElapsedCallback` 是 HAL 规定死的，写错名字就永远进不来
- 中断里**不要用 `HAL_Delay`**（第 1 周讲过原因）

## 6. 验证方法（别用眼睛猜）

改完烧进去后：

1. 手机打开秒表
2. 从灯**刚亮**的那一瞬开始数
3. 数到 **10 秒**时，灯应该翻了 **5 次**（亮 1 秒 + 灭 1 秒 = 2 秒一个完整循环）

如果数出来是 5 次 → 定时器精确定时成功 ✅
如果快了/慢了一倍 → 回去检查 PSC/ARR 有没有填错，或者时钟是不是 8 MHz

## 7. 这一课之后能干什么

- **PWM 呼吸灯**：把同一个 TIM2 换个通道，让 LED 亮度自己渐变
- **1ms 扫按键**：把第 1 周的按键搬进这个 1ms 中断里 → 长按连发不再需要主循环轮询
- **非阻塞万能模板**：以后所有"每隔 N 毫秒做一次"的活，都往这个回调里加
