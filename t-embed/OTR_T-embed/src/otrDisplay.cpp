#include "otrDisplay.h"


extern TFT_eSPI tft;
extern RotaryEncoder encoder;
extern OneButton button;
EventGroupHandle_t lv_input_event;

void initDisplay() {
    typedef struct {
        uint8_t cmd;
        uint8_t data[14];
        uint8_t len;
    } lcd_cmd_t;

    lcd_cmd_t lcd_st7789v[] = {
        {0x11, {0}, 0 | 0x80},
        {0x3A, {0X05}, 1},
        {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
        {0xB7, {0X75}, 1},
        {0xBB, {0X28}, 1},
        {0xC0, {0X2C}, 1},
        {0xC2, {0X01}, 1},
        {0xC3, {0X1F}, 1},
        {0xC6, {0X13}, 1},
        {0xD0, {0XA7}, 1},
        {0xD0, {0XA4, 0XA1}, 2},
        {0xD6, {0XA1}, 1},
        {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
        {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
    };
        /* Enable display */
    pinMode(PIN_LCD_POWER_ON, OUTPUT);
    digitalWrite(PIN_LCD_POWER_ON, HIGH); // Turn on the display enable pin

    tft.begin();
    // Update Embed initialization parameters
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        for (int j = 0; j < lcd_st7789v[i].len & 0x7f; j++) {
            tft.writecommand(lcd_st7789v[i].cmd);
            tft.writedata(lcd_st7789v[i].data[j]);
        }
        if (lcd_st7789v[i].len & 0x80) {
            delay(120);
        }
    }
    tft.setRotation(2);
    tft.fillScreen(TFT_BLACK);
    pinMode(PIN_LCD_BL, OUTPUT);
    digitalWrite(PIN_LCD_BL, HIGH);
    tft.setSwapBytes(true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("OPEN TAG", LV_SCREEN_WIDTH/2, 20, 4);
    tft.drawCentreString("READER",LV_SCREEN_WIDTH/2,45,4);
    tft.drawCentreString("Initialising ...",LV_SCREEN_WIDTH/2,LV_SCREEN_HEIGHT/2,4);
    lv_begin();
}

/* Display rendering callback */
static void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h);
    lv_disp_flush_ready(disp);
}

/* Read input events callback */
static void lv_encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    EventGroupHandle_t *lv_input_event = (EventGroupHandle_t *)indev_drv->user_data;
    EventBits_t bit = xEventGroupGetBits(lv_input_event);
    data->state = LV_INDEV_STATE_RELEASED;
    if (bit & LV_BUTTON)
    {
        // Serial.println("Button pressed");
        xEventGroupClearBits(lv_input_event, LV_BUTTON);
        data->state = LV_INDEV_STATE_PR;
    }
    else if (bit & LV_ENCODER_CW)
    {
        // Serial.println("Encoder CW");
        xEventGroupClearBits(lv_input_event, LV_ENCODER_CW);
        data->enc_diff = 1;
    }
    else if (bit & LV_ENCODER_CCW)
    {
        // Serial.println("Encoder CCW");
        xEventGroupClearBits(lv_input_event, LV_ENCODER_CCW);
        data->enc_diff = -1;
    }
}

/* Read encoder direction */
static void encoder_read()
{
    RotaryEncoder::Direction dir = encoder.getDirection();
    if (dir != RotaryEncoder::Direction::NOROTATION)
    {
        if (dir != RotaryEncoder::Direction::CLOCKWISE)
        {
            xEventGroupSetBits(lv_input_event, LV_ENCODER_CW);
            // xEventGroupSetBits(lv_input_event, LV_ENCODER_LED_CW);
        }
        else
        {
            xEventGroupSetBits(lv_input_event, LV_ENCODER_CCW);
            // xEventGroupSetBits(lv_input_event, LV_ENCODER_LED_CCW);
        }
    }
}

void lv_begin()
{
    /* Initialize rotary encoder button*/
    lv_input_event = xEventGroupCreate();
    button.attachClick(
        [](void *param)
        {
            EventGroupHandle_t *lv_input_event = (EventGroupHandle_t *)param;
            xEventGroupSetBits(lv_input_event, LV_BUTTON);
            // xEventGroupSetBits(global_event_group, WAV_RING_1);
        },
        lv_input_event);

    lv_init();

    /* Initialize the display buffer */
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t *buf1, *buf2;
    buf1 = (lv_color_t *)heap_caps_malloc(LV_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = (lv_color_t *)heap_caps_malloc(LV_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    assert(buf2);
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, LV_BUF_SIZE);

    /* Initialize the display */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_SCREEN_WIDTH;
    disp_drv.ver_res = LV_SCREEN_HEIGHT;
    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the rotary encoder */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = lv_encoder_read;
    indev_drv.user_data = lv_input_event;

    /* Initialize the rotary input group */
    static lv_indev_t *lv_encoder_indev = lv_indev_drv_register(&indev_drv);
    lv_group_t *g = lv_group_create();
    lv_indev_set_group(lv_encoder_indev, g);
    lv_group_set_default(g);

    Serial.printf("Lvgl v%d.%d.%d initialized\n", lv_version_major(), lv_version_minor(), lv_version_patch());
}

void lv_handler()
/* Handles updating the display and encoder/button events */
{
    lv_task_handler();
    delay(5);
    button.tick();
    encoder.tick();
    encoder_read();
}

void displaySuccessfulScan(int& row) {
        tft.fillScreen(TFT_GREEN);
        delay(100);
        tft.fillScreen(TFT_BLACK);
        Serial.println(numTags);
        Serial.println(row);
        tft.drawCentreString(RFID, 85, 10, 2);
        tft.drawCentreString(tagNLISID[row], LV_SCREEN_WIDTH/2, 30, 2);
        tft.drawCentreString(String(tagVisual_ID[row]).substring(0,4), LV_SCREEN_WIDTH/2, 100, 4);
        tft.drawCentreString(String(tagVisual_ID[row]).substring(5,8), LV_SCREEN_WIDTH/2, 140, 8);
        tft.drawCentreString(tagColour[row], LV_SCREEN_WIDTH/2, 240, 4);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawCentreString(tagName[row], LV_SCREEN_WIDTH/2, 280, 4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        Serial.println(tagVisual_ID[row]);
        Serial.println(tagNLISID[row]);   
}