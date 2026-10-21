#ifndef SOFTWARE_I2C_H
#define SOFTWARE_I2C_H

#include "main.h"

void Software_I2C_Init(void);

void Software_I2C_Start(void);

void Software_I2C_Stop(void);

void Software_I2C_Ack(void);

void Software_I2C_NAck(void);

uint8_t Software_I2C_WaitAck(void);

void Software_I2C_SendByte(uint8_t Byte);

uint8_t Software_I2C_ReadByte(uint8_t ack);

uint8_t Software_I2C_SendData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint8_t len);

uint8_t Software_I2C_ReadData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint8_t len);

#endif