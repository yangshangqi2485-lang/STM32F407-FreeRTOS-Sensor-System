#include "BUTTON.h"

/**
 * @brief 上升沿检测按键状态
 * @param btn 按键结构体指针
 * @retval BTN_NOT_PRESSED 表示未按下，BTN_PRESSED 表示按下
 */
ButtonState_t RisingEdge_Button(Button_t *btn)
{
  ButtonState_t key_pressed = BTN_NOT_TRIGGERED;  // 按键事件标志
  uint8_t gpio_pin_state = HAL_GPIO_ReadPin(btn->GPIOx, btn->GPIO_Pin);
  uint8_t pressed_state = GPIO_PIN_RESET;         // 默认上拉电阻判定触发状态
  
  // 如果是下拉电阻，改变判定触发状态
  if (btn->PullState == BTN_PULL_DOWN)
  {
    pressed_state = GPIO_PIN_SET;
  }
  
  // 按下一瞬间触发事件，如果按住未松手则不触发事件
  if (btn->LastState == !pressed_state && gpio_pin_state == pressed_state)
  {
    key_pressed = BTN_TRIGGERED; // 按键按下事件
  }
  
  btn->LastState = gpio_pin_state; // 更新上一次按键状态
  
  return key_pressed;
}

/**
 * @brief 下降沿检测按键状态
 * @param btn 按键结构体指针
 * @retval BTN_NOT_PRESSED 表示未按下，BTN_PRESSED 表示按下
 */
ButtonState_t FallingEdge_Button(Button_t *btn)
{
  ButtonState_t key_pressed = BTN_NOT_TRIGGERED;  // 按键事件标志
  uint8_t gpio_pin_state = HAL_GPIO_ReadPin(btn->GPIOx, btn->GPIO_Pin);
  uint8_t pressed_state = GPIO_PIN_RESET;         // 默认上拉电阻判定触发状态
  
  // 如果是下拉电阻，改变判定触发状态
  if (btn->PullState == BTN_PULL_DOWN)
  {
    pressed_state = GPIO_PIN_SET;
  }
  
  // 上一次是按下状态，且当前变成了未按下状态（松开瞬间）
  if (btn->LastState == pressed_state && gpio_pin_state == !pressed_state)
  {
    key_pressed = BTN_TRIGGERED; // 触发释放事件
  }
  
  btn->LastState = gpio_pin_state; // 更新上一次状态
  
  return key_pressed;
}
