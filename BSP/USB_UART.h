#ifndef USB_UART_H
#define USB_UART_H

uint8_t USB_Send_Safe(uint8_t* Buf, uint16_t Len, uint32_t Timeout_ms);

void USB_Send_UART(const char *format, ...);

#endif