#include "Software_I2C.h"
#include "tim.h"

/*引脚配置*/
#define SCL(x) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (x)) // PA2 SCL(0):低电平 SCL(0):高电平
#define SDA(x) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, (x)) // PA3 SDA(0):低电平 SDA(0):高电平
#define READ_SDA HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)     // 读取 PA2 数据电平

/**
 * @brief  微秒级阻塞延时（基于 TIM6，1MHz 计数）
 * @param  us: 延时的微秒数（无上限限制，自动处理 16 位定时器溢出）
 */
void Delay_us(uint32_t us)
{
    // 如果延时非常大（超过 TIM6 16位最大计数 65535），先进行分段
    while (us > 60000)
    {
        // 先处理 60000us (60ms) 的延时
        uint16_t start = TIM6->CNT;
        while ((uint16_t)(TIM6->CNT - start) < 60000);
        us -= 60000;
    }

    // 处理剩余小于 60000us 的微秒延时
    if (us > 0)
    {
        uint16_t start = TIM6->CNT;
        while ((uint16_t)(TIM6->CNT - start) < (uint16_t)us);
    }
}

/**
 * @brief  引脚初始化
 * @param  无
 * @retval 无
 */
void Software_I2C_Init(void)
{
    /*注意要在MX配置GPIO为开漏输出，设置上拉*/
    SCL(1);
    SDA(1);
}

/**
 * @brief  I2C开始
 * @param  无
 * @retval 无
 */
void Software_I2C_Start(void)
{
    SDA(1);
    SCL(1);
    Delay_us(1);
    SDA(0);
    Delay_us(1);
    SCL(0);
}

/**
 * @brief  I2C停止
 * @param  无
 * @retval 无
 */
void Software_I2C_Stop(void)
{
    SDA(0);
    SCL(1);
    Delay_us(1);
    SDA(1);
    Delay_us(1);
}

/**
 * @brief  主机产生应答 (ACK)
 * @param  无
 */
void Software_I2C_Ack(void)
{
    SCL(0);
    SDA(0);
    Delay_us(1);
    SCL(1);
    Delay_us(1);
    SCL(0);
}

/**
 * @brief  主机产生非应答 (NACK)
 * @param  无
 */
void Software_I2C_NAck(void)
{
    SCL(0);
    SDA(1);
    Delay_us(1);
    SCL(1);
    Delay_us(1);
    SCL(0);
}

/**
 * @brief  等待从机应答 (ACK)
 * @param  无
 * @retval 返回 0 代表有应答，返回 1 代表无应答 (NACK)
 */
uint8_t Software_I2C_WaitAck(void)
{
    uint8_t timeOut = 0;
    SDA(1); // 释放数据线准备接收
    Delay_us(1);
    SCL(1);
    Delay_us(1);

    while (READ_SDA)
    {
        timeOut++;
        if (timeOut > 250)
        {
            Software_I2C_Stop();
            return 1; // 超时无应答
        }
    }
    SCL(0);
    return 0;
}

/**
 * @brief  I2C发送一个字节
 * @param  Byte 要发送的一个字节
 * @retval 无
 */
void Software_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        SDA(Byte & (0x80 >> i)); // 进行转换两次逻辑非，强制 0 或 1，消除非零数值的多样性，强制进行二值化
        SCL(1);
        Delay_us(1);
        SCL(0);
    }
}

/**
 * @brief  I2C读取一个字节
 * @param  ack 读取完后应答 0:应答(Ack)  1:非应答(NAck)
 * @retval 返回读取到一个字节
 */
uint8_t Software_I2C_ReadByte(uint8_t ack)
{
    uint8_t dataByte = 0x00;

    // 1. 释放数据线，准备接收
    SDA(1);
    Delay_us(1);

    // 2. 循环读取 8 位数据
    for (uint8_t i = 0; i < 8; i++)
    {
        SCL(0); // 拉低时钟，允许从机改变数据
        Delay_us(1);
        SCL(1); // 拉高时钟，锁定数据
        Delay_us(1);

        dataByte <<= 0x01; // 左移一位，腾出最低位
        if (READ_SDA)
        {
            dataByte |= 0x01; // 如果接收到高电平，最低位置1
        }
    }

    // 3. 接收完毕后，由主机向从机发送应答信号
    // 注意：这里的 ack 参数通常由外部决定（如：0表示继续读，11表示读最后一个字节）
    if (ack == 0)
        Software_I2C_Ack(); // 主机发送应答
    else
        Software_I2C_NAck(); // 主机发送非应答

    return dataByte;
}

/**
 * @brief  I2C发送数据
 * @param  slaveAddress: 从机地址
 * @param  regAddress: 寄存器地址
 * @param  data: 数据缓冲区
 * @param  len: 数据长度
 * @retval 0:成功 1:失败
 */
uint8_t Software_I2C_SendData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint8_t len)
{
    Software_I2C_Start();

    Software_I2C_SendByte(slaveAddress);
    if (Software_I2C_WaitAck())
        return 1;

    Software_I2C_SendByte(regAddress);
    if (Software_I2C_WaitAck())
        return 1;

    for (uint8_t i = 0; i < len; i++)
    {
        Software_I2C_SendByte(data[i]);
        if (Software_I2C_WaitAck())
            return 1;
    }

    Software_I2C_Stop();
    return 0;
}

/**
 * @brief  I2C读取数据
 * @param  slaveAddress: 从机地址
 * @param  regAddress: 寄存器地址
 * @param  data: 数据缓冲区
 * @param  len: 数据长度
 * @retval 0:成功 1:失败
 */
uint8_t Software_I2C_ReadData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint8_t len)
{
    Software_I2C_Start();

    Software_I2C_SendByte(slaveAddress);
    if (Software_I2C_WaitAck())
        return 1;

    Software_I2C_SendByte(regAddress);
    if (Software_I2C_WaitAck())
        return 1;

    Software_I2C_Start();

    Software_I2C_SendByte(slaveAddress + 1);
    if (Software_I2C_WaitAck())
        return 1;

    for (uint8_t i = 0; i < len; i++)
    {
        if (i == len - 1)
            data[i] = Software_I2C_ReadByte(1);
        else
            data[i] = Software_I2C_ReadByte(0);
    }

    Software_I2C_Stop();
    return 0;
}
