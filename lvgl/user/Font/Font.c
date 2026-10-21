#include "Font.h"

static void anime_1(lv_anim_t *a);
static void anime_2(lv_anim_t *a);

static void start_anime(lv_obj_t *label, int32_t text_width)
{
    int32_t text_center = text_width / 2;

    // 创建自定义滚动动画a
    lv_anim_t a_in;
    lv_anim_init(&a_in);
    lv_anim_set_var(&a_in, label);

    lv_anim_set_values(&a_in, 240, text_center); // 从右侧进场到正中间
    lv_anim_set_exec_cb(&a_in, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_time(&a_in, 3000); // 进场时长 3秒

    lv_anim_set_user_data(&a_in, (void*)(intptr_t)text_width);
    lv_anim_set_completed_cb(&a_in, anime_1);

    lv_anim_start(&a_in);
}

static void anime_1(lv_anim_t *a)
{
    lv_obj_t * label = (lv_obj_t *)a->var;
    int32_t text_width = (int32_t)(intptr_t)a->user_data;
    int32_t text_center = (240 - text_width) / 2; // 正确计算居中 X 坐标

    lv_anim_t a_out;
    lv_anim_init(&a_out);
    lv_anim_set_var(&a_out, label);

    lv_anim_set_values(&a_out, text_center, -text_width); // 从中间滚到屏幕左侧外
    lv_anim_set_exec_cb(&a_out, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_time(&a_out, 3000); // 滚动时长 3秒

    // 核心点：设置 2000ms 启动延迟，实现在中间静止停留 3 秒
    lv_anim_set_delay(&a_out, 3000);

    lv_anim_set_user_data(&a_out, (void*)(intptr_t)text_width);
    lv_anim_set_completed_cb(&a_out, anime_2);

    lv_anim_start(&a_out);
}

static void anime_2(lv_anim_t *a)
{
    lv_obj_t * label = (lv_obj_t *)a->var;
    int32_t text_width = (int32_t)(intptr_t)a->user_data;
    start_anime(label, text_width);
}

void create_forced_scroll_welcome(void)
{
    // 1. 创建 Label
    lv_obj_t *label = lv_label_create(lv_scr_act());

    // 2. 设置字体与文本
    LV_FONT_DECLARE(my_font_chinese_32); // 声明自己生成的自定义字体
    lv_obj_set_style_text_font(label, &my_font_chinese_32, 0);   // 加载字体
    lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0); // 设置字体颜色
    lv_label_set_text(label, "热烈欢迎");

    // 3. 强制刷新布局，精准获取文字渲染后的像素宽高
    lv_obj_update_layout(label);
    int32_t text_width = lv_obj_get_width(label);
    int32_t text_height = lv_obj_get_height(label);
    // int32_t text_center = lv_obj_get_width(label) / 2;

    // 4. 设置 Y 轴在 320 高度屏幕正中间
    lv_obj_set_y(label, (320 - text_height) / 2);

    start_anime(label, text_width);

    // // 5. 创建自定义滚动动画
    // lv_anim_t a;
    // lv_anim_init(&a);
    // lv_anim_set_var(&a, label);

    // lv_anim_t b;
    // lv_anim_init(&b);
    // lv_anim_set_var(&b, label);



    // // 起点：240（屏幕最右侧外） -> 中间：text_center（滚到屏幕中间）
    // lv_anim_set_values(&a, 240, text_center);

    // lv_anim_set_duration(&a, 2000);

    // // 中间：240（屏幕最右侧外） -> 终点点：text_center（滚到屏幕中间）
    // lv_anim_set_values(&a, 240, -text_width);

    // // 执行回调：更新 X 坐标
    // lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);



    // // 滚动周期时长（4000ms = 4秒，数值越大越慢）
    // lv_anim_set_time(&a, 4000);

    // // 设置无限循环
    // lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    // // 启动动画
    // lv_anim_start(&a);
}
