#include "lvgl.h"
#include "esp_log.h"

char* TAG = "main_draw";

#define HEIGHT 480
#define WIDTH 800
#define COL_WIDTH 180
#define BORDER_SIZE 3

lv_obj_t* stint_timer_obj;
lv_obj_t* lap_timer_obj;
lv_obj_t* radio_obj;
lv_obj_t* oil_obj;
lv_obj_t* h2o_obj;
lv_obj_t* gas_obj;

void apply_styling(lv_obj_t* obj, lv_border_side_t additional_side)  {
    lv_obj_set_style_bg_color (obj, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xCFCFCF), LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN);
    lv_obj_set_style_border_side(obj, additional_side | LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);

}

void create_boxes(lv_obj_t* screen) {
    
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, &lv_font_montserrat_40);
    lv_style_set_shadow_color(&style, lv_color_hex(0xEF7215));


    stint_timer_obj = lv_obj_create(screen);
    lv_obj_set_size(stint_timer_obj, COL_WIDTH, 145);
    lv_obj_set_pos(stint_timer_obj, 0, 0);
    apply_styling(stint_timer_obj, LV_BORDER_SIDE_RIGHT);
    
    lv_obj_t* label = lv_label_create(stint_timer_obj);
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "16:32");
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lap_timer_obj = lv_obj_create(screen);
    lv_obj_set_size(lap_timer_obj, COL_WIDTH, HEIGHT - 2 * 145 + 2 * BORDER_SIZE);
    lv_obj_set_pos(lap_timer_obj, 0, 145 - BORDER_SIZE);
    apply_styling(lap_timer_obj, LV_BORDER_SIDE_RIGHT);

    radio_obj = lv_obj_create(screen);
    lv_obj_set_size(radio_obj, COL_WIDTH, 145);
    lv_obj_set_pos(radio_obj, 0, 480 - 145);
    apply_styling(radio_obj, LV_BORDER_SIDE_RIGHT );

    oil_obj = lv_obj_create(screen);
    lv_obj_set_size(oil_obj, COL_WIDTH, 160);
    lv_obj_set_pos(oil_obj, WIDTH-COL_WIDTH, 0);
    apply_styling(oil_obj, LV_BORDER_SIDE_LEFT);

    h2o_obj = lv_obj_create(screen);
    lv_obj_set_size(h2o_obj, COL_WIDTH, 160 + 2 * BORDER_SIZE);
    lv_obj_set_pos(h2o_obj, WIDTH-COL_WIDTH, 160 - BORDER_SIZE);
    apply_styling(h2o_obj, LV_BORDER_SIDE_LEFT);

    gas_obj = lv_obj_create(screen);
    lv_obj_set_size(gas_obj, COL_WIDTH, 160);
    lv_obj_set_pos(gas_obj, WIDTH-COL_WIDTH, 480 - 160);
    apply_styling(gas_obj, LV_BORDER_SIDE_LEFT);
}

void lvgl_draw_main_ui(lv_disp_t *disp)
{
    lv_obj_t* screen = lv_disp_get_scr_act(disp);
    lv_obj_set_style_bg_color(screen, lv_color_black(),LV_PART_MAIN);

    create_boxes(screen);

    

}

