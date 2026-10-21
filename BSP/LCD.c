#include "LCD.h"
#include "spi.h"

/*******************************
 *  LCD显示驱动（ILI9341显示芯片）
 *******************************/

// 定义一行 240 个像素的缓冲区 (480 字节)
static uint8_t line_buf[240 * 2];

/**
 * @brief  硬件 SPI 发送一个字节
 */
void LCD_SPI_SendByte(uint8_t data)
{
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
}

/**
 * @brief  写命令
 */
void LCD_WriteCommand(uint8_t cmd)
{
    LCD_DC(0); //  切换写命令模式

    LCD_CS(0);
    LCD_SPI_SendByte(cmd);
    LCD_CS(1);
}

/**
 * @brief 写数据
 */
void LCD_WriteData(uint8_t data)
{
    LCD_DC(1); // 切换写数据模式

    LCD_CS(0);
    LCD_SPI_SendByte(data);
    LCD_CS(1);
}

/**
 * @brief 硬件复位
 */
void LCD_Reset(void)
{
    LCD_RESET(0);
    HAL_Delay(1);
    LCD_RESET(1);
    HAL_Delay(120);
}

/**
 * @brief  初始化 ILI9341 显示屏
 */
void LCD_Init(void)
{
    LCD_CS(1);

    LCD_Reset();

    LCD_BACKLIGHT(1); // 点亮背光

    /* 发送 ILI9341 初始化指令集 */

    // 电源管理和时序方面
    // LCD_WriteCommand(0xCF); // Power Control B
    // LCD_WriteData(0x00);
    // LCD_WriteData(0xC1);
    // LCD_WriteData(0x30);

    // LCD_WriteCommand(0xED); // Power on sequence control
    // LCD_WriteData(0x64);
    // LCD_WriteData(0x03);
    // LCD_WriteData(0x12);
    // LCD_WriteData(0x81);

    // LCD_WriteCommand(0xE8); // Driver timing control A
    // LCD_WriteData(0x85);
    // LCD_WriteData(0x00);
    // LCD_WriteData(0x78);

    // LCD_WriteCommand(0xCB); // Power control A
    // LCD_WriteData(0x39);
    // LCD_WriteData(0x2C);
    // LCD_WriteData(0x00);
    // LCD_WriteData(0x34);
    // LCD_WriteData(0x02);

    // LCD_WriteCommand(0xF7); // Pump ratio control
    // LCD_WriteData(0x20);

    // LCD_WriteCommand(0xEA); // Driver timing control B
    // LCD_WriteData(0x00);
    // LCD_WriteData(0x00);

    // LCD_WriteCommand(0xC0); // Power Control 1 (VRH[5:0])
    // LCD_WriteData(0x23);

    // LCD_WriteCommand(0xC1); // Power Control 2 (SAP[2:0], BT[2:0])
    // LCD_WriteData(0x10);

    // LCD_WriteCommand(0xC5); // VCOM Control 1
    // LCD_WriteData(0x3E);
    // LCD_WriteData(0x28);

    // LCD_WriteCommand(0xC7); // VCOM Control 2
    // LCD_WriteData(0x86);

    // 内存访问与像素格式设置 (Memory & Pixel Format) [手册 Section 8.2.29 / 8.2.33]
    LCD_WriteCommand(0x36); // Memory Access Control (MADCTL)
    LCD_WriteData(0x8C);    // 默认竖屏，RGB 顺序 (若色彩反了或镜像等问题，修改此字节)

    LCD_WriteCommand(0x3A); // COLMOD: Pixel Format Set
    LCD_WriteData(0x55);    // 0x55 表示 16-bit/pixel (RGB565)

    // 帧率与显示功能控制 (Frame Rate & Display Function) [手册 Section 8.3.2 / 8.3.7]
    LCD_WriteCommand(0xB1); // Frame Rate Control (In Normal Mode)
    LCD_WriteData(0x00);
    LCD_WriteData(0x1B); // 70Hz 刷新率

    LCD_WriteCommand(0xB6); // Display Function Control
    LCD_WriteData(0x0A);
    LCD_WriteData(0x82);
    LCD_WriteData(0x27);

    // // Enable 3G & Gamma 曲线设置 (Gamma Curve) [手册 Section 8.2.17 / 8.3.24]
    // LCD_WriteCommand(0xF2); // Enable 3G
    // LCD_WriteData(0x00);    // Disable

    // LCD_WriteCommand(0x26); // Gamma Set
    // LCD_WriteData(0x01);    // Gamma curve 1 (G2.2)

    // LCD_WriteCommand(0xE0); // Positive Gamma Correction
    // LCD_WriteData(0x0F); LCD_WriteData(0x31); LCD_WriteData(0x2B); LCD_WriteData(0x0C);
    // LCD_WriteData(0x0E); LCD_WriteData(0x08); LCD_WriteData(0x4E); LCD_WriteData(0xF1);
    // LCD_WriteData(0x37); LCD_WriteData(0x07); LCD_WriteData(0x10); LCD_WriteData(0x03);
    // LCD_WriteData(0x0E); LCD_WriteData(0x09); LCD_WriteData(0x00);

    // LCD_WriteCommand(0xE1); // Negative Gamma Correction
    // LCD_WriteData(0x00); LCD_WriteData(0x0E); LCD_WriteData(0x14); LCD_WriteData(0x03);
    // LCD_WriteData(0x11); LCD_WriteData(0x07); LCD_WriteData(0x31); LCD_WriteData(0x2C);
    // LCD_WriteData(0x48); LCD_WriteData(0x08); LCD_WriteData(0x0F); LCD_WriteData(0x0C);
    // LCD_WriteData(0x31); LCD_WriteData(0x36); LCD_WriteData(0x0F);

    // 退出睡眠并开启显示 [手册 Section 8.2.12 / 8.2.19]
    LCD_WriteCommand(0x11); // Sleep Out
    HAL_Delay(120);         // 退出睡眠必须等待 120ms

    LCD_WriteCommand(0x29); // Display ON
}

/**
 * @brief 设置 LCD 显示窗口
 * @param x1 起始列坐标 (0 ~ 239)
 * @param y1 起始行坐标 (0 ~ 319)
 * @param x2 结束列坐标 (0 ~ 239)
 * @param y2 结束行坐标 (0 ~ 319)
 */
void LCD_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // 设置列坐标 X
    LCD_WriteCommand(0x2A); // Column Address Set
    LCD_WriteData(x1 >> 8);
    LCD_WriteData(x1 & 0x00FF);
    LCD_WriteData(x2 >> 8);
    LCD_WriteData(x2 & 0x00FF);

    // 设置行坐标 Y
    LCD_WriteCommand(0x2B); // Page Address Set
    LCD_WriteData(y1 >> 8);
    LCD_WriteData(y1 & 0x00FF);
    LCD_WriteData(y2 >> 8);
    LCD_WriteData(y2 & 0x00FF);

    // 准备写 GRAM
    LCD_WriteCommand(0x2C);
}

/**
 * @brief  LCD 全屏填充单色
 * @param  color: 16-bit RGB565 颜色值
 */
void LCD_Fill(uint16_t color)
{
    // 填充单行 240 像素数据
    for (int i = 0; i < 240; i++)
    {
        line_buf[i * 2] = color >> 8;
        line_buf[i * 2 + 1] = color & 0xFF;
    }

    LCD_SetAddressWindow(0, 0, 239, 319); // 设置窗口大小 240 * 320 （分辨率）

    LCD_DC(1); // 切换写数据模式

    LCD_CS(0); // 批量传输时片选保持拉低
    // 逐行推送，共 320 行
    for (int y = 0; y < 320; y++)
    {
        HAL_SPI_Transmit(&hspi1, line_buf, sizeof(line_buf), HAL_MAX_DELAY);
    }
    LCD_CS(1);
}

/******************************
 *  LCD触摸驱动（XPT2046触摸芯片）
 ******************************/

/**
 * @brief  XPT2046 底层读写一个字节
 */
uint8_t XPT2046_SPI_ReadWriteByte(uint8_t tx_data)
{
    uint8_t rx_data = 0;

    HAL_SPI_TransmitReceive(&hspi2, &tx_data, &rx_data, 1, HAL_MAX_DELAY);

    return rx_data;
}

/**
 * @brief  读取 XPT2046 单个通道的 12 位 ADC 原始值
 */
uint16_t XPT2046_Read_ADC(uint8_t cmd)
{
    uint8_t num_H = 0, num_L = 0;
    uint16_t adcValue = 0;

    TOUCH_CS(0); // 拉低片选，开始通信

    // 1. 发送 8 位控制指令
    XPT2046_SPI_ReadWriteByte(cmd);

    // 2. 转换延迟（可选，硬件 SPI 传输速度较快时可微延时，通常不加或加 1~2us）

    // 3. 读取 16 位数据（有效数据为高 12 位）
    num_H = XPT2046_SPI_ReadWriteByte(0xFF);
    num_L = XPT2046_SPI_ReadWriteByte(0xFF);

    TOUCH_CS(1); // 拉高片选，结束通信

    // 数据拼接：高字节在前面，右移 3 位得到真正的 12-bit 数据 (0~4095)
    adcValue = ((uint16_t)num_H << 8) | num_L;
    adcValue >>= 3;

    return adcValue;
}

/**
 * @brief  多次采样求平均值（降噪滤波）
 * @param  cmd: 采样指令 (XPT2046_CMD_X 或 XPT2046_CMD_Y)
 * @param  samples: 采样次数 (建议 5 ~ 10 次)
 */
uint16_t XPT2046_Read_Average(uint8_t cmd, uint8_t samples)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < samples; i++)
    {
        sum += XPT2046_Read_ADC(cmd);
    }
    return (uint16_t)(sum / samples);
}

/**
 * @brief  检测当前是否有按下状态
 * @return 1: 有触摸按压, 0: 无按压
 */
uint8_t Touch_IsPressed(void)
{
    // 如果按键中断引脚 PB13 被拉低，说明有按下（触发 PENIRQ）
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief  读取映射后的最终屏幕像素坐标 (带防抖与校验)
 * @param  x: 传出的 X 像素坐标指针
 * @param  y: 传出的 Y 像素坐标指针
 * @return 1: 成功读取坐标, 0: 未触摸或读取失败
 */
uint8_t Touch_Read_XY(uint16_t *x, uint16_t *y)
{
    // 确认触摸状态
    if (!Touch_IsPressed())
    {
        return 0; // 未按压，直接返回
    }

    // 采样两次消除突变抖动
    uint16_t adcValue_X = XPT2046_Read_Average(XPT2046_CMD_X, 5);
    uint16_t adcValue_Y = XPT2046_Read_Average(XPT2046_CMD_Y, 5);

    // 再次确认触摸状态
    if (!Touch_IsPressed())
    {
        return 0;
    }

    // 映射为像素坐标
    // 注意：若发现触摸轴方向反了，只需把 Touch_Map_X 传给 *y，或修改 Touch_Map_X 内部公式即可
    *x = Touch_Map_X(adcValue_X);
    *y = Touch_Map_Y(adcValue_Y);

    return 1;
}

/**
 * @brief  将 X 轴 ADC 值映射为屏幕像素坐标
 * @param  adcValue_X: X 轴 ADC 原始值
 * @return 屏幕像素坐标
 */
uint16_t Touch_Map_X(uint16_t adcValue_X)
{
    
    // 防止越界
    if (adcValue_X < XPT2046_MIN_RAW_X)
        adcValue_X = XPT2046_MIN_RAW_X;

    if (adcValue_X > XPT2046_MAX_RAW_X)
        adcValue_X = XPT2046_MAX_RAW_X;

    // 实测左侧是 3804，右侧是 194
    // 把 ADC 值转换 X 轴坐标（X / 239 = adcValue_X - 194 / 3804 - 194）
    return (uint32_t)(XPT2046_MAX_RAW_X - adcValue_X) * 239 / (XPT2046_MAX_RAW_X - XPT2046_MIN_RAW_X);
}

/**
 * @brief  将 Y 轴 ADC 值映射为屏幕像素坐标
 * @param  adcValue_Y: Y 轴 ADC 原始值
 * @return 屏幕像素坐标
 */
uint16_t Touch_Map_Y(uint16_t adcValue_Y)
{
    // 防止越界
    if (adcValue_Y < XPT2046_MIN_RAW_Y)
        adcValue_Y = XPT2046_MIN_RAW_Y;

    if (adcValue_Y > XPT2046_MAX_RAW_Y)
        adcValue_Y = XPT2046_MAX_RAW_Y;

    // 实测上方是 311，下方侧是 3911
    // 把 ADC 值转换 Y 轴坐标
    return (uint32_t)(adcValue_Y - XPT2046_MIN_RAW_Y) * 319 / (XPT2046_MAX_RAW_Y - XPT2046_MIN_RAW_Y);
}
