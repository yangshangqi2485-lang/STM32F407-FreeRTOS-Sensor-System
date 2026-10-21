#include "main.h"

#include <stdio.h>

/* 用户头文件 */
#include "BSP/USB_UART.h"

/* LVGL 头文件 */
#include "lvgl/user/UI/UI.h"
#include "lvgl/lvgl.h"

/* FreeRTOS 相关头文件 */
#include "FreeRTOS.h"
#include "freertos_dynamic_demo.h"
#include "queue.h"
#include "task.h"

/* ======================================== */
/* =============== 全局变量 =============== */
/* ======================================== */

/* 有关 FreeRTOS */
extern QueueHandle_t Display_Queue_Handle; // 队列句柄

lv_obj_t *label_ntc_temperature;

lv_obj_t *label_vl53l0x_distance;

/* ============================================= */
/* ============= 触发事件后业务 ================= */
/* ============================================= */

// static void button_1_event(lv_event_t *e)
// {
//     // lv_obj_t *button_1 = lv_event_get_target(e);
//     // vTaskSuspend(Task_2_Handle);
//     portDISABLE_INTERRUPTS();
// }


/* ============================================= */
/* ============= 主要逻辑业务 ================= */
/* ============================================= */

// 读取队列，返回当前接收到的设备类型，并通过指针带回对应的数值
void get_display_data(void)
{
    Display_t rx_msg;
    // 1. 非阻塞（0）尝试接收队列中的完整结构体
    while (xQueueReceive(Display_Queue_Handle, &rx_msg, 0) == pdPASS)
    {
        if (rx_msg.dev == NTC)
        {
            lv_label_set_text_fmt(label_ntc_temperature, "Temp: %.1f°C", rx_msg.value.val_float);
        }
        else if (rx_msg.dev == VL53L0X)
        {
            lv_label_set_text_fmt(label_vl53l0x_distance, "Distance: %dmm", rx_msg.value.val_int);
        }
        else
        {
            USB_Send_UART("收到未知设备");
        }
    }
}

void User_UI(void)
{
    // 获取当前活动的屏幕对象
    lv_obj_t *screen = lv_screen_active();

    /* 创建标签用于显示标题 */
    lv_obj_t *label_theme = lv_label_create(screen);
    lv_label_set_text_fmt(label_theme, "Multi-Sensor Acquisition and Display System");
    lv_obj_align(label_theme, LV_ALIGN_CENTER, 0, -110);    // 将标题标签放置在屏幕的上方
    lv_label_set_long_mode(label_theme, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);    // 设置标签的长文本模式为循环滚动
    lv_obj_set_size(label_theme, 200, 30); // 设置标签的宽度和高度

    /* 创建对象用于显示 NTC 温度 */
    lv_obj_t *obj_ntc = lv_obj_create(screen);
    lv_obj_set_size(obj_ntc, 200, 50); // 设置对象的宽度和高度
    lv_obj_align(obj_ntc, LV_ALIGN_CENTER, 0, -50); // 将对象放置在屏幕的中间偏上位置

    /* 创建对象用于显示 VL53L0X 测距数据 */
    lv_obj_t *obj_vl53l0x = lv_obj_create(screen);
    lv_obj_set_size(obj_vl53l0x, 200, 50); // 设置对象的宽度和高度
    lv_obj_align(obj_vl53l0x, LV_ALIGN_CENTER, 0, 50); // 将对象放置在屏幕的中间偏下位置

    /* 创建标签显示 NTC 温度数据 */
    label_ntc_temperature = lv_label_create(obj_ntc);
    lv_label_set_text_fmt(label_ntc_temperature, "Temp: None°C");
    lv_obj_align(label_ntc_temperature, LV_ALIGN_CENTER, 0, 0); // 将标签放置在对象的中心位置
    lv_label_set_long_mode(label_ntc_temperature, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR); // 设置标签的长文本模式为循环滚动

    /* 创建标签显示 VL53L0X 测距数据 */
    label_vl53l0x_distance = lv_label_create(obj_vl53l0x);
    lv_label_set_text_fmt(label_vl53l0x_distance, "Distance: None mm");
    lv_obj_align(label_vl53l0x_distance, LV_ALIGN_CENTER, 0, 0); // 将标签放置在对象的中心位置
    lv_label_set_long_mode(label_vl53l0x_distance, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR); // 设置标签的长文本模式为循环滚动
    
    // lv_obj_add_event_cb(button_1, button_1_event, LV_EVENT_CLICKED, NULL);
}
