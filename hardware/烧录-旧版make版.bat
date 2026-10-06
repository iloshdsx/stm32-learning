@echo off
>nul 2>&1 chcp 936
setlocal

set "GTOOLS=D:\STM32\CubeIDE_1x\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.win32_1.0.0.202411081344\tools\bin"
set "MAKE=D:\STM32\CubeIDE_1x\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.2.0.202409170845\tools\bin"
set "PROG=D:\STM32\CubeIDE_1x\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.2.200.202503041107\tools\bin\STM32_Programmer_CLI.exe"
set "PROJ=D:\STM32\CubeIDE_1x\test_led"
set "ELF=%PROJ%\Debug\test_led.elf"
set "PATH=%GTOOLS%;%MAKE%;%PATH%"

echo.
echo ==========================================
echo    STM32  一键 编译 + 烧录
echo ==========================================
echo.

echo [1/2] 编译中 ... 请稍等
"%MAKE%\make.exe" -C "%PROJ%\Debug" all -j8
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
echo 检查这几样：
echo   1. 板子上的红灯亮着吗（供电正常）
echo   2. SWD 的 4 根线插满了吗
echo   3. ST-Link 插稳了吗
echo.
pause
exit /b 1
