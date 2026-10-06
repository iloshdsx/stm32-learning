@echo off
>nul 2>&1 chcp 936
setlocal

set "MK=D:\STM32\CubeIDE_1x\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.2.0.202409170845\tools\bin"
set "GTOOLS=D:\STM32\CubeIDE_1x\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.win32_1.0.0.202411081344\tools\bin"
set "PROG=D:\STM32\CubeIDE_1x\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.2.200.202503041107\tools\bin\STM32_Programmer_CLI.exe"

rem ---- 编译目录（自动扫描版，和 CubeIDE 的 Debug 目录互不干扰）----
set "BUILD=D:\STM32\test_led_build"
set "ELF=%BUILD%\test_led.elf"

set "PATH=%GTOOLS%;%MK%;%PATH%"

echo.
echo ==========================================
echo    STM32 一键 编译 + 烧录   (自动扫描版)
echo ==========================================
echo.

if not exist "%BUILD%" mkdir "%BUILD%"

echo [1/2] 编译中 ... 请稍等
"%MK%\make.exe" -C "%BUILD%" -f Makefile -j8
if errorlevel 1 goto BUILD_FAIL

echo.
echo [2/2] 烧录中 ... 请稍等
"%PROG%" -c port=SWD -w "%ELF%" -v -rst
if errorlevel 1 goto FLASH_FAIL

echo.
echo ==========================================
echo    成功 ! 看板子上的绿灯
echo ==========================================
echo.
pause
exit /b 0

:BUILD_FAIL
echo.
echo ********** 编译失败 **********
echo 把上面的报错内容发给我，我帮你看
echo.
pause
exit /b 1

:FLASH_FAIL
echo.
echo ********** 烧录失败 **********
echo 检查这几点
echo    1. 板子上的红灯亮着吗（供电正常吗）
echo    2. SWD 那 4 根线插对了吗
echo    3. ST-Link 插好了吗
echo.
pause
exit /b 1
