#include "freertos_static_demo.h"
#include "FreeRTOSConfig.h"

#if configSUPPORT_STATIC_ALLOCATION

/* FreeRTOS 相关驱动头文件，必须的 */
#include "FreeRTOS.h"
#include "task.h"

/* ======================================== */
/* ============= 静态任务创建 ============= */
/* ======================================== */

/* 启动任务配置 */
void Task_Start(void *pvParameters);            // 任务函数
#define Task_Start_Stack 128                    // 设置内存栈大小，单位（Word）根据处理器架构位数变化
#define Task_Start_Priority 5                   // 优先等级（数字越大优先级越大，相反优先级越小）
StackType_t task_start_stack[Task_Start_Stack]; // 静态任务栈（数组地址）
StaticTask_t task_start_tcb;                    // 静态任务TCB结构体类型
TaskHandle_t Task_Start_Handle;                 // 句柄

/* LED_1任务配置 */
void Task_1(void *pvParameters);            // 任务函数
#define TASK_1_STACK 512                    // 设置内存栈大小，单位（Word）根据处理器架构位数变化
#define Task_1_PRIORITY 1                   // 优先等级（数字越大优先级越大，相反优先级越小）
StackType_t task_1_stack[TASK_1_STACK]; // 静态任务栈（数组地址）
StaticTask_t task_1_tcb;                    // 静态任务TCB结构体类型
TaskHandle_t Task_1_Handle;                 // 句柄

/* LED_2任务配置 */
void Task_2(void *pvParameters);            // 任务函数
#define Task_2_STACK 512                    // 设置内存栈大小，单位（Word）根据处理器架构位数变化
#define Task_2_PRIORITY 1                   // 优先等级（数字越大优先级越大，相反优先级越小）
StackType_t task_2_stack[Task_2_STACK]; // 静态任务栈（数组地址）
StaticTask_t task_2_tcb;                    // 静态任务TCB结构体类型
TaskHandle_t Task_2_Handle;                 // 句柄


/* ===================================================== */
/* ============= 静态分配必须实现的回调函数 ============= */
/* ===================================================== */

StaticTask_t xIdleTaskTCB;
StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

/**
 * @brief 提供空闲任务（Idle Task）的内存资源
 */
void vApplicationGetIdleTaskMemory(
    StaticTask_t **ppxIdleTaskTCBBuffer,
    StackType_t **ppxIdleTaskStackBuffer,
    uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = xIdleStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

StaticTask_t xTimerTaskTCB;
StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

/**
 * @brief 提供软件定时器任务（Timer Task）的内存资源
 * @note 如果 FreeRTOSConfig.h 中 configUSE_TIMERS = 1，则必须实现此函数
 */
void vApplicationGetTimerTaskMemory(
    StaticTask_t **ppxTimerTaskTCBBuffer,
    StackType_t **ppxTimerTaskStackBuffer,
    uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = xTimerStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


/* ======================================== */
/* ============= 任务流程实现 ============= */
/* ======================================== */

/**
 * @brief 启动FreeRTOS调度器
 */
void FreeRTOS_Static_Start(void)
{
    /* 1.创建专门启动FreeRTOS调度器的任务 */
    xTaskCreateStatic(
        (TaskFunction_t)Task_Start,
        (const char *)"Task_Start",
        (const uint32_t)Task_Start_Stack,
        (void *)NULL,
        (UBaseType_t)Task_Start_Priority,
        (StackType_t *)task_start_stack,
        (StaticTask_t *)&task_start_tcb);

    /* 2.启动调度器：会自动创建空闲任务 */
    vTaskStartScheduler();
}

/**
 * @brief 启动任务：启动创建好的任务
 */
void Task_Start(void *pvParameters)
{
    // 进入临界区（保护代码），防止出现抢占式上下文切换导致未能创建所有任务需要
    taskENTER_CRITICAL();

    /* 创建 LED_1 任务 */
    Task_1_Handle = xTaskCreateStatic(
        (TaskFunction_t)Task_1,               // 任务函数的地址
        (const char *)"LED_1",                    // 任务名字字符串
        (configSTACK_DEPTH_TYPE)TASK_1_STACK, // 任务栈大小，单位（Word）
        (void *)NULL,                             // 传递给任务参数
        (UBaseType_t)Task_1_PRIORITY,         // 任务优先级
        (StackType_t *)task_1_stack,          // 静态任务栈（数组地址）
        (StaticTask_t *)&task_1_tcb           // 静态任务TCB结构体类型
    );

    /* 创建 LED_2 任务 */
    Task_2_Handle = xTaskCreateStatic(
        (TaskFunction_t)Task_2,               // 任务函数的地址
        (const char *)"LED_2",                    // 任务名字字符串
        (configSTACK_DEPTH_TYPE)Task_2_STACK, // 任务栈大小，单位（Word）
        (void *)NULL,                             // 传递给任务参数
        (UBaseType_t)Task_2_PRIORITY,         // 任务优先级
        (StackType_t *)task_2_stack,          // 静态任务栈（数组地址）
        (StaticTask_t *)&task_2_tcb           // 静态任务TCB结构体类型
    );
    
    // 退出临界区
    taskEXIT_CRITICAL();
    
    // 创建任务只需要一次，用完立马销毁此任务
    vTaskDelete(NULL);
}

/* 测试FreeRTOS任务调度状态 */
#include "BSP/USB_UART.h"

void Task_1(void *pvParameters)
{
    /* 1. 任务单次初始化（可选） */

    /* 2. 主死循环 */
    for (;;)
    {
        USB_Send_UART("LED_1 启动");

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Task_2(void *pvParameters)
{
    /* 1. 任务单次初始化（可选） */

    /* 2. 主死循环 */
    for (;;)
    {
        USB_Send_UART("LED_2 启动");

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

#endif
