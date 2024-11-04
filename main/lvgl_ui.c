#include "lvgl_ui.h"

char* TAG_MAIN = "main_draw";

#define HEIGHT 480
#define WIDTH 800
#define COL_WIDTH 180
#define DESC_WIDTH 30
#define BORDER_SIZE 3

lv_style_t style;
lv_style_t style_padding;
lv_obj_t* stint_rem_obj;
lv_obj_t* lap_timer_obj;
lv_obj_t* radio_obj;
lv_obj_t* stint_rem_desc_obj;
lv_obj_t* lap_timer_desc_obj;
lv_obj_t* radio_desc_obj;
lv_obj_t* oil_obj;
lv_obj_t* gps_obj;
lv_obj_t* gas_obj;
lv_obj_t* oil_desc_obj;
lv_obj_t* gps_desc_obj;
lv_obj_t* gas_desc_obj;

lv_obj_t* main_laps_obj;
lv_obj_t* main_stint_obj;
lv_obj_t* main_events_obj;

lv_obj_t* remaining_time;
lv_obj_t* pb_time;
lv_obj_t* ll_time;
lv_obj_t* ll_time_diff;

lv_obj_t* radio_last_message;
lv_obj_t* radio_last_message_time_ago;
lv_obj_t* gps_spd_message;
lv_obj_t* gps_coord_message;
lv_obj_t* oil_temp_message;
lv_obj_t* oil_pres_message;
lv_obj_t* gas_pres_message; 

lv_obj_t* lap_number_labels[5];
lv_obj_t* lap_time_labels[5];
lv_obj_t* lap_diff_labels[5];

lv_obj_t* main_events_label;

LV_FONT_DECLARE(lv_immono_20);
LV_FONT_DECLARE(lv_immono_28);
LV_FONT_DECLARE(lv_immono_38);
LV_FONT_DECLARE(lv_immono_40);
LV_FONT_DECLARE(lv_immono_48);
LV_FONT_DECLARE(lv_immono_bold_80);


void apply_styling(lv_obj_t* obj, lv_border_side_t additional_side)  {
    lv_obj_set_style_bg_color (obj, lv_cont_bg(), LV_PART_MAIN);
    lv_obj_set_style_border_color(obj, lv_border_color(), LV_PART_MAIN);
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
    lv_obj_set_style_text_font(pb_time, &lv_immono_40, 0);
    lv_label_set_text(pb_time, "-");
    lv_obj_set_style_text_color(pb_time, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(pb_time, LV_ALIGN_TOP_MID, 0, 0);

    ll_time = lv_label_create(lap_timer_obj);
    lv_obj_set_style_text_font(ll_time, &lv_immono_40, 0);
    lv_label_set_text(ll_time, "-");
    lv_obj_set_style_text_color(ll_time, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(ll_time, LV_ALIGN_CENTER, 0, 0);

    ll_time_diff = lv_label_create(lap_timer_obj);
    lv_obj_set_style_text_font(ll_time_diff, &lv_immono_40, 0);
    lv_label_set_text(ll_time_diff, "");

    #if PRIMARY
    lv_obj_set_style_text_color(ll_time_diff, lv_color_ok(), LV_PART_MAIN);
    #else
    lv_obj_set_style_text_color(ll_time_diff, lv_color_white(), LV_PART_MAIN);
    #endif

    lv_obj_set_style_text_align(ll_time_diff, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(ll_time_diff, LV_ALIGN_BOTTOM_MID, 0, 0);

    //lv_obj_align_to(ll_time_diff, ll_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}


void create_desc_containers_right(lv_obj_t* screen) {
    oil_desc_obj = lv_obj_create(screen);
    lv_obj_set_size(oil_desc_obj, DESC_WIDTH, 160);
    lv_obj_align_to(oil_desc_obj, oil_obj, LV_ALIGN_OUT_LEFT_MID, 7, 0);
    lv_obj_move_background(oil_desc_obj);
    apply_styling(oil_desc_obj, LV_BORDER_SIDE_LEFT);

    gps_desc_obj = lv_obj_create(screen);
    lv_obj_set_size(gps_desc_obj, DESC_WIDTH, HEIGHT - 2 * 160 + 2 * BORDER_SIZE);
    lv_obj_align_to(gps_desc_obj, gps_obj, LV_ALIGN_OUT_LEFT_MID, 7, 0);
    lv_obj_move_background(gps_desc_obj);
    apply_styling(gps_desc_obj, LV_BORDER_SIDE_LEFT);

    gas_desc_obj = lv_obj_create(screen);
    lv_obj_set_size(gas_desc_obj, DESC_WIDTH, 160);
    lv_obj_align_to(gas_desc_obj, gas_obj, LV_ALIGN_OUT_LEFT_MID, 7, 0);
    lv_obj_move_background(gas_desc_obj);
    apply_styling(gas_desc_obj, LV_BORDER_SIDE_LEFT);

    lv_obj_set_style_bg_color(oil_desc_obj, lv_desc_bg(), 0);
    lv_obj_set_style_bg_color(gps_desc_obj, lv_desc_bg(), 0);
    lv_obj_set_style_bg_color(gas_desc_obj, lv_desc_bg(), 0);
}

void create_desc_labels_right(lv_obj_t* screen) {
    lv_obj_t* oil_desc_lbl = lv_label_create(oil_desc_obj);
    lv_obj_add_style(oil_desc_lbl, &style, 0);
    lv_label_set_text(oil_desc_lbl, "O\nI\nL");
    lv_obj_set_style_text_color(oil_desc_lbl, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(oil_desc_lbl, &lv_immono_20, 0);
    lv_obj_set_style_text_align(oil_desc_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(oil_desc_lbl, LV_ALIGN_CENTER, -2, 0);


    lv_obj_t* h2o_desc_lbl = lv_label_create(gps_desc_obj);
    lv_obj_add_style(h2o_desc_lbl, &style, 0);
    lv_label_set_text(h2o_desc_lbl, "G\nP\nS");
    lv_obj_set_style_text_color(h2o_desc_lbl, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(h2o_desc_lbl, &lv_immono_20, 0);
    lv_obj_set_style_text_align(h2o_desc_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(h2o_desc_lbl, LV_ALIGN_CENTER, -2, 0);

    lv_obj_t* gas_desc_lbl = lv_label_create(gas_desc_obj);
    lv_obj_add_style(gas_desc_lbl, &style, 0);
    lv_label_set_text(gas_desc_lbl, "G\nA\nS");
    lv_obj_set_style_text_color(gas_desc_lbl, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(gas_desc_lbl, &lv_immono_20, 0);
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
    #if PRIMARY
    lv_label_set_text(pb_desc, "P\nB");
    lv_obj_align(pb_desc, LV_ALIGN_TOP_MID, 2, 0);
    #else
    lv_label_set_text(pb_desc, "L\nO\nR\nA");
    lv_obj_align(pb_desc, LV_ALIGN_CENTER, 2, 0);
    #endif
    lv_obj_set_style_text_color(pb_desc, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(pb_desc, &lv_immono_20, 0);
    lv_obj_set_style_text_align(pb_desc, LV_TEXT_ALIGN_CENTER, 0);


    #if PRIMARY
    lv_obj_t* ll_desc = lv_label_create(lap_timer_desc_obj);
    lv_obj_add_style(ll_desc, &style, 0);
    lv_label_set_text(ll_desc, "L\nA\nS\nT");
    lv_obj_set_style_text_color(ll_desc, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(ll_desc, &lv_immono_20, 0);
    lv_obj_set_style_text_align(ll_desc, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(ll_desc, LV_ALIGN_CENTER, 2, 25);
    #endif


   lv_obj_t* box_descr = lv_label_create(stint_rem_desc_obj);
    lv_obj_add_style(box_descr, &style, 0);
    lv_label_set_text(box_descr, "S\nT\nI\nN\nT");
    lv_obj_set_style_text_color(box_descr, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(box_descr, &lv_immono_20, 0);
    lv_obj_set_style_text_align(box_descr, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(box_descr, LV_ALIGN_CENTER, 2, 0);

    lv_obj_t* radio_desc = lv_label_create(radio_desc_obj);
    lv_obj_add_style(radio_desc, &style, 0);
    lv_label_set_text(radio_desc, "C\nO\nM\nM\nS");
    lv_obj_set_style_text_color(radio_desc, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(radio_desc, &lv_immono_20, 0);
    lv_obj_set_style_text_align(radio_desc, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(radio_desc, LV_ALIGN_CENTER, 2, 0);
}

void create_radio_timer(lv_obj_t* screen) {
    radio_obj = lv_obj_create(screen);
    lv_obj_set_size(radio_obj, COL_WIDTH, 145);
    lv_obj_set_pos(radio_obj, 0, 480 - 145);
    apply_styling(radio_obj, LV_BORDER_SIDE_RIGHT);

    radio_last_message = lv_label_create(radio_obj);
    lv_obj_set_style_text_font(radio_last_message, &lv_immono_48, 0);
    lv_label_set_text(radio_last_message, "OFF");
    lv_obj_set_style_text_color(radio_last_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(radio_last_message, LV_ALIGN_TOP_MID, 0, 0);

    radio_last_message_time_ago = lv_label_create(radio_obj);
    lv_obj_set_style_text_font(radio_last_message_time_ago, &lv_immono_28, 0);
    lv_label_set_text(radio_last_message_time_ago, "-");
    lv_obj_set_style_text_color(radio_last_message_time_ago, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(radio_last_message_time_ago, LV_ALIGN_BOTTOM_MID, 0, 0);
}


void create_h2o_status(lv_obj_t* screen) {
    gps_obj = lv_obj_create(screen);
    lv_obj_set_size(gps_obj, COL_WIDTH, 160 + 2 * BORDER_SIZE);
    lv_obj_set_pos(gps_obj, WIDTH-COL_WIDTH, 160 - BORDER_SIZE);
    apply_styling(gps_obj, LV_BORDER_SIDE_LEFT);

    gps_spd_message = lv_label_create(gps_obj);
    lv_obj_set_style_text_font(gps_spd_message, &lv_immono_48, 0);
    lv_label_set_text(gps_spd_message, "140");
    lv_obj_set_style_text_color(gps_spd_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(gps_spd_message, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t* h2o_temp_currency = lv_label_create(gps_obj);
    lv_obj_set_style_text_font(h2o_temp_currency, &lv_immono_20, 0);
    lv_label_set_text(h2o_temp_currency, "kmh");
    lv_obj_set_style_text_color(h2o_temp_currency, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(h2o_temp_currency, LV_ALIGN_TOP_RIGHT, 0, 0);

    gps_coord_message = lv_label_create(gps_obj);
    lv_obj_set_style_text_font(gps_coord_message, &lv_immono_28, 0);
    lv_label_set_text(gps_coord_message, "50.323|23.234");
    lv_obj_set_style_text_color(gps_coord_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(gps_coord_message, LV_ALIGN_BOTTOM_LEFT, 0, 0);
}


void create_oil_status(lv_obj_t* screen) {
    oil_obj = lv_obj_create(screen);
    lv_obj_set_size(oil_obj, COL_WIDTH, 160);
    lv_obj_set_pos(oil_obj, WIDTH-COL_WIDTH, 0);
    apply_styling(oil_obj, LV_BORDER_SIDE_LEFT);

    oil_temp_message = lv_label_create(oil_obj);
    lv_obj_set_style_text_font(oil_temp_message, &lv_immono_48, 0);
    lv_label_set_text(oil_temp_message, "130");
    lv_obj_set_style_text_color(oil_temp_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(oil_temp_message, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t* oil_temp_currency = lv_label_create(oil_obj);
    lv_obj_set_style_text_font(oil_temp_currency, &lv_immono_20, 0);
    lv_label_set_text(oil_temp_currency, "°C");
    lv_obj_set_style_text_color(oil_temp_currency, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(oil_temp_currency, LV_ALIGN_TOP_RIGHT, 0, 8);

    oil_pres_message = lv_label_create(oil_obj);
    lv_obj_set_style_text_font(oil_pres_message, &lv_immono_48, 0);
    lv_label_set_text(oil_pres_message, "6.4");
    lv_obj_set_style_text_color(oil_pres_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(oil_pres_message, LV_ALIGN_BOTTOM_LEFT, 0, 0);


    lv_obj_t* oil_pres_curency = lv_label_create(oil_obj);
    lv_obj_set_style_text_font(oil_pres_curency, &lv_immono_20, 0);
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
    lv_obj_set_style_text_font(gas_pres_message, &lv_immono_48, 0);
    lv_label_set_text(gas_pres_message, "2.3");
    lv_obj_set_style_text_color(gas_pres_message, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(gas_pres_message, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* gas_pres_currency = lv_label_create(gas_obj);
    lv_obj_set_style_text_font(gas_pres_currency, &lv_immono_20, 0);
    lv_label_set_text(gas_pres_currency, "bar");
    lv_obj_set_style_text_color(gas_pres_currency, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(gas_pres_currency, LV_ALIGN_RIGHT_MID, 0, 0);
}

void create_lap_child_element(lv_obj_t* container, int i) {

    lv_obj_t* sub_obj = lv_obj_create(container);
    lv_obj_set_size(sub_obj, 800 - 2*205, HEIGHT/5);
    lv_obj_align(sub_obj, LV_ALIGN_TOP_MID, 0, (HEIGHT/5) * i);
    lv_obj_set_style_bg_color(sub_obj, lv_main_bg(), LV_PART_MAIN);
    lv_obj_set_style_border_width(sub_obj, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(sub_obj, lv_border_color(), 0);
    lv_obj_set_style_border_side(sub_obj, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);

    lv_obj_t* lap_time = lv_label_create(sub_obj);
    lv_label_set_text(lap_time, "");
    lv_obj_set_style_text_color(lap_time, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(lap_time, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(lap_time, &lv_immono_38, LV_PART_MAIN);

    lap_time_labels[i] = lap_time;
    
    lv_obj_t* lap_no = lv_label_create(sub_obj);
    lv_label_set_text(lap_no, "");
    lv_obj_set_style_text_color(lap_no, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_align(lap_no, LV_ALIGN_LEFT_MID);
    lv_obj_set_style_text_font(lap_no, &lv_immono_28, LV_PART_MAIN);
    lap_number_labels[i] = lap_no;
    // no lap + diff as first is LIVE
    #if PRIMARY
        if (i == 0) {
            return; 
        }
    #endif
    
    lv_obj_t* lap_diff = lv_label_create(sub_obj);
    lv_label_set_text(lap_diff, "");
    lv_obj_set_style_text_color(lap_diff, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_align(lap_diff, LV_ALIGN_RIGHT_MID);
    lv_obj_set_style_text_font(lap_diff, &lv_immono_28, LV_PART_MAIN);
    lap_diff_labels[i] = lap_diff;

}

void create_main_laps(lv_obj_t* screen) {
    lv_style_t style;
    lv_style_init(&style);
    
    main_laps_obj = lv_obj_create(screen);
    lv_obj_set_content_width(main_laps_obj, 800 - 2*205);
    lv_obj_set_content_height(main_laps_obj, HEIGHT);
    lv_obj_set_size(main_laps_obj, 800 - 2*205, HEIGHT);
    lv_obj_align(main_laps_obj, LV_ALIGN_TOP_MID, 0, 0); 
    lv_obj_set_style_bg_color (main_laps_obj, lv_main_bg(), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(main_laps_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(main_laps_obj, 0, LV_PART_MAIN);
    lv_obj_set_style_border_color(main_laps_obj, lv_color_crit(), 0);
    
    lv_obj_add_style(main_laps_obj, &style_padding, 0);
    
    for (int i = 0; i < 5; i ++) {
        create_lap_child_element(main_laps_obj, i);
    }
}

void create_notification_area(lv_obj_t* screen) {
    main_events_obj = lv_obj_create(screen);
    lv_obj_set_content_width(main_events_obj, 800 - 2*205);
    lv_obj_set_content_height(main_events_obj, HEIGHT);
    lv_obj_set_size(main_events_obj, 800 - 2*205, HEIGHT);  
    lv_obj_align(main_events_obj, LV_ALIGN_TOP_MID, 0, 0); 
    lv_obj_move_background(main_events_obj);

    main_events_label = lv_label_create(main_events_obj);
    
    lv_label_set_text(main_events_label, "");
    lv_obj_set_style_text_color(main_events_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_align(main_events_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(main_events_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(main_events_label, &lv_immono_bold_80, LV_PART_MAIN);
    lv_obj_set_style_text_line_space(main_events_label, 30, LV_PART_MAIN);
}

void create_content(lv_obj_t* screen) {
    create_notification_area(screen);

    create_stint_timer(screen);
    create_lap_timer(screen);
    create_radio_timer(screen);

    create_h2o_status(screen);
    create_oil_status(screen);
    create_gas_status(screen);

    create_main_laps(screen);

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
    lv_style_set_text_font(&style, &lv_immono_48);
    lv_style_set_shadow_color(&style, lv_color_hex(0xEF7215));

    lv_style_init(&style_padding);
    lv_style_set_pad_all(&style_padding, 0);

    create_content(screen);
}

double checksum = -1; 
void lvgl_set_lora_stats(ProtoLoraStats* stats) {
    int time = (int)((esp_timer_get_time() / 1000) - get_lora_stats_age()) / 1000;
    double check = stats->rssi + stats->snr + time;
    if(check == checksum) {
        return;
    }
    
    checksum = check;
    char fstring[15];
    sprintf(fstring, "DMP %03.0f", stats->rssi);
    lv_label_set_text(pb_time, fstring);
    sprintf(fstring, "SNR %03.0f", stats->snr);
    lv_label_set_text(ll_time,fstring);
    sprintf(fstring, "TME %03d", time);
    lv_label_set_text(ll_time_diff, fstring);
    
}

long last_lap_checksum = -1L;
void lvgl_set_last_laps(ProtoLapData* data) {
    if(data->lap_no == 1) {
        return;
    }  

    long checksum = data->best_lap_ms + data->laps[0]->lap_time_ms;
    if(checksum == last_lap_checksum) {
        return;
    }
    last_lap_checksum = checksum;

    struct time_str pb = convert_millis_to_time(data->best_lap_ms);
    lv_label_set_text_fmt(pb_time, "%1d:%02d.%02d", pb.minutes, pb.seconds, pb.milliseconds/10);
    
    struct time_str ll = convert_millis_to_time(data->laps[0]->lap_time_ms);
    lv_label_set_text_fmt(ll_time, "%1d:%02d.%02d", ll.minutes, ll.seconds, ll.milliseconds/10);
 
    if(data->lap_no > 2) {
        long diff = data->laps[0]->lap_time_ms - data->laps[1]->lap_time_ms;
        struct time_str ll_diff = convert_millis_to_time(diff);

        lv_obj_set_style_text_color(ll_time_diff, (diff > 0 ? lv_color_crit() : lv_color_ok()), LV_PART_MAIN);

        lv_label_set_text_fmt(ll_time_diff, "%s%1d.%02d", (diff < 0 ? "-":"+"), ll_diff.seconds + (ll_diff.minutes * 60), ll_diff.milliseconds/10);
    }
}

bool previously_running = false;
long checksums[5] = {-1};
void lvgl_set_last_laps_main(uint64_t time_offset, ProtoLapData* lap_data) {    
    if(lap_data->has_current_lap_ms) {
        uint32_t data_age = (esp_timer_get_time() / 1000)  - (lap_data->current_lap_snapshot_time - time_offset);
        struct time_str time = convert_millis_to_time(lap_data->current_lap_ms + data_age);
        lv_label_set_text_fmt(lap_time_labels[0], "%d:%02d.%02d", time.minutes, time.seconds, time.milliseconds / 10);
        
        if(previously_running != lap_data->has_current_lap_ms) {
            lv_label_set_text(lap_number_labels[0], "Lap");
        }
    } else if (previously_running) {
        lv_label_set_text(lap_time_labels[0], "OFF");
        lv_label_set_text(lap_number_labels[0], "");
    }
    previously_running = lap_data->has_current_lap_ms;
    #if PRIMARY 
        int control_index = 0;
        size_t last_index = lap_data->n_laps > 4 ? 4 : lap_data->n_laps;
    #else
        int control_index = -1;
        size_t last_index = lap_data->n_laps > 5 ? 5 : lap_data->n_laps;
    #endif
    
    for (int i = 0; i < last_index; i ++) {
        control_index++;

        int64_t time_in_ms = lap_data->laps[i]->lap_time_ms;
        int64_t diff_in_ms = time_in_ms - lap_data->laps[(i == last_index - 1) ? last_index -1 : i + 1]->lap_time_ms;
        int lapNo = lap_data->laps[i]->lap_no;
        long checksum = time_in_ms + diff_in_ms + lapNo;
        if(checksum == checksums[control_index]) {
            continue;
        }
        checksums[control_index] = checksum;
        if (diff_in_ms > 99990) {
            diff_in_ms = 99990;
        }

        if (diff_in_ms < -99990) {
            diff_in_ms = -99990;
        }

        struct time_str time_in_str = convert_millis_to_time(time_in_ms);
        struct time_str diff_in_str = convert_millis_to_time(diff_in_ms);
        
        lv_label_set_text_fmt(lap_number_labels[control_index], "%d", lapNo);
        lv_label_set_text_fmt(lap_time_labels[control_index], "%d:%02d.%02d", time_in_str.minutes, time_in_str.seconds, time_in_str.milliseconds / 10);
        lv_label_set_text_fmt(
            lap_diff_labels[control_index], 
            "%s%d.%02d", 
            (diff_in_ms > 0 ? "+" : "-"), 
            diff_in_str.seconds + (60*diff_in_str.minutes), 
            diff_in_str.milliseconds / 10);
        lv_obj_set_style_text_color(lap_diff_labels[control_index], (diff_in_ms > 0 ? lv_color_crit() : lv_color_ok()), LV_PART_MAIN);
    }
}

long stint_timer_checksum = -1L;
uint64_t last_elapsed;
int cnt = 0;
void lvgl_set_stint_timer(bool enabled, bool running, int64_t target, int64_t elapsed, int32_t time_adjust) {    
    if(elapsed < last_elapsed && time_adjust < 1000) {
        return;
    }
    long checksum = enabled + running + target + (elapsed + time_adjust) / 1000;
    if (checksum == stint_timer_checksum) {
        return;
    }
    stint_timer_checksum = checksum;
    last_elapsed = elapsed;
    if (!enabled) {
        draw_as_normal(stint_rem_obj, remaining_time);
        lv_label_set_text(remaining_time, "OFF");
        return;
    }

    if (!running) {
        draw_as_normal(stint_rem_obj, remaining_time);
        lv_label_set_text(remaining_time, "PAUSE");
        return; 
    }

    long time_rem = round((double)(target - (elapsed + time_adjust)));
    bool is_neg = false;
    double elapsed_percent = target == 0 ? 0 : (double)((double)elapsed / target);
    
    //ESP_LOGI(TAG_MAIN, "target [%ld], elapsed [%ld], rem [%ld]", target, elapsed, time_rem);

    if(elapsed_percent <= 0.5) {
        draw_as_normal(stint_rem_obj, remaining_time);
    } else if (elapsed_percent < 0.8) {
        draw_as_warn(stint_rem_obj, remaining_time);
    } else {
        draw_as_critical(stint_rem_obj, remaining_time);
        if(elapsed_percent > 1) {
            is_neg = true;
            time_rem *= -1;
            #if PRIMARY
                int aligned = round((double)time_rem / 1000);
                if (aligned % 2 == 0) {
                    draw_as_normal(stint_rem_obj, remaining_time);
                }
            #endif
        }
    }
   
    struct time_str time = convert_millis_to_time(time_rem);
    time.seconds = time.seconds + round((double)time.milliseconds / 1000);
    lv_label_set_text_fmt(remaining_time, "%s%02d:%02d", is_neg ? "-": "", time.minutes, time.seconds);
}

double prev_oil_checksum = -1;
double prev_water_checksum = -1;
double prev_gas_checksum = -1;
void lvgl_set_temperatures(ProtoMcuData* data) {
    char temp[10];
    if (data->oil == NULL || data->water == NULL || data->gas == NULL) {
        return;
    }
    
    double checksum = data->oil->temp + data->oil->preassure;
    if(checksum != prev_oil_checksum) {
        prev_oil_checksum = checksum;
        sprintf(temp, "%0.1f", data->oil->preassure);
        lv_label_set_text_fmt(oil_pres_message, temp);    
        lv_label_set_text_fmt(oil_temp_message, "%"PRIu32, data->oil->temp);
        if(data->oil->preassure < data->oil_warn->preassure || data->oil->temp > data->oil_warn->temp) {
            draw_as_critical(oil_obj, oil_temp_message);
            draw_as_critical(oil_obj, oil_pres_message);
        } else {
            draw_as_normal(oil_obj, oil_temp_message);
            draw_as_normal(oil_obj, oil_pres_message);
        }    
    }

    if(prev_gas_checksum != data->gas->preassure) {
        prev_gas_checksum = data->gas->preassure;
        if(data->gas->preassure > 99 || data->gas->preassure < 0) {
            sprintf(temp, "%0.1f", 0.0);
        } else {
            sprintf(temp, "%0.1f", data->gas->preassure);
        }
        lv_label_set_text(gas_pres_message, temp); 

        if(data->gas->preassure < data->gas_warn->preassure) {
            draw_as_critical(gas_obj, gas_pres_message);
        } else {
            draw_as_normal(gas_obj, gas_pres_message);
        }
    }
}

char* type_to_string(ProtoCommandType type) {
    switch (type) {
        case PROTO__COMMAND__TYPE__COM_PIT: return "PIT";
        case PROTO__COMMAND__TYPE__COM_STINT_OVER: return "STINT";
        case PROTO__COMMAND__TYPE__COM_FCK: return "FUCK";
        default: return "";
    }
}

double prev_checksum = -1.0;
void lvgl_set_gps(ProtoGpsData* data) {
    double checksum = data->spd + data->lat + data->lon;
    if (checksum != prev_checksum) {
        prev_checksum = checksum;
        char tmp[20];
        sprintf(tmp, "%3.6f\n%3.6f", data->lat, data->lon);
        lv_label_set_text_fmt(gps_spd_message, "%"PRId32, data->spd);
        lv_label_set_text(gps_coord_message, tmp);
    }
}

int run = 0; 
long last_comms_checksum = -1;
void lvgl_set_last_comms(long timestamp_adjustment, ProtoMcuData* data) {
    ProtoCommand* newest = NULL;
    if (run++%100 == 0) {
        //ESP_LOGI(TAG_MAIN, "no_of_incomming_commands %d", data->n_incoming_commands);
    }
    for (int i = 0; i < data->n_incoming_commands; i++) {
        if (run%100 == 0) {
     //       ESP_LOGI(TAG_MAIN, "Type: %d, Created %"PRId64, data->incoming_commands[i]->type, data->incoming_commands[i]->created);
        }
        if(data->incoming_commands[i]->created > 0 && (newest == NULL || data->incoming_commands[i]->created > newest->created)) {
            newest = data->incoming_commands[i];
        }
    }
    if(newest == NULL) {
        return;
    }
    
    long time_since_in_ms = esp_timer_get_time()/1000 - newest->created - timestamp_adjustment;
    struct time_str time_since = convert_millis_to_time(time_since_in_ms);

    if (run%100 == 0) {
      //  ESP_LOGI(TAG_MAIN, "message since in ms: [%ld], min [%d]", time_since_in_ms, time_since.minutes);
    }

    long checksum = time_since.minutes + newest->created;
    if(checksum == last_comms_checksum) {
        return;
    }

    last_comms_checksum = checksum;
    lv_label_set_text(radio_last_message, type_to_string(newest->type));
    lv_label_set_text_fmt(radio_last_message_time_ago, "%d m ago", time_since.minutes);
}


void lvgl_update_data() {
    //ESP_LOGI(TAG_MAIN, "UPDATE_START");
    if (xSemaphoreTake(get_data_mutex(), pdMS_TO_TICKS(10)) == pdTRUE) {
    //    ESP_LOGI(TAG_MAIN, "SEMAPHORE_TAKEN");
        ProtoMcuData* data = get_data();
        if(data == NULL) {
            xSemaphoreGive(get_data_mutex());
            return;
        }
        int32_t time_adjust = data->send_timestamp - data->stint->elapsed_timestamp + (esp_timer_get_time() / 1000 - get_data_age());
        lvgl_set_stint_timer(data->stint->enabled, data->stint->running, data->stint->target, data->stint->elapsed, time_adjust);
        
        #if PRIMARY
            lvgl_set_last_laps(data->lap_data);
        #endif

        lvgl_set_last_laps_main(data->network_time_adjustment, data->lap_data);
        lvgl_set_last_comms(data->network_time_adjustment, data);
        lvgl_set_temperatures(data);
        lvgl_set_gps(data->gps);
        if(!lvgl_set_commands(data, main_events_obj, main_events_label)) {
            lvgl_set_events(data, main_events_obj, main_events_label);
        }
    //    ESP_LOGI(TAG_MAIN, "SEMAPHORE_GIVEN");
        xSemaphoreGive(get_data_mutex());
    } else {
        ESP_LOGI(TAG_MAIN, "Could not update UI");
    }

    #if !PRIMARY
    if (xSemaphoreTake(get_lora_mutex(), pdMS_TO_TICKS(10)) == pdTRUE) {
        ProtoLoraStats* lora_stats = get_lora_stats();
        if(lora_stats == NULL) {
            xSemaphoreGive(get_lora_mutex());
            return;
        }            
        lvgl_set_lora_stats(lora_stats);
        xSemaphoreGive(get_lora_mutex());
    } else {  
        ESP_LOGI(TAG_MAIN, "Could not update UI");
    }
    #endif

    //ESP_LOGI(TAG_MAIN, "UPDATE_END");
}