#ifndef FREERTOS_DYNAMIC_DEMO_H
#define FREERTOS_DYNAMIC_DEMO_H

// 设备类型枚举
typedef enum {
    NTC,
    VL53L0X,
    NO_DEVICE
} Device_t;

// 设备读取值类型
typedef union {
    int val_int;     // 当类型是距离时，用这个存整数
    float val_float; // 当类型是温度时，用这个存浮点数
} Value_t;             // 联合体，用于存储不同类型的值

// 屏幕显示消息
typedef struct
{
    Device_t dev; // 设备类型
    Value_t value;
} Display_t;

void FreeRTOS_Dynamic_Start(void);

#endif