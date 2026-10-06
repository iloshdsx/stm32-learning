# VSCode 写 STM32 代码：智能提示配置（复用模板）

## 这个文件是干什么的

`c_cpp_properties.json` 告诉 VSCode 的 C/C++ 扩展三件事：

1. 用哪个编译器（必须指到 **arm-none-eabi-gcc**，不能用普通 gcc）
2. 头文件去哪找（HAL / CMSIS 那 4 个 `-I` 路径）
3. 定义哪些宏（`USE_HAL_DRIVER`、`STM32F103xB`、`DEBUG`）

不配的后果：`#include "stm32f1xx_hal.h"` 下面全是红波浪线，`HAL_GPIO_WritePin` 点不进去。

## 怎么用（新项目三步）

1. 在 CubeIDE 里生成好工程（会自动产生 `Core/`、`Drivers/` 文件夹）
2. 用 VSCode **打开工程文件夹本身**（File → Open Folder → 选 `xxx.ioc` 所在那层）
3. 把本目录的 `c_cpp_properties.json` 复制到工程下的 `.vscode\` 文件夹里

> 新建 `.vscode` 文件夹时注意前面有个点，Windows 默认会隐藏它。

## 换芯片要改哪里

| 换的东西 | 要改的行 |
|---|---|
| 换芯片型号（如 F411） | `STM32F103xB` → `STM32F411xE`；`-mcpu=cortex-m3` → `cortex-m4`；include 里的 `STM32F1xx` → `STM32F4xx` |
| 换 CubeIDE 版本 | `compilerPath` 里的版本号目录名，去 `D:\STM32\CubeIDE_1x\STM32CubeIDE\plugins\` 里找实际的 `gnu-tools-for-stm32.*` 文件夹 |

## 注意

- VSCode 只负责**写代码 + 智能提示**，编译/烧录/断点调试仍然回 CubeIDE 做（F11）
- 改了配置后，VSCode 右下角会提示重载，点 Reload 即可生效
- 工程里的 `.ioc` 才是配置源头，换编辑器不影响它