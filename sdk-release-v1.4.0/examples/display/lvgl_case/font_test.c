#include "lvgl.h"

void lv_label_font_test_case(void)
{
	lv_obj_t* src = lv_scr_act();
	static lv_style_t font_style;
    lv_obj_t* audio_label = lv_label_create(src);
    lv_obj_set_size(audio_label, 100, 40);
	lv_style_init(&font_style);

    // 使用系统字体，显示音乐符号.
    LV_FONT_DECLARE(lv_font_montserrat_12);
    lv_style_set_text_font(&font_style, &lv_font_montserrat_12);
    lv_obj_add_style(audio_label, LV_LABEL_LONG_WRAP, &font_style);
	lv_label_set_text_static(audio_label, LV_SYMBOL_AUDIO " audio");
    lv_obj_align(audio_label, LV_ALIGN_CENTER, 0, -360);

	static lv_style_t font_style1;
	LV_FONT_DECLARE(lv_font_montserrat_24);
    lv_style_init(&font_style1);
    lv_style_set_text_font(&font_style1, &lv_font_montserrat_24);
	lv_obj_t* video_label = lv_label_create(src);
    lv_obj_set_size(video_label, 100, 20);
    lv_obj_add_style(video_label, LV_LABEL_LONG_WRAP, &font_style1);
	lv_label_set_text_static(video_label, LV_SYMBOL_VIDEO " video");
    lv_obj_align(video_label, LV_ALIGN_CENTER, 0, -300);

    LV_FONT_DECLARE(lv_font_unscii_16);
    lv_obj_t * scroll_label1 = lv_label_create(src);
    // 设置字体从右向左滚动.
    lv_label_set_long_mode(scroll_label1, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(scroll_label1, 150, 60);
    lv_label_set_text(scroll_label1, "It is a circularly scrolling text. ");
    lv_obj_align(scroll_label1, LV_ALIGN_CENTER, 0, -200);

    lv_obj_t *scroll_label2 = lv_label_create(src);
    lv_label_set_long_mode(scroll_label2, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(scroll_label2, 150, 60);
    lv_obj_align(scroll_label2, LV_ALIGN_CENTER, 0, -100);

    lv_style_t font_style2;
	lv_style_init(&font_style2);
	lv_style_set_text_font(&font_style2, &lv_font_unscii_16);
    lv_obj_add_style(scroll_label2, LV_LABEL_LONG_WRAP, &font_style2);
    lv_label_set_text(scroll_label2, "Hello world 12345678ABCDEF ...");

    // 自定义中文黑体字体测试 
    LV_FONT_DECLARE(lv_font_simhei_18);
    lv_obj_t* lab_song = lv_label_create(lv_scr_act());
    lv_label_set_text(lab_song, "中文字体黑体测试12345abcEFG");
    lv_label_set_long_mode(lab_song, LV_LABEL_LONG_WRAP);
    lv_obj_align(lab_song, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(lab_song, 300, 28);
    lv_obj_set_scrollbar_mode(lab_song, LV_SCROLLBAR_MODE_OFF);
    // 应用字体
    lv_obj_set_style_text_font(lab_song, &lv_font_simhei_18, LV_PART_MAIN | LV_STATE_DEFAULT);
}


