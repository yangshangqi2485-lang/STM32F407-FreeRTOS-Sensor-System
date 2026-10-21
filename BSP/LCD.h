#ifndef LCD_H
#define LCD_H

#include "main.h"

/*********************************
 *  控制LCD屏幕显示（ILI9341显示芯片）
 *********************************/

#define LCD_CS(x)           HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (x))   // LCD片选信号，低电平选中通信
#define LCD_RESET(x)        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, (x))   // LCD复位信号，低电平有效复位
#define LCD_DC(x)           HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, (x))   // 高电平时，选择数据。低电平时选择命令
#define LCD_BACKLIGHT(x)    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, (x))   // 控制LCD背光，高电平开启背光，低电平关闭背光

/* 基础标准色(16bit) */

#define COLOR_BLACK       0x0000  // 黑色
#define COLOR_WHITE       0xFFFF  // 白色
#define COLOR_RED         0xF800  // 纯红
#define COLOR_GREEN       0x07E0  // 纯绿
#define COLOR_BLUE        0x001F  // 纯蓝
#define COLOR_YELLOW      0xFFE0  // 纯黄
#define COLOR_CYAN        0x07FF  // 青色
#define COLOR_MAGENTA     0xF81F  // 品红


/*********************************
 *  控制LCD屏幕触摸（XPT2046触摸芯片）
 *********************************/

// 定义引脚
#define TOUCH_CS(x)         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, (x))  // 触摸片选信号，低电平选中通信
#define TOUCH_EXTI(x)       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, (x))  // 触摸中断信号

// XPT2046 命令控制字宏定义
// S=1, A2-A0通道选择, MODE=0(12位), SER/DFR=0(差分模式), PD1-PD0=00(开启中断)
#define XPT2046_CMD_X   0xD0  // 采样 X 坐标 (通道 YP，差分)，测量 X 位置
#define XPT2046_CMD_Y   0x90  // 采样 Y 坐标 (通道 XP，差分)，测料 Y 位置

// 根据实际测量到的极限 ADC 值调整这 4 个边界常量
#define XPT2046_MIN_RAW_X   194
#define XPT2046_MAX_RAW_X   3804
#define XPT2046_MIN_RAW_Y   311
#define XPT2046_MAX_RAW_Y   3911


/*********************************
 *           声明函数
 *********************************/

void LCD_SPI_SendByte(uint8_t data);

void LCD_WriteCommand(uint8_t cmd);

void LCD_WriteData(uint8_t data);

void LCD_Reset(void);

void LCD_Init(void);

void LCD_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void LCD_Fill(uint16_t color);

uint8_t XPT2046_SPI_ReadWriteByte(uint8_t tx_data);

uint16_t XPT2046_Read_ADC(uint8_t cmd);

uint16_t XPT2046_Read_Average(uint8_t cmd, uint8_t samples);

uint8_t Touch_IsPressed(void);

uint8_t Touch_Read_XY(uint16_t *x, uint16_t *y);

uint16_t Touch_Map_X(uint16_t adcValue_X);

uint16_t Touch_Map_Y(uint16_t adcValue_Y);

#endif

