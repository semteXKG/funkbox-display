#include "lvgl.h"
#include "esp_log.h"

char* TAG = "main_draw";

#define HEIGHT 480
#define WIDTH 800
#define COL_WIDTH 180
#define DESC_WIDTH 30
#define BORDER_SIZE 3

lv_style_t style;
lv_style_t style_spacing;
lv_obj_t* stint_rem_obj;
lv_obj_t* lap_timer_obj;
lv_obj_t* radio_obj;
lv_obj_t* stint_rem_desc_obj;
lv_obj_t* lap_timer_desc_obj;
lv_obj_t* radio_desc_obj;
lv_obj_t* oil_obj;
lv_obj_t* h2o_obj;
lv_obj_t* gas_obj;
lv_obj_t* oil_desc_obj;
lv_obj_t* h2o_desc_obj;
lv_obj_t* gas_desc_obj;

lv_obj_t* remaining_time;
lv_obj_t* pb_time;
lv_obj_t* ll_time;
lv_obj_t* ll_time_diff;

lv_obj_t* radio_last_message;
lv_obj_t* radio_last_message_time_ago;
lv_obj_t* h2o_temp_message;
lv_obj_t* oil_temp_message;
lv_obj_t* oil_pres_message;
lv_obj_t* gas_pres_message; 

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

void apply_styling(lv_obj_t* obj, lv_border_side_t additional_side)  {
    lv_obj_set_style_bg_color (obj, lv_cont_bg(), LV_PART_MAIN);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xCFCFCF), LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN);
    lv_obj_set_style_border_side(obj, additional_side | LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
}

void create_stint_timer(lv_obj_t* screen) {
    stint_rem_obj = lv_obj_create(screen);
    lv_obj_set_size(stint_rem_obj, COL_WIDTH, 145);
    lv_obj_set_pos(stint_rem_obj, 0, 0);
    apply_styling(stint_rem_obj, LV_BORDER_SIDE_RIGHT);
    
    remaining_time = lv_label_create(stint_rem_obj);
    lv_obj_add_style(remaining_time, &style, 0);
    lv_label_set_text(remaining_time, "06:20");
    lv_obj_set_style_text_color(remaining_time, lv_color_warn(), LV_PART_MAIN);
    lv_obj_align(remaining_time, LV_ALIGN_CENTER, 0, 0);
}

void create_lap_timer(lv_obj_t* screen) {
    lap_timer_obj = lv_obj_create(screen);
    lv_obj_set_size(lap_timer_obj, COL_WIDTH, HEIGHT - 2 * 145 + 2 * BORDER_SIZE);
    lv_obj_set_pos(lap_timer_obj, 0, 145 - BORDER_SIZE);
    apply_styling(lap_timer_obj, LV_BORDER_SIDE_RIGHT);

    pb_time = lv_label_create(lap_timer_obj);
    lv_obj_set_style_text_font(pb_time, &lv_font_montserrat_48, 0);
    lv_label_set_text(pb_time, "1:16.10");
    lv_obj_set_style_text_color(pb_time, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(pb_time, LV_ALIGN_TOP_MID, 0, 0);

    ll_time = lv_label_create(lap_timer_obj);
    lv_obj_set_style_text_font(ll_time, &lv_font_montserrat_48, 0);
    lv_label_set_text(ll_time, "1:17.66");
    lv_obj_set_style_text_color(ll_time, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(ll_time, LV_ALIGN_CENTER, 0, 0);

    ll_time_diff = lv_label_create(lap_timer_obj);
    lv_obj_set_style_text_font(ll_time_diff, &lv_font_montserrat_48, 0);
    lv_label_set_text(ll_time_diff, "-0.06");
    lv_obj_set_style_text_color(ll_time_diff, lv_color_ok(), LV_PART_MAIN);
    lv_obj_align_to(ll_time_diff, ll_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}


void create_desc_containers_right(lv_obj_t* screen) {
    oil_desc_obj = lv_obj_create(screen);
    lv_obj_set_size(oil_desc_obj, DESC_WIDTH, 160);
    lv_obj_align_to(oil_desc_obj, oil_obj, LV_ALIGN_OUT_LEFT_MID, 7, 0);
    lv_obj_move_background(oil_desc_obj);
    apply_styling(oil_desc_obj, LV_BORDER_SIDE_LEFT);

    h2o_desc_obj = lv_obj_create(screen);
    lv_obj_set_size(h2o_desc_obj, DESC_WIDTH, HEIGHT - 2 * 160 + 2 * BORDER_SIZE);
    lv_obj_align_to(h2o_desc_obj, h2o_obj, LV_ALIGN_OUT_LEFT_MID, 7, 0);
    lv_obj_move_background(h2o_desc_obj);
    apply_styling(h2o_desc_obj, LV_BORDER_SIDE_LEFT);

    gas_desc_obj = lv_obj_create(screen);
    lv_obj_set_size(gas_desc_obj, DESC_WIDTH, 160);
    lv_obj_align_to(gas_desc_obj, gas_obj, LV_ALIGN_OUT_LEFT_MID, 7, 0);
    lv_obj_move_background(gas_desc_obj);
    apply_styling(gas_desc_obj, LV_BORDER_SIDE_LEFT);

    lv_obj_set_style_bg_color(oil_desc_obj, lv_desc_bg(), 0);
    lv_obj_set_style_bg_color(h2o_desc_obj, lv_desc_bg(), 0);
    lv_obj_set_style_bg_color(gas_desc_obj, lv_desc_bg(), 0);
}

void create_desc_labels_right(lv_obj_t* screen) {
    lv_obj_t* oil_desc_lbl = lv_label_create(oil_desc_obj);
    lv_obj_add_style(oil_desc_lbl, &style, 0);
    lv_label_set_text(oil_desc_lbl, "O\nI\nL");
    lv_obj_set_style_text_color(oil_desc_lbl, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(oil_desc_lbl, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(oil_desc_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(oil_desc_lbl, LV_ALIGN_CENTER, -2, 0);


    lv_obj_t* h2o_desc_lbl = lv_label_create(h2o_desc_obj);
    lv_obj_add_style(h2o_desc_lbl, &style, 0);
    lv_label_set_text(h2o_desc_lbl, "H\n2\nO");
    lv_obj_set_style_text_color(h2o_desc_lbl, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(h2o_desc_lbl, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(h2o_desc_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(h2o_desc_lbl, LV_ALIGN_CENTER, -2, 0);

    lv_obj_t* gas_desc_lbl = lv_label_create(gas_desc_obj);
    lv_obj_add_style(gas_desc_lbl, &style, 0);
    lv_label_set_text(gas_desc_lbl, "G\nA\nS");
    lv_obj_set_style_text_color(gas_desc_lbl, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(gas_desc_lbl, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(gas_desc_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(gas_desc_lbl, LV_ALIGN_CENTER, -2, 0);
}

void create_desc_containers_left(lv_obj_t* screen) {
    stint_rem_desc_obj = lv_obj_create(screen);
    lv_obj_set_size(stint_rem_desc_obj, DESC_WIDTH, 145);
    lv_obj_align_to(stint_rem_desc_obj, stint_rem_obj, LV_ALIGN_OUT_RIGHT_MID, -7, 0);
    lv_obj_move_background(stint_rem_desc_obj);
    apply_styling(stint_rem_desc_obj, LV_BORDER_SIDE_RIGHT);

    lap_timer_desc_obj = lv_obj_create(screen);
    lv_obj_set_size(lap_timer_desc_obj, DESC_WIDTH, HEIGHT - 2 * 145 + 2 * BORDER_SIZE);
    lv_obj_align_to(lap_timer_desc_obj, lap_timer_obj, LV_ALIGN_OUT_RIGHT_MID, -7, 0);
    lv_obj_move_background(lap_timer_desc_obj);
    apply_styling(lap_timer_desc_obj, LV_BORDER_SIDE_RIGHT);

    radio_desc_obj = lv_obj_create(screen);
    lv_obj_set_size(radio_desc_obj, DESC_WIDTH, 145);
    lv_obj_align_to(radio_desc_obj, radio_obj, LV_ALIGN_OUT_RIGHT_MID, -7, 0);
    lv_obj_move_background(radio_desc_obj);
    apply_styling(radio_desc_obj, LV_BORDER_SIDE_RIGHT);

    lv_obj_set_style_bg_color(stint_rem_desc_obj, lv_desc_bg(), 0);
    lv_obj_set_style_bg_color(lap_timer_desc_obj, lv_desc_bg(), 0);
    lv_obj_set_style_bg_color(radio_desc_obj, lv_desc_bg(), 0);
}

void create_desc_labels_left(lv_obj_t* screen) {
    lv_obj_t* pb_desc = lv_label_create(lap_timer_desc_obj);
    lv_obj_add_style(pb_desc, &style, 0);
    lv_label_set_text(pb_desc, "P\nB");
    lv_obj_set_style_text_color(pb_desc, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(pb_desc, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(pb_desc, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(pb_desc, LV_ALIGN_TOP_MID, 2, 0);


    lv_obj_t* ll_desc = lv_label_create(lap_timer_desc_obj);
    lv_obj_add_style(ll_desc, &style, 0);
    lv_label_set_text(ll_desc, "L\nA\nS\nT");
    lv_obj_set_style_text_color(ll_desc, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(ll_desc, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(ll_desc, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(ll_desc, LV_ALIGN_CENTER, 2, 25);


   lv_obj_t* box_descr = lv_label_create(stint_rem_desc_obj);
    lv_obj_add_style(box_descr, &style, 0);
    lv_label_set_text(box_descr, "S\nT\nI\nN\nT");
    lv_obj_set_style_text_color(box_descr, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(box_descr, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(box_descr, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(box_descr, LV_ALIGN_CENTER, 2, 0);

    lv_obj_t* radio_desc = lv_label_create(radio_desc_obj);
    lv_obj_add_style(radio_desc, &style, 0);
    lv_label_set_text(radio_desc, "C\nO\nM\nM\nS");
    lv_obj_set_style_text_color(radio_desc, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(radio_desc, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(radio_desc, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(radio_desc, LV_ALIGN_CENTER, 2, 0);
}

void create_radio_timer(lv_obj_t* screen) {
    radio_obj = lv_obj_create(screen);
    lv_obj_set_size(radio_obj, COL_WIDTH, 145);
    lv_obj_set_pos(radio_obj, 0, 480 - 145);
    apply_styling(radio_obj, LV_BORDER_SIDE_RIGHT);

    radio_last_message = lv_label_create(radio_obj);
    lv_obj_set_style_text_font(radio_last_message, &lv_font_montserrat_48, 0);
    lv_label_set_text(radio_last_message, "PIT");
    lv_obj_set_style_text_color(radio_last_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(radio_last_message, LV_ALIGN_CENTER, 0, 0);

    radio_last_message_time_ago = lv_label_create(radio_obj);
    lv_obj_set_style_text_font(radio_last_message_time_ago, &lv_font_montserrat_28, 0);
    lv_label_set_text(radio_last_message_time_ago, "4m ago");
    lv_obj_set_style_text_color(radio_last_message_time_ago, lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(radio_last_message_time_ago, radio_last_message, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}


void create_h2o_status(lv_obj_t* screen) {
    h2o_obj = lv_obj_create(screen);
    lv_obj_set_size(h2o_obj, COL_WIDTH, 160 + 2 * BORDER_SIZE);
    lv_obj_set_pos(h2o_obj, WIDTH-COL_WIDTH, 160 - BORDER_SIZE);
    apply_styling(h2o_obj, LV_BORDER_SIDE_LEFT);

    h2o_temp_message = lv_label_create(h2o_obj);
    lv_obj_set_style_text_font(h2o_temp_message, &lv_font_montserrat_48, 0);
    lv_label_set_text(h2o_temp_message, "90");
    lv_obj_set_style_text_color(h2o_temp_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(h2o_temp_message, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* h2o_temp_currency = lv_label_create(h2o_obj);
    lv_obj_set_style_text_font(h2o_temp_currency, &lv_font_montserrat_20, 0);
    lv_label_set_text(h2o_temp_currency, "°C");
    lv_obj_set_style_text_color(h2o_temp_currency, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(h2o_temp_currency, LV_ALIGN_RIGHT_MID, 0, 0);
}


void create_oil_status(lv_obj_t* screen) {
    oil_obj = lv_obj_create(screen);
    lv_obj_set_size(oil_obj, COL_WIDTH, 160);
    lv_obj_set_pos(oil_obj, WIDTH-COL_WIDTH, 0);
    apply_styling(oil_obj, LV_BORDER_SIDE_LEFT);

    oil_temp_message = lv_label_create(oil_obj);
    lv_obj_set_style_text_font(oil_temp_message, &lv_font_montserrat_48, 0);
    lv_label_set_text(oil_temp_message, "130");
    lv_obj_set_style_text_color(oil_temp_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(oil_temp_message, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t* oil_temp_currency = lv_label_create(oil_obj);
    lv_obj_set_style_text_font(oil_temp_currency, &lv_font_montserrat_20, 0);
    lv_label_set_text(oil_temp_currency, "°C");
    lv_obj_set_style_text_color(oil_temp_currency, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(oil_temp_currency, LV_ALIGN_TOP_RIGHT, 0, 8);

    oil_pres_message = lv_label_create(oil_obj);
    lv_obj_set_style_text_font(oil_pres_message, &lv_font_montserrat_48, 0);
    lv_label_set_text(oil_pres_message, "6.4");
    lv_obj_set_style_text_color(oil_pres_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(oil_pres_message, LV_ALIGN_BOTTOM_LEFT, 0, 0);


    lv_obj_t* oil_pres_curency = lv_label_create(oil_obj);
    lv_obj_set_style_text_font(oil_pres_curency, &lv_font_montserrat_20, 0);
    lv_label_set_text(oil_pres_curency, "bar");
    lv_obj_set_style_text_color(oil_pres_curency, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(oil_pres_curency, LV_ALIGN_BOTTOM_RIGHT, 0, -8);
}

void create_gas_status(lv_obj_t* screen) {
    gas_obj = lv_obj_create(screen);
    lv_obj_set_size(gas_obj, COL_WIDTH, 160);
    lv_obj_set_pos(gas_obj, WIDTH-COL_WIDTH, 480 - 160);
    apply_styling(gas_obj, LV_BORDER_SIDE_LEFT);


    gas_pres_message = lv_label_create(gas_obj);
    lv_obj_set_style_text_font(gas_pres_message, &lv_font_montserrat_48, 0);
    lv_label_set_text(gas_pres_message, "2.3");
    lv_obj_set_style_text_color(gas_pres_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(gas_pres_message, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* gas_pres_currency = lv_label_create(gas_obj);
    lv_obj_set_style_text_font(gas_pres_currency, &lv_font_montserrat_20, 0);
    lv_label_set_text(gas_pres_currency, "bar");
    lv_obj_set_style_text_color(gas_pres_currency, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(gas_pres_currency, LV_ALIGN_RIGHT_MID, 0, 0);
}

void create_content(lv_obj_t* screen) {
    create_stint_timer(screen);
    create_lap_timer(screen);
    create_radio_timer(screen);

    create_h2o_status(screen);
    create_oil_status(screen);
    create_gas_status(screen);

    create_desc_containers_left(screen);
    create_desc_labels_left(screen);

    create_desc_containers_right(screen);
    create_desc_labels_right(screen);
}

void lvgl_draw_main_ui(lv_disp_t *disp)
{
    lv_obj_t* screen = lv_disp_get_scr_act(disp);
    lv_obj_set_style_bg_color(screen, lv_main_bg(),LV_PART_MAIN);

    lv_style_init(&style);
    lv_style_set_text_font(&style, &lv_font_montserrat_48);
    lv_style_set_shadow_color(&style, lv_color_hex(0xEF7215));

    lv_style_init(&style_spacing);
    lv_style_set_text_letter_space(&style_spacing, 150);

    create_content(screen);
}

