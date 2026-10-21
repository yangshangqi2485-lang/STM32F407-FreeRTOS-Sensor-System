#include "main.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "USB_UART.h"

/**
 * @brief  安全的 USB 串口数据发送函数
 * @param  Buf: 数据缓冲区指针
 * @param  Len: 发送数据长度
 * @param  Timeout_ms: 等待超时时间 (ms)
 * @return USBD_OK / USBD_BUSY / USBD_FAIL
 */
uint8_t USB_Send_Safe(uint8_t *Buf, uint16_t Len, uint32_t Timeout_ms)
{
  uint32_t tickstart = HAL_GetTick();

  // 当 USB 状态为 BUSY 时循环等待，直到超时或空闲
  while (CDC_Transmit_FS(Buf, Len) == USBD_BUSY)
  {
    if ((HAL_GetTick() - tickstart) > Timeout_ms)
    {
      return USBD_FAIL; // 发送超时
    }
  }
  return USBD_OK;
}

/**
 * @brief 自定义 USB 格式化打印函数
 * @note Cmake 开启 printf 的浮点数支持：target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -u _printf_float)
 */
void USB_Send_UART(const char *format, ...)
{
  char send_buf[1024]; // 根据你单次打印的最大长度调整大小

  // 1. va_list: 声明一个“参数游标/指针”
  va_list args;

  // 2. va_start: 初始化游标。告诉它：“从 format 参数后面的第一个参数开始抓取”
  va_start(args, format);

  // 3. vsnprintf: 拿着游标，把所有传入的 %d、%f 替换并打印到 send_buf 数组里
  vsnprintf(send_buf, sizeof(send_buf), format, args);

  // 4. va_end: 善后清理，收回游标（防止指针悬空）
  va_end(args);

  // 最后把打包好的文本发给 USB
  USB_Send_Safe((uint8_t *)send_buf, strlen(send_buf), 50);
}
