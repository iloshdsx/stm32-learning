# 第 1 周实验 ④：外部中断 EXTI

> 对应《第 1 周执行手册》Day 6。
> 前三个实验已完成：点灯 ✅　按键（点按 + 长按连发）✅　这一个：把按键从「轮询」改成「中断」。

## 一、先搞懂：轮询 vs 中断

**你现在做的（轮询 polling）**：主循环每 10ms 主动问一次「按键按了吗？」

```c
while (1)
{
  uint8_t now = HAL_GPIO_ReadPin(...);   /* 每 10ms 问一次 */
  ...
  HAL_Delay(10);
}
```

缺点：

- **CPU 被绑住了**，一刻不停地问
- 两次询问之间的空档里发生的事，**可能被漏掉**
- 按键按得比 10ms 还快，会整段错过

**现在要做的（中断 interrupt）**：CPU 平时该干嘛干嘛，**硬件发现电平变化就主动去「拍 CPU 的肩膀」**。

> 比喻：
> - 轮询 = 你每隔 10 秒跑到门口看一眼快递到没到
> - 中断 = 快递到了门铃响，你才起身去开门

## 二、信号是怎么从引脚走到 CPU 的

```
PA0 电平变化
   |
AFIO（引脚复用/重映射的「交换机」：把「第 0 号引脚」接到「第 0 号中断线」）
   |
EXTI0（外部中断控制器的一条线，负责判断上升沿 / 下降沿）
   |
NVIC（嵌套向量中断控制器：决定「允不允许打断」「谁先谁后」）
   |
CPU 放下手里的活 -> 跳到 EXTI0_IRQHandler -> 执行完再回来
```

### 两个必须记住的点

**1. EXTI 线是按「引脚编号」分的，不是按「端口」分的。**

| 中断线 | 挂着哪些引脚 |
|---|---|
| EXTI0 | **PA0**、**PB0**、PC0 … |
| EXTI1 | PA1、PB1、PC1 … |

所以 **PA0 和 PB0 不能同时做外部中断**——它们抢同一根 EXTI0 线。
（面试原题：「STM32 里 PA0 和 PB0 能同时做外部中断吗？」答：不能，共用 EXTI0。）

**2. F1 系列必须开 AFIO 时钟。** 别的系列不一定，F1 一定要，忘了就永远进不了中断。

## 三、要改的 4 个地方（`main.c`）

> 文件：`D:\STM32\CubeIDE_1x\test_led\Core\Src\main.c`

### 改动 1：在 `/* USER CODE BEGIN 0 */`（第 56 行）下面加两个函数

```c
/* USER CODE BEGIN 0 */

/**
  * @brief  把 PA0 配成「下降沿触发的外部中断」
  */
static void EXTI0_Init(void)
{
  GPIO_InitTypeDef gpio = {0};

  /* ① 开 AFIO 时钟 —— F1 上 EXTI 的引脚映射归它管，不开就进不了中断 */
  __HAL_RCC_AFIO_CLK_ENABLE();

  /* ② 把 PA0 配成「输入 + 下降沿中断」模式
        按键上拉接法：按下是 1->0，也就是「下降沿」 */
  gpio.Pin  = BTN_Pin;
  gpio.Mode = GPIO_MODE_IT_FALLING;
  gpio.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BTN_GPIO_Port, &gpio);

  /* ③ 在 NVIC 里打开 EXTI0 这条中断线，并给它一个优先级 */
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/**
  * @brief  EXTI0 的中断服务函数（ISR）
  *         CubeMX 没生成它，必须自己写，否则会跳到启动文件里的死循环！
  */
void EXTI0_IRQHandler(void)
{
  /* 交给 HAL 处理：它负责清掉「中断标志位」，再调用 HAL_GPIO_EXTI_Callback */
  HAL_GPIO_EXTI_IRQHandler(BTN_Pin);
}

/* USER CODE END 0 */
```

### 改动 2：第 92～94 行（`last` / `press_tick` / `long_pressed`）删掉，改成

```c
  EXTI0_Init();     /* 把 PA0 交给中断，主循环不再管按键 */
```

> 那三个变量不用了，留着会报 `unused variable` 警告。

### 改动 3：第 104～131 行（`while (1)` 里的按键逻辑）全删掉

删完 `while (1)` 里是空的（**第 132、133 行那两个 `}` 留着，别删**）：

```c
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* 按键交给中断了，主循环现在闲着 */
    /* USER CODE END 3 */
  }
```

### 改动 4：在 `/* USER CODE BEGIN 4 */`（第 209 行）下面加回调函数

```c
/* USER CODE BEGIN 4 */

/**
  * @brief  中断真正要干的活写在这里
  *         注意：它在「中断上下文」里跑，必须短，绝对不能用 HAL_Delay！
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BTN_Pin)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
}

/* USER CODE END 4 */
```

## 四、烧录后会发现一个「问题」——这是重点，别慌

按下一次按键，灯**可能翻转好几次**，也可能出现「按一下看着没变」。

**原因：机械抖动。**

轮询版之所以没暴露，是因为 `HAL_Delay(10)` 天然把抖动过滤掉了（10ms 才看一眼）。
中断版是**电平一变就触发**，触点抖动产生的那些毛刺，全部被当成「按了好几下」。

**这是好事**——它让你第一次亲眼看见「抖动到底是什么」。

下一课解决：在中断回调里加一个时间戳，200ms 内重复触发就 `return` 掉（软件消抖）。

## 五、第二步：在中断里加软件消抖

### 5.1 先理解「中断版的记忆」怎么写

轮询版我们把「上一次的状态」存在 `while` 外面的局部变量里。中断版没有主循环可依靠，
每一次中断进来，函数里的**普通局部变量都会重新创建**，记不住上次的事。

办法是用 **`static` 局部变量**：

```c
static uint32_t last_irq = 0;   /* 加了 static，这个变量在两次中断之间会"保留自己的值" */
```

| 写法 | 每次函数调用时 | 用途 |
|---|---|---|
| `uint32_t x;` | 重新创建，值不确定 | 临时算一算 |
| `static uint32_t x;` | **沿用上次的值** | 记住"上次发生了什么" |

> 记法：**`static` = 函数里的"长期记忆"。** 它只初始化一次，之后一直活着。

### 5.2 消抖代码

```c
/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint32_t last_irq = 0;      /* 上次中断的时刻 */

  if (GPIO_Pin == BTN_Pin)
  {
    uint32_t now = HAL_GetTick();

    /* 距上次中断不到 50ms -> 判定为触点抖动，丢掉这一次 */
    if (now - last_irq < 50)
    {
      return;
    }

    last_irq = now;
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
}
/* USER CODE END 4 */
```

### 5.3 为什么阈值取 50ms

| 数值 | 说明 |
|---|---|
| 机械抖动持续时间 | 通常 **5～20ms**，最差到 50ms |
| 人最快的连按间隔 | 约 100～150ms |

所以取 **20～50ms**：足够盖住抖动，又不会把人的正常连按吞掉。
取太大（比如 500ms）会出现「按了没反应」的感觉。

### 5.4 为什么这里用 `HAL_GetTick()` 而**绝不能**用 `HAL_Delay(50)`

这是今天第二个必背的知识点：

```
SysTick 中断的优先级  = 15（最低）
EXTI0 中断的优先级    = 2（比 SystTick 高）
```

EXTI0 的 ISR 正在跑的时候，**SysTick 中断进不来**，于是 `uwTick` 这个毫秒计数器**停止增长**。
在 ISR 里调 `HAL_Delay(50)`，它等的那个"50ms"永远不会到来 → **死等，整块板子卡死**。

`HAL_GetTick()` 只是**读一个变量**，不等待，所以在中断里用是安全的。

> 一句话：**中断里只能"看时间"，不能"等时间"。**

## 六、今天要能回答（面试会问）

- 轮询和中断的本质区别？
  - 轮询是 CPU 主动查（占 CPU、可能漏事件）；中断是硬件事件驱动（不占 CPU、响应快）。
- 按键该配上升沿还是下降沿？
  - 上拉接法（平时高、按下低）→ **下降沿** = 按下的瞬间。下拉接法反之。
- 为什么中断服务函数里不能用 `HAL_Delay`？
  - 它靠 SysTick 中断计数，而中断里默认不能被打断，会**死等**；而且 ISR 要短，长时间霸占会拖垮整个系统的实时性。
- `EXTI0_IRQHandler` 里为什么要调 `HAL_GPIO_EXTI_IRQHandler`？
  - 必须**清中断标志位**，否则退出中断后标志还在，会立刻再进一次，无限循环。
