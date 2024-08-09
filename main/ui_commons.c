#include <ui_commons.h>


lv_color_t lv_color_ok() {
    return lv_color_hex(0x00FF00);
}

lv_color_t lv_color_warn() {
    return lv_color_hex(0xffb84d);
}

lv_color_t lv_color_crit() {
    return lv_color_hex(0xff0000);
}
lv_color_t lv_desc_bg() {
    return lv_color_hex(0x363B3B);
}

lv_color_t lv_cont_bg() {
    return lv_color_hex(0x1F2222);
}

lv_color_t lv_main_bg() {
    return lv_color_hex(0x2A2F2F);
}

lv_color_t lv_border_color() {
    return lv_color_hex(0xCFCFCF);
}


void draw_as_normal(lv_obj_t* box, lv_obj_t* text) {
    lv_obj_set_style_bg_color(box, lv_cont_bg(), LV_PART_MAIN);
    lv_obj_set_style_text_color(text, lv_color_white(), LV_PART_MAIN);
}


void draw_as_warn(lv_obj_t* box, lv_obj_t* text) {
    lv_obj_set_style_bg_color(box, lv_color_warn(), LV_PART_MAIN);
    lv_obj_set_style_text_color(text, lv_color_black(), LV_PART_MAIN);
}


void draw_as_critical(lv_obj_t* box, lv_obj_t* text) {
    lv_obj_set_style_bg_color(box, lv_color_crit(), LV_PART_MAIN);
    lv_obj_set_style_text_color(text, lv_color_black(), LV_PART_MAIN);
}