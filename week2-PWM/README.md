# 第 2 周 · 第 2 课：PWM 呼吸灯

> **两个版本，先做 A**
> - **版本 A（软件 PWM）**：零外接器件，用板载 PC13 那颗灯。现在就能做。
> - **版本 B（硬件 PWM）**：等电阻到了，在面包板上插外接 LED（PA6），对比用。
>
> 先做 A 有个好处：你会**亲眼看到软件 PWM 有多费 CPU**，然后才明白为什么
> 每颗 MCU 都专门做了硬件 PWM —— 这个"为什么"比直接会用更值钱。

---

# 版本 A：软件 PWM（现在就能做）

## A1. PWM 是什么

一句话：**在一个固定周期里，让"高电平"占多少比例可以调。**

| 参数 | 意思 | 版本 A 的值 |
|---|---|---|
| **频率** | 一秒钟多少个周期 | 100 Hz |
| **占空比 duty** | 一个周期里高电平占多少 | 0 ~ 100 可调 |

LED 就是最好的"人眼积分器"：它跟不上 100 Hz 的开关，只看到**平均值**。

| 占空比 | 眼睛看到 |
|---|---|
| 100 | 最亮 |
| 50 | 一半亮 |
| 10 | 很暗 |
| 0 | 灭 |

**所谓呼吸灯，不是慢慢调电压，而是飞快地开关，改"开"占的比例。**

## A2. 拆开算：频率从哪来，步数从哪来

```
TIM3 时钟 8 MHz ÷ (PSC+1)=8  →  1 MHz      （每 1 µs 进一次中断）
1 MHz ÷ (ARR+1)=100          →  10 kHz     （中断频率）
10 kHz ÷ 100 步              →  100 Hz     （PWM 频率）
```

所以这一课要配：

| 项 | 值 | 为什么 |
|---|---|---|
| `PSC` | **7** | 把 8 MHz 降到 1 MHz |
| `ARR` | **99** | 每 100 次中断当"一个 PWM 周期" |
| 软件里的步数 | **100** | 和 ARR+1 一致，每步 1% |

## A3. CubeMX 配置

1. 打开 `test_led.ioc`
2. `Timers` → `TIM3` → `Clock Source` 选 **`Internal Clock`**
3. `Parameter Settings`：`Prescaler` = **7**，`Counter Period (ARR)` = **99**
4. `NVIC Settings` 标签 → 勾上 **`TIM3 global interrupt`** 的 `Enabled`
5. `GENERATE CODE`

> **不要配 Channel。** 软件 PWM 是我们自己在中断里手动拉 GPIO，用不到定时器输出通道。

## A4. 代码

### ① 全局变量 —— `USER CODE BEGIN PV` 区

```c
/* USER CODE BEGIN PV */
volatile int32_t duty = 50;   /* 占空比 0~100。两个中断都要碰它 → 加 volatile */
/* USER CODE END PV */
```

> **为什么要 volatile？** 这个变量被**中断**改、被**另一个中断**读。
> 编译器看不见"中断随时会改它"，可能把它缓存进寄存器。
> 加 `volatile` 就是告诉编译器：**每次都要老老实实回内存读**。

### ② 启动定时器 —— `USER CODE BEGIN 2` 区

```c
/* USER CODE BEGIN 2 */
  EXTI0_Init();
  HAL_TIM_Base_Start_IT(&htim2);   /* 1ms 心跳（上一课） */
  HAL_TIM_Base_Start_IT(&htim3);   /* 10kHz，软件 PWM 用 */
/* USER CODE END 2 */
```

### ③ 两个中断的分工 —— `USER CODE BEGIN 4` 区

```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* ================= TIM2：每 1ms 一次 ================= */
  if (htim->Instance == TIM2)
  {
    static uint32_t cnt = 0;
    cnt++;
    if (cnt >= 1000)
    {
      cnt = 0;
      /* HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);   <-- 先注释掉！ */
    }

    static uint32_t t10 = 0;
    static int32_t  step = 1;

    if (++t10 >= 10)                     /* 每 10ms 动一下占空比 */
    {
      t10 = 0;
      duty += step;
      if (duty >= ____) { duty = ____; step = -1; }   /* 空1 */
      if (duty <= ____) { duty = ____; step =  1; }   /* 空2 */
    }
  }

  /* ================= TIM3：每 0.1ms 一次（软件 PWM 本体）================= */
  if (htim->Instance == TIM3)
  {
    static uint8_t pwm_cnt = 0;
    pwm_cnt++;
    if (pwm_cnt >= ____) pwm_cnt = 0;    /* 空3：一个周期分成几份？ */

    if (pwm_cnt < duty)
      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  /* 亮（低电平点亮） */
    else
      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);    /* 灭 */
  }
}
```

**务必先注释掉那行 `TogglePin`** —— 它和软件 PWM 都在抢 PC13，两个一起写会打架。

## A5. 三个空的提示

- 空1 / 空2：占空比的**满值**和**灭掉**分别是多少？（用 0~100 这套刻度）
- 空3：`ARR = 99`，一个周期里中断进 100 次 → 分成几份？

## A6. 验证

1. `duty = 50` 固定不动 → 板载灯应该是**半亮**（比之前直接点亮明显暗）
2. 改成 `duty = 10` → 很暗；`duty = 100` → 全亮。**这就是 PWM**
3. 开呼吸（`duty` 自己动）→ 灯一呼一吸，约 **2 秒一个来回**
   （100 格 × 10 ms = 1 秒升 + 1 秒降）

## A7. 这一版的代价（重点）

软件 PWM 时，CPU **每秒要进 1 万次中断**，每次都要跑一遍 HAL 的分发 + 你的代码。
板子跑得动，但你想想：如果同时还要读传感器、刷屏幕、发串口，CPU 还剩多少？

**硬件 PWM 就是把这个活整个交给定时器自己做 —— CPU 一次都不用管。** 所以下一版。

## A8. 顺便用逻辑分析仪看波形

软件 PWM 是 **100 Hz**，比按键抖动慢得多，而且干净、规律、**不用触发器**：

采样率 `100 kHz`、采样数量 `100 k`（= 1 秒窗口）、只留 `D0`。
夹子夹 **C13**（板子上 PC13 那个脚）和 **GND**，点 Run 就有一串方波。

放大进去数：高电平占的格数 ÷ 一个周期的格数 = **占空比**。
呼吸的时候，你会看到这个比例自己在变 —— 这就是 PWM 的"底片"。

---

# 版本 B：硬件 PWM（等电阻到了再做）

## B0. 为什么不能用板载那颗灯

- 板载 LED 焊死在 **PC13** 上
- **PC13 没有定时器通道** → 出不了硬件 PWM（不是配置问题，是芯片没这根线）
- 所以版本 B 要在面包板上插一个**外接 LED**，接到 **PA6**（= `TIM3_CH1`）

接线（见 `PWM接线图.png`）：

```
PA6  ──[ 1 kΩ 电阻 ]──▶|──  GND
                        LED
```

- LED **长脚 = 正极**，接电阻那一侧；**短脚 = 负极**，接 GND
- 电阻不能省：LED 内阻很小，直接接 3.3V 电流会很大 → 烧 LED / 烧板子的脚
- 手边没 1 kΩ 也行：**470 Ω ~ 2.2 kΩ 都能用**，只是亮度不同

## B1. 三个寄存器

| 名字 | 管什么 | 版本 B 的值 |
|---|---|---|
| `PSC` | 数数的快慢 | 8 MHz ÷ (7+1) = **1 MHz** |
| `ARR` | 一个周期数几下 | 1 MHz ÷ (999+1) = **1 kHz**（PWM 频率） |
| `CCR` | 数到几翻转 | **占空比 = CCR ÷ (ARR+1)** |

`ARR = 999` 时满值是 `1000`：`CCR = 500` → 50%，`CCR = 100` → 10%。

> 版本 A 是"我们自己在中断里数 100 步"，
> 版本 B 是"定时器自己数 1000 步，还自动帮你拉引脚"。**原理一模一样。**

## B2. CubeMX 改动

> ⚠️ **TIM3 要改回来**：版本 A 把它配成了 `ARR = 99` + 开中断，
> 版本 B 要改成下面这样（中断可以关掉，不再需要）。

1. `TIM3` → `Channel1` 选 **`PWM Generation CH1`**
2. `Parameter Settings`：`Prescaler` = **7**，`Counter Period (ARR)` = **999**，
   `PWM Mode` = `PWM mode 1`，`Pulse (CCR1)` = **500**
3. `NVIC Settings` → `TIM3 global interrupt` 可以取消勾选
4. `GENERATE CODE`

## B3. 代码

`USER CODE BEGIN 2` 里把 TIM3 那行换掉：

```c
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);   /* 让 PA6 开始出 PWM */
```

删掉版本 A 里 TIM3 那段软件 PWM 代码和 `pwm_cnt`，
呼吸部分改成直接写寄存器：

```c
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);   /* duty: 0~1000 */
```

## B4. 对比着看（这才是版本 B 的价值）

| | 版本 A 软件 PWM | 版本 B 硬件 PWM |
|---|---|---|
| CPU 占用 | 每秒 1 万次中断 | **几乎 0** |
| 频率上限 | 低（受中断开销拖累） | 高（能到 MHz 级） |
| 抖动 | 有（中断会被别的活挤晚） | 极稳（定时器独立跑） |
| 谁在干活 | **CPU** | **定时器外设** |

**记住这张表，面试问"PWM 怎么实现"你就有东西讲了。**
