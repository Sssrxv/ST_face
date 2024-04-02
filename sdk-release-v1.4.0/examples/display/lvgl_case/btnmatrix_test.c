#include "lvgl.h"
#include "syslog.h"
#include "indev.h"
#include "aiva_sleep.h"

static indev_dev_t* sim_touchpad_dev = NULL;
static int matrixIndex = 0;
static int setpos = 1;

typedef struct 
{
    int x;
    int y;
} keypadpos_t;
static keypadpos_t MatrixPos[] = {
    {120, 520}, {240, 520}, {360, 520},
    {120, 600}, {240, 600}, {360, 600},
    {120, 700}, {240, 700}, {360, 700}, 
    {120, 780}, {240, 780}, {360, 780}, 
};

static void in_text_cb_handler(lv_event_t* e)
{
    lv_obj_t* ta = lv_event_get_target(e);
    LOGI("", "Enter was pressed. The current text is: %s", lv_textarea_get_text(ta));
}

static void btnmat_event_handler(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_obj_t* ta = lv_event_get_user_data(e);
    const char* txt = lv_btnmatrix_get_btn_text(obj, lv_btnmatrix_get_selected_btn(obj));
    // LOGI(__func__, "btn matrix: %s.", txt);

    if (strcmp(txt, LV_SYMBOL_BACKSPACE) == 0) {
        lv_textarea_del_char(ta);
    }
    else if (strcmp(txt, LV_SYMBOL_NEW_LINE) == 0) {
        lv_event_send(ta, LV_EVENT_READY, NULL);
    }
    else {
        lv_textarea_add_text(ta, txt);
    }
}

// timer for sim-keypad
void sim_keypressed_cb(struct _lv_timer_t *timer)
{
    if(setpos) {
        indev_set_pressed_pos(sim_touchpad_dev, MatrixPos[matrixIndex % 12].x, MatrixPos[matrixIndex % 12].y);
        matrixIndex++;
        if(matrixIndex == 12) {
            setpos = 0;
            matrixIndex = 0;
            lv_timer_set_period(timer, 300);
        }
    }else {
        indev_set_pressed_pos(sim_touchpad_dev, MatrixPos[9].x, MatrixPos[9].y);
        if(matrixIndex++ == 10) {
            setpos = 1;
            matrixIndex = 0;
            lv_timer_set_period(timer, 1000);
        }
    }
}

void btn_matrix_test_case(void)
{
    lv_disp_t* disp = lv_disp_get_default();
    int w = disp->driver->hor_res;
    int h = disp->driver->ver_res;
    LOGI(__func__, "display w:%d, h:%d.", w, h);

    // create text input widget.
    lv_obj_t* in_text = lv_textarea_create(lv_scr_act());
    lv_textarea_set_placeholder_text(in_text, "Please input Passwd");
    lv_textarea_set_password_mode(in_text, false);
    // lv_textarea_set_password_bullet(in_text, "*");
    lv_textarea_set_one_line(in_text, true);
    lv_obj_align(in_text, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_add_event_cb(in_text, in_text_cb_handler, LV_EVENT_READY, in_text);
    lv_obj_add_state(in_text, LV_STATE_FOCUSED);

    // setup keyboard.
    static const char * btnm_map[] = {"1", "2", "3", "\n",
                                      "4", "5", "6", "\n",
                                      "7", "8", "9", "\n",
                                      LV_SYMBOL_BACKSPACE, LV_SYMBOL_BELL, LV_SYMBOL_NEW_LINE, ""
                                     };
    lv_obj_t* btnmat = lv_btnmatrix_create(lv_scr_act());
    lv_obj_set_size(btnmat, w-20, h/2+20);
    lv_obj_align(btnmat, LV_ALIGN_BOTTOM_MID, 0, -10); // LV_ALIGN_BOTTOM_MID
    lv_obj_add_event_cb(btnmat, btnmat_event_handler, LV_EVENT_VALUE_CHANGED, in_text);
    lv_obj_clear_flag(btnmat, LV_OBJ_FLAG_CLICK_FOCUSABLE); /*To keep the text area focused on button clicks*/
    lv_btnmatrix_set_map(btnmat, btnm_map);

    // sim_touchpad_dev = indev_init("touchpad_sim");
    // configASSERT(sim_touchpad_dev != NULL);
    // indev_enable(sim_touchpad_dev);
    // // lv_timer_create(sim_keypressed_cb, 1000, NULL);
}
