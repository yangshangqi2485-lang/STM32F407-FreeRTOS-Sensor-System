#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"

// 上拉按键专用宏（LastState 为上拉的高电平 1）
#define BUTTON_PULLUP(gpiox, pin) {(gpiox), BTN_PULL_UP, (pin), GPIO_PIN_SET}

// 下拉按键专用宏（LastState 为下拉的低电平 0）
#define BUTTON_PULLDOWN(gpiox, pin) {(gpiox), BTN_PULL_DOWN, (pin), GPIO_PIN_RESET}

typedef enum {
    BTN_PULL_UP = 1,
    BTN_PULL_DOWN = 0,
} GPIO_PullState_t;

typedef enum {
    BTN_TRIGGERED = 1,
    BTN_NOT_TRIGGERED = 0,
} ButtonState_t;

typedef struct {
    GPIO_TypeDef* GPIOx;        // GPIO 端口
    GPIO_PullState_t PullState; // 上拉/下拉模式
    uint16_t GPIO_Pin;          // GPIO 引脚
    uint8_t LastState;          // 记录上一次电平
} Button_t;

ButtonState_t RisingEdge_Button(Button_t *btn);

ButtonState_t FallingEdge_Button(Button_t *btn);

#endif
