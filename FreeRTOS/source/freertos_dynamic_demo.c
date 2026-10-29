#include "freertos_dynamic_demo.h"
#include "FreeRTOSConfig.h"

#if configSUPPORT_STATIC_ALLOCATION == 0

/* FreeRTOS 相关头文件，必须的 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/* 用户头文件 */
#include "BSP/USB_UART.h"
#include "BSP/Button.h"
#include "BSP/VL53L0X_LaserRanging/VL53L0X_LaserRanging.h"
#include "BSP/NTC.h"

#include "lvgl/lvgl.h"
#include "lvgl/porting/lv_port_disp.h"
#include "lvgl/porting/lv_port_indev.h"
#include "lvgl/user/UI/UI.h"

/* ======================================== */
/* ============= 用户全局变量 ============= */
/* ======================================== */

BaseType_t state;
uint8_t error_status;
#define EVENT_BIT_0 (1 << 0) // 定义事件位 0（VL53L0X 开关）

/* ======================================== */
/* ============= 回调函数 ================= */
/* ======================================== */

/* 栈溢出钩子函数（FreeRTOS 内存安全最后一道防线） */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* 1. 屏蔽全局中断，锁定现场，防止崩溃扩大 */
    taskDISABLE_INTERRUPTS();

    /* 2. 打印导致爆栈的任务名称 */
    USB_Send_UART("\r\n====================================\r\n");
    USB_Send_UART("[FATAL ERROR] Task Stack Overflow!\r\n");
    USB_Send_UART("Overflow Task Name: ");
    USB_Send_UART(pcTaskName); // 直接输出爆栈任务的名字
    USB_Send_UART("\r\n====================================\r\n");
}

/* 内存申请失败钩子函数 */
void vApplicationMallocFailedHook(void)
{
    /* 1. 屏蔽中断，锁定现场 */
    taskDISABLE_INTERRUPTS();

    /* 2. 串口打印报警信息 */
    USB_Send_UART("\r\n====================================\r\n");
    USB_Send_UART("[FATAL ERROR] FreeRTOS Malloc Failed!\r\n");
    USB_Send_UART("Reason: Heap space is out of memory.\r\n");
    USB_Send_UART("====================================\r\n");
}

/* ======================================== */
/* =============== 实现流程 ================ */
/* ======================================== */

// ====== PS: 也可以利用结构体实现 ========

/* 声明函数 */
void Task_1(void *pvParameters); // LCD 任务函数（LVGL图形）
void Task_2(void *pvParameters); // VL53L0X 开关按钮 任务函数
void Task_3(void *pvParameters); // VL53L0X 激光测距 任务函数
void Task_4(void *pvParameters); // NTC温度 任务函数（ADC测量）

/* 设置内存栈大小 */
#define TASK_1_STACK 1024 // 设置内存栈大小，单位（Word）根据处理器架构位数变化 4096字节
#define Task_2_STACK 512  // 设置内存栈大小，单位（Word）根据处理器架构位数变化 2048字节
#define Task_3_STACK 512  // 设置内存栈大小，单位（Word）根据处理器架构位数变化 2048字节
#define Task_4_STACK 512  // 设置内存栈大小，单位（Word）根据处理器架构位数变化 2048字节

/* 设置优先等级 */
#define Task_1_PRIORITY 3 // 优先等级（数字越大优先级越大，相反优先级越小）
#define Task_2_PRIORITY 4 // 优先等级（数字越大优先级越大，相反优先级越小）
#define Task_3_PRIORITY 2 // 优先等级（数字越大优先级越大，相反优先级越小）
#define Task_4_PRIORITY 1 // 优先等级（数字越大优先级越大，相反优先级越小）

/* 创建句柄 */
TaskHandle_t Task_1_Handle; // 任务句柄
TaskHandle_t Task_2_Handle; // 任务句柄
TaskHandle_t Task_3_Handle; // 任务句柄
TaskHandle_t Task_4_Handle; // 任务句柄

QueueHandle_t Display_Queue_Handle; // 队列句柄

SemaphoreHandle_t I2C_Mutex_Handle; // 互斥锁句柄

EventGroupHandle_t EventGroupHandle; // 事件组句柄

/**
 * @brief 创建并启动FreeRTOS
 */
void FreeRTOS_Dynamic_Start(void)
{
    /* 创建 Task_1 任务 */
    state = xTaskCreate(
        (TaskFunction_t)Task_1,               // 任务函数的地址
        (const char *)"Task_1",               // 任务名字字符串
        (configSTACK_DEPTH_TYPE)TASK_1_STACK, // 任务栈大小，单位（Word）
        (void *)NULL,                         // 传递给任务参数
        (UBaseType_t)Task_1_PRIORITY,         // 任务优先级
        (TaskHandle_t *)&Task_1_Handle        // 任务句柄的地址
    );
    if (state != pdPASS)
    {
        USB_Send_UART("任务一创建失败！");
    }

    /* 创建 Task_2 任务 */
    state = xTaskCreate(
        (TaskFunction_t)Task_2,               // 任务函数的地址
        (const char *)"Task_2",               // 任务名字字符串
        (configSTACK_DEPTH_TYPE)Task_2_STACK, // 任务栈大小，单位（Word）
        (void *)NULL,                         // 传递给任务参数
        (UBaseType_t)Task_2_PRIORITY,         // 任务优先级
        (TaskHandle_t *)&Task_2_Handle        // 任务句柄的地址
    );
    if (state != pdPASS)
    {
        USB_Send_UART("任务二创建失败！");
    }

    /* 创建 Task_3 任务 */
    state = xTaskCreate(
        (TaskFunction_t)Task_3,               // 任务函数的地址
        (const char *)"Task_3",               // 任务名字字符串
        (configSTACK_DEPTH_TYPE)Task_3_STACK, // 任务栈大小，单位（Word）
        (void *)NULL,                         // 传递给任务参数
        (UBaseType_t)Task_3_PRIORITY,         // 任务优先级
        (TaskHandle_t *)&Task_3_Handle        // 任务句柄的地址
    );
    if (state != pdPASS)
    {
        USB_Send_UART("任务三创建失败！");
    }

    /* 创建 Task_4 任务 */
    state = xTaskCreate(
        (TaskFunction_t)Task_4,               // 任务函数的地址
        (const char *)"Task_4",               // 任务名字字符串
        (configSTACK_DEPTH_TYPE)Task_4_STACK, // 任务栈大小，单位（Word）
        (void *)NULL,                         // 传递给任务参数
        (UBaseType_t)Task_4_PRIORITY,         // 任务优先级
        (TaskHandle_t *)&Task_4_Handle        // 任务句柄的地址
    );
    if (state != pdPASS)
    {
        USB_Send_UART("任务四创建失败！");
    }

    /* 创建队列 */
    Display_Queue_Handle = xQueueCreate(5, sizeof(Display_t));
    if (Display_Queue_Handle == NULL)
    {
        USB_Send_UART("队列创建失败！\n");
    }

    /* 创建互斥锁 */
    I2C_Mutex_Handle = xSemaphoreCreateMutex();
    if (I2C_Mutex_Handle == NULL)
    {
        USB_Send_UART("互斥锁创建失败！\n");
    }
    
    /* 创建事件组 */
    EventGroupHandle = xEventGroupCreate();
    if (EventGroupHandle == NULL)
    {
        USB_Send_UART("事件组创建失败！\n");
    }

    /* 2.启动调度器：会自动创建IDLE空闲任务 */
    vTaskStartScheduler();
}

void Task_1(void *pvParameters)
{
    /* 1. 任务单次初始化（可选） */

    /* LVGL 系统初始化 */
    lv_init();            // 初始化 LVGL
    lv_port_disp_init();  // 初始化LCD显示屏移植层
    lv_port_indev_init(); // 初始化触摸屏移植层
    User_UI();
    /* 2. 主死循环 */
    for (;;)
    {
        get_display_data(); // 刷新显示数据
        
        // 处理 LVGL 任务并获取下次唤醒时间
        uint32_t sleep_time_ms = lv_timer_handler();

        // 如果没有任何定时器任务，设定一个上限防止永久阻塞
        if (sleep_time_ms == LV_NO_TIMER_READY)
        {
            sleep_time_ms = LV_DEF_REFR_PERIOD; // 默认通常是 33ms
        }

        // 确保延时至少为 1ms，防止完全不延时导致 CPU 占满 100%
        else if (sleep_time_ms == 0)
        {
            sleep_time_ms = 1;
        }

        // 防止延时过大
        else if (sleep_time_ms > 33)
        {
            sleep_time_ms = 33;
        }

        vTaskDelay(pdMS_TO_TICKS(sleep_time_ms));
    }
}

void Task_2(void *pvParameters)
{
    /* 1. 任务单次初始化（可选） */
    static Button_t btn_1 = BUTTON_PULLUP(GPIOD, GPIO_PIN_8);
    static Button_t btn_2 = BUTTON_PULLUP(GPIOD, GPIO_PIN_7);

    /* 2. 主死循环 */
    for (;;)
    {
        // 按钮一：开启激光测量 
        if (FallingEdge_Button(&btn_1) == BTN_TRIGGERED)
        {
            // 设置事件标志位
            xEventGroupSetBits(EventGroupHandle, EVENT_BIT_0); // 开启
        }

        // 按钮二：关闭激光测量
        if (FallingEdge_Button(&btn_2) == BTN_TRIGGERED)
        {
            // 清除事件标志位
            xEventGroupClearBits(EventGroupHandle, EVENT_BIT_0); // 关闭
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void Task_3(void *pvParameters)
{
    /* 1. 任务单次初始化（可选） */
    uint8_t error_status = VL53L0X_Init();
    VL53L0X_Errors(error_status);

    int distance_mm = 0;
    Display_t tx_msg = {0};
    /* 2. 主死循环 */
    for (;;)
    {
        // 等待通知，阻塞直到有通知到来
        EventBits_t event_value = xEventGroupGetBits(EventGroupHandle);
        if (event_value & EVENT_BIT_0)
        {
            // 进入互斥锁，防止多任务争夺同一个I2C线程（当前只有一个任务在用 I2C）
            if (xSemaphoreTake(I2C_Mutex_Handle, portMAX_DELAY) == pdPASS)
            {
                // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // 切换板载 LED 状态，作为按钮按下的视觉反馈
                
                error_status = VL53L0X_Start();
                VL53L0X_Errors(error_status); // 检测开启过程是否错误

                // 抓取最新的绝对毫米距离 (ToF 时间转换值)
                distance_mm = VL53L0X_GetDistance();

                xSemaphoreGive(I2C_Mutex_Handle); // 释放I2C使用权
            }
            
            /* 准备发送到LCD任务的消息 */
            tx_msg.dev = VL53L0X;
            tx_msg.value.val_int = distance_mm; // 显示采集数据

            // 将距离数据发送到LCD任务队列
            xQueueSend(Display_Queue_Handle, &tx_msg, 0);
            
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        else
        {
            // 进入互斥锁，防止多任务争夺同一个I2C线程（当前只有一个任务在用 I2C）
            if (xSemaphoreTake(I2C_Mutex_Handle, portMAX_DELAY) == pdPASS)
            {
                // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // 切换板载 LED 状态，作为按钮按下的视觉反馈
                
                error_status = VL53L0X_Stop();
                VL53L0X_Errors(error_status); // 检测关闭过程是否错误
                
                xSemaphoreGive(I2C_Mutex_Handle); // 释放I2C使用权
            }
            
            /* 准备发送到LCD任务的消息 */
            tx_msg.dev = VL53L0X;
            tx_msg.value.val_int = 0;  // 显示关闭数据状态

            // 将距离数据发送到LCD任务队列
            xQueueSend(Display_Queue_Handle, &tx_msg, 0);
            
            // 进入阻塞状态，等待下一次置位任意事件标志位再唤醒
            xEventGroupWaitBits(EventGroupHandle, EVENT_BIT_0, pdFALSE, pdFALSE, portMAX_DELAY);
        }
    }
}

void Task_4(void *pvParameters)
{
    /* 1. 任务单次初始化（可选） */
    float temperature = 0.0;
    Display_t tx_msg = {0};
    NTC_DMA_Init();
    /* 2. 主死循环 */
    for (;;)
    {
        // 获取ADC并算出温度
        temperature = NTC_Temperature(NTC_DMA_Value(0));

        /* 准备发送到LCD任务的消息 */
        tx_msg.dev = NTC;
        tx_msg.value.val_float = temperature;

        // 将温度数据发送到LCD任务队列
        xQueueSend(Display_Queue_Handle, &tx_msg, 0);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#endif
