本项目基于 STM32CubeMX 生成 CMake 工程（GCC）。

使用前，请使用 CubeMX 重新生成代码。

# 基于 FreeRTOS 的多传感器采集与显示系统

## 📌 项目简介
本项目基于 STM32F407VET6 微控制器与 FreeRTOS 实时操作系统，搭建了一套传感器数据采集与显示系统。实现了 NTC 温度采集与 VL53L0X 激光测距的高频并发处理，并将测量数据解耦渲染于 LCD 屏幕上。

## 🛠️ 硬件与外设配置
- **主控芯片：** STM32F407VET6 (ARM Cortex-M4)
- **操作系统：** FreeRTOS (版本：v202212.01 手动移植)
- **传感器：** VL53L0X (ToF 激光测距，I2C 协议)、NTC 热敏电阻 (温度采集)
- **显示与交互：** 2.8寸 LCD 显示屏 (硬件SPI)、独立按键 (GPIOA)
- **底层驱动与硬件外设：** ADC + DMA 循环采样、软件模拟 I2C、GPIO 状态机

## 🚀 软件架构与核心亮点

1. **数据与 UI 彻底解耦（生产者-消费者模型）**
   - 自定义 `Display_t` 结构体，结合 `enum` 标记设备类型、`union` 压缩内存（共用存储 `int` 距离与 `float` 温度）。
   - 采用 FreeRTOS **消息队列（`Queue`）** 将传感器采集任务与 LCD 显示渲染任务彻底分离，降低任务耦合度。

2. **共享 I2C 总线互斥保护**
   - 针对 其他设备 与 VL53L0X 传感器共享同一 I2C 总线可能导致的并发冲突与死机问题，使用 FreeRTOS **互斥量 (`Mutex`)** 建立保护，确保总线操作的线程安全。

3. **ADC + DMA 静默高频采集**
   - 配置 ADC 配合 DMA 循环模式 进行 NTC 模拟信号后台静默采样，无需 CPU 频繁干预；基于 B值法转换公式进行温度计算。

4. **模块化按键驱动设计**
   - 采用指针传递替代局部 `static` 变量，消除多实例调用时的内存冲突，实现按键边沿检测与状态更新。

## 📁 目录结构说明
```text
├── Core/                  # 核心系统代码（由 STM32CubeMX 生成：main.c、中断、时钟配置等）
├── BSP/                   # 板级支持包（外设驱动：LCD屏、NTC温度采样、按键、VL53L0X激光测距等）
├── Drivers/               # STM32F4xx HAL 库及底层硬件驱动
├── FreeRTOS/              # FreeRTOS 实时操作系统源码及接口文件
├── Middlewares/           # 中间件（包含 ST USB Device 等官方组件）
├── lvgl/                  # LVGL v9 图形用户界面库
├── CMakeLists.txt         # CMake 主构建脚本
├── lvgl_project.ioc       # STM32CubeMX 工程配置文件
├── STM32F407.svd          # 芯片外设寄存器描述文件，用于在 IDE 调试时查看外设寄存器状态。
├── STM32F407XX_FLASH.ld   # 链接脚本（Linker Script），定义了 STM32F407 的 Flash 和 RAM 内存空间分配布局。
├── startup_stm32f407xx.s  # 汇编启动文件，负责芯片上电后的初始化、中断向量表设置并跳转到 main 函数。
└── README.md              # 项目说明文档
