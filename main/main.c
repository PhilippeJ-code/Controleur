// Main.c
//

#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "esp_log.h"
#include "lvgl.h"

#include "hardware.h"
#include "languages.h"

LV_IMG_DECLARE(img_arrow_up);
LV_IMG_DECLARE(img_arrow_down);
LV_IMG_DECLARE(img_arrow_left);
LV_IMG_DECLARE(img_arrow_right);
LV_IMG_DECLARE(img_home);
LV_IMG_DECLARE(img_folder);
LV_IMG_DECLARE(img_file);
LV_IMG_DECLARE(img_folder_up);
LV_IMG_DECLARE(img_unlock);
LV_IMG_DECLARE(img_marker);
LV_IMG_DECLARE(img_focus);
LV_IMG_DECLARE(img_square);
LV_IMG_DECLARE(img_center);
LV_IMG_DECLARE(img_corner);
LV_IMG_DECLARE(img_execute);
LV_IMG_DECLARE(img_caret_down);
LV_IMG_DECLARE(img_arrow_bottom_left);
LV_IMG_DECLARE(img_arrow_bottom_right);
LV_IMG_DECLARE(img_arrow_top_left);
LV_IMG_DECLARE(img_arrow_top_right);

LV_FONT_DECLARE(my_lv_font_montserrat_18);

#include "dirent.h"

#define MIN_FOLDER_SIZE 8
#define MAX_FOLDER_SIZE 256
#define MAX_FILE_SIZE 256

static uint16_t language = 0;

char folderName[MAX_FOLDER_SIZE];
char fileName[MAX_FILE_SIZE];

static const char *TAG = "Controleur";

static lv_style_t image_style;

static float decodedFloat[4];
static char machineStatus[9];
static float wcoXYZA[4];
static float wposXYZA[4];
static float mposXYZA[4];
static float feedSpindle[2];
static float bufferAvailable[2];
static float overwritePercent[3];
static char lastMsg[80];

static int32_t readSize = 0;
static int32_t size = 0;

static lv_obj_t *objPosX;
static lv_obj_t *objPosY;
static lv_obj_t *objPosZ;
static lv_obj_t *state_label;
static lv_obj_t *status_label;
static lv_obj_t *alarm_label;
static lv_obj_t *folder_label;
static lv_obj_t *msg_box;
static lv_obj_t *err_msg_box;

static lv_obj_t *main_list;
static lv_obj_t *list_params;

lv_obj_t *labelSurfaceX = NULL;
lv_obj_t *labelSurfaceY = NULL;

static float surfaceX = 10.0;
static float surfaceY = 10.0;

#define MAX_PARAMS 50
#define PARAM_SIZE 20
static int memoIdxPar = 0;
static uint8_t idxPar = 0;
static char params[MAX_PARAMS][PARAM_SIZE];

static lv_obj_t *win = NULL;
static lv_obj_t *bar = NULL;
static lv_obj_t *barBuffer = NULL;
static lv_obj_t *label = NULL;
static lv_obj_t *labelFile = NULL;
static lv_obj_t *labelFileTools = NULL;
static lv_obj_t *labelBuffer = NULL;

static int speed = 1000;

static lv_style_t style_radio;
static lv_style_t style_radio_chk;
static uint32_t active_index = 2;
static float distance = 10;

static int scnSendFile = 0;
static FILE *f = NULL;

static uint16_t bufferSize = 0;
static uint32_t okReceived = 0;

static void eventErrorMessage(lv_event_t *e)
{
    lv_msgbox_close(err_msg_box);
}

void errorMessage(char *msg)
{
    static const char *btns[] = {strOk, ""};

    err_msg_box = lv_msgbox_create(NULL, strError, msg, btns, false);
    lv_obj_set_style_text_font(err_msg_box, &my_lv_font_montserrat_18, 0);
    lv_obj_add_event_cb(err_msg_box, eventErrorMessage, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(err_msg_box);
    lv_obj_set_size(err_msg_box, 350, 170);
}

void decodeFloat(char *pSection)
{ // decode up to 4 float numbers comma delimited in a section
    decodedFloat[0] = 0;
    decodedFloat[1] = 0;
    decodedFloat[2] = 0;
    decodedFloat[3] = 0;
    char *pEndNum;
    int i = 0;
    pEndNum = strpbrk(pSection, ",|>"); // cherche un dernier caractère valide après le nombre
    while ((i < 4) && pEndNum)
    { // decode max 4 floats
        decodedFloat[i] = atof(pSection);
        if (*pEndNum == ',')
        { // if last char is ',', then we loop
            pSection = pEndNum + 1;
            i++;
            pEndNum = strpbrk(pSection, ",|>"); // search first char that end the section and return the position of this end
        }
        else
        {
            i = 4; // force to exit while when the last char was not a comma
        }
    }
}

void parseStatusLine(char *line)
{

    char *pBegin;
    char *pEndType;

    if (line[strlen(line) - 1] != '>')
        return;

    pBegin = line + 1;
    pEndType = strchr(pBegin, '|');
    *pEndType = '\0';
    memccpy(machineStatus, pBegin, '\0', 9);
    pBegin = strchr(pBegin, '\0') + 1;
    char MPosOrWPos = ' ';
    while (true)
    {
        pEndType = strchr(pBegin, ':');
        if (pEndType)
        {
            decodeFloat(pEndType + 1);
            if (strncmp(pBegin, "MPos:", strlen("MPos:")) == 0)
            {
                for (uint8_t i = 0; i < 4; i++)
                {
                    mposXYZA[i] = decodedFloat[i];
                    wposXYZA[i] = mposXYZA[i] - wcoXYZA[i];
                    MPosOrWPos = 'M';
                }
            }
            else if (strncmp(pBegin, "WPos:", strlen("WPos:")) == 0)
            {
                for (uint8_t i = 0; i < 4; i++)
                {
                    wposXYZA[i] = decodedFloat[i];
                    mposXYZA[i] = wposXYZA[i] + wcoXYZA[i];
                    MPosOrWPos = 'W';
                }
            }
            else if (strncmp(pBegin, "WCO:", strlen("WCO:")) == 0)
            {
                for (uint8_t i = 0; i < 4; i++)
                {
                    wcoXYZA[i] = decodedFloat[i];
                    if (MPosOrWPos == 'W')
                    { // we previously had a WPos so we update MPos
                        mposXYZA[i] = wposXYZA[i] + wcoXYZA[i];
                    }
                    else
                    { // we previously had a MPos so we update WPos
                        wposXYZA[i] = mposXYZA[i] - wcoXYZA[i];
                    }
                }
            }
            else if (strncmp(pBegin, "Bf:", strlen("Bf:")) == 0)
            {
                bufferAvailable[0] = decodedFloat[0];
                bufferAvailable[1] = decodedFloat[1];
            }
            else if (strncmp(pBegin, "F:", strlen("F:")) == 0)
            {
                feedSpindle[0] = decodedFloat[0];
            }
            else if (strncmp(pBegin, "FS:", strlen("FS:")) == 0)
            {
                feedSpindle[0] = decodedFloat[0];
                feedSpindle[1] = decodedFloat[1];
            }
            else if (strncmp(pBegin, "Ov:", strlen("Ov:")) == 0)
            {
                overwritePercent[0] = decodedFloat[0];
                overwritePercent[1] = decodedFloat[1];
                overwritePercent[2] = decodedFloat[2];
            }

            pBegin = strchr(pBegin, '|');
            if (pBegin)
            {
                pBegin++;
            }
            else
            {
                break;
            }
        }
    }
}

void parseErrorLine(const char *line)
{
    int errorNum = atoi(&line[6]);
    int errorNumCorr;
    errorNumCorr = errorNum;
    if (errorNum < 1 || errorNum > 39)
        errorNumCorr = 0;
    memccpy(lastMsg, strErrors[errorNumCorr], '\0', 79);
}

void parseAlarmLine(const char *line)
{
    int alarmNum = atoi(&line[6]);
    if (alarmNum < 1 || alarmNum > 9)
        alarmNum = 0;
    memccpy(lastMsg, strAlarms[alarmNum], '\0', 79);
}

static void updateVals(lv_timer_t *timer)
{
    if (hardLvglLock(-1))
    {
        char str[32];
        sprintf(str, "X : %.1f", wposXYZA[0]);
        lv_label_set_text(objPosX, str);

        sprintf(str, "Y : %.1f", wposXYZA[1]);
        lv_label_set_text(objPosY, str);

        sprintf(str, "Z : %.1f", wposXYZA[2]);
        lv_label_set_text(objPosZ, str);

        if (size != 0)
        {
            sprintf(str, "%d%%", (int)(readSize * 100 / size));
            if (bar != NULL)
                lv_bar_set_value(bar, readSize * 100 / size, LV_ANIM_ON);
            if (label != NULL)
                lv_label_set_text(label, str);
            if (barBuffer != NULL)
                lv_bar_set_value(barBuffer, bufferSize * 100 / 128, LV_ANIM_ON);
        }

        if (memoIdxPar != idxPar)
        {
            while (memoIdxPar < idxPar)
            {
                lv_obj_t *list_txt = lv_list_add_text(list_params, params[memoIdxPar++]);
                lv_obj_set_style_text_font(list_txt, &my_lv_font_montserrat_18, 0);
            }
            memoIdxPar = idxPar;
        }
        else
        {
            if (usbIsConnected())
            {
                uint8_t data[] = "?";
                usbSend(data, strlen((char *)data));

                lv_label_set_text(state_label, strGrblConnected);
                lv_label_set_text(status_label, machineStatus);
                lv_label_set_text(alarm_label, lastMsg);
            }
            else
            {
                lv_label_set_text(state_label, strGrblDisconnected);
                lv_label_set_text(status_label, "");
                lv_label_set_text(alarm_label, "");
            }
        }

        char path[MAX_FILE_SIZE];
        if ((strlen(folderName) + strlen(fileName)) >= MAX_FILE_SIZE)
        {
            strcpy(path, strFilenameTooLong);
        }
        else
        {
            strcpy(path, folderName);
            strcat(path, fileName);
        }
        lv_label_set_text(labelFileTools, path);

        hardLvglUnlock();
    }
}

static void read_usb(lv_timer_t *timer)
{
    uint8_t datas[128];

    while (1)
    {
        if (usbReceive(datas, sizeof(datas)) == true)
        {
            if (datas[0] == '<')
            {
                parseStatusLine((char *)datas);
            }
            else if (strncmp((char *)datas, "error:", 6) == 0)
            {
                parseErrorLine((char *)datas);
            }
            else if (strncmp((char *)datas, "ALARM:", 6) == 0)
            {
                parseAlarmLine((char *)datas);
            }
            else if (datas[0] == '$')
            {
                if (hardLvglLock(-1))
                {
                    strncpy(params[idxPar++], (char *)datas, PARAM_SIZE);
                    hardLvglUnlock();
                }
            }
        }
        vTaskDelay(1);
    }
}

static void event_cancel(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        scnSendFile = 3;
    }
}

static void send_file(void *arg)
{

    static uint16_t idxLct = 0;
    static uint16_t idxEcr = 0;
    static uint16_t sendLengths[64];

    static uint8_t carusb[256];
    static uint16_t idxusb = 0;

    static bool bEnd = true;

    while (1)
    {
        if (scnSendFile == 1)
        {
            bEnd = true;

            if ((strlen(folderName) + strlen(fileName)) >= MAX_FILE_SIZE)
            {
                errorMessage(strFilenameTooLong);
                scnSendFile = 0;
                return;
            }

            if (!sdCardPresent())
            {
                errorMessage(strNoSDReader);
                scnSendFile = 0;
                return;
            }

            if (!sdCardMount())
            {
                errorMessage(strNoSDCard);
                scnSendFile = 0;
                return;
            }

            char path[MAX_FILE_SIZE];
            strcpy(path, folderName);
            strcat(path, fileName);

            f = fopen(path, "r");
            if (f == NULL)
            {
                sdCardUnmount();
                errorMessage(strFileReadError);
                scnSendFile = 0;
                return;
            }

            readSize = 0;
            fseek(f, 0, SEEK_END);
            size = ftell(f);
            fseek(f, 0, SEEK_SET);

            if (hardLvglLock(-1))
            {
                win = lv_win_create(lv_scr_act(), 40);
                lv_obj_set_height(win, 350);

                lv_win_add_title(win, strEngraving);

                lv_obj_t *cont = lv_win_get_content(win);

                labelFile = lv_label_create(cont);
                lv_label_set_text(labelFile, path);
                lv_obj_align(labelFile, LV_ALIGN_TOP_MID, 0, 0);
                lv_obj_set_style_text_color(labelFile, lv_palette_darken(LV_PALETTE_GREY, 3), 0);

                bar = lv_bar_create(cont);
                lv_obj_set_size(bar, 300, 40);
                lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 50);
                lv_bar_set_value(bar, 0, LV_ANIM_OFF);

                label = lv_label_create(bar);
                lv_label_set_text(label, "0%");
                lv_obj_center(label);
                lv_obj_set_style_text_color(label, lv_palette_darken(LV_PALETTE_GREY, 3), 0);

                barBuffer = lv_bar_create(cont);
                lv_obj_set_size(barBuffer, 120, 35);
                lv_obj_align(barBuffer, LV_ALIGN_TOP_MID, 0, 100);
                lv_bar_set_value(barBuffer, 0, LV_ANIM_OFF);

                labelBuffer = lv_label_create(barBuffer);
                lv_label_set_text(labelBuffer, strBuffer);
                lv_obj_center(labelBuffer);
                lv_obj_set_style_text_color(labelBuffer, lv_palette_darken(LV_PALETTE_GREY, 3), 0);

                lv_obj_t *btn_cancel = lv_btn_create(cont);
                lv_obj_add_event_cb(btn_cancel, event_cancel, LV_EVENT_ALL, NULL);
                lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_MID, 0, 0);

                lv_obj_t *labelCancel = lv_label_create(btn_cancel);
                lv_label_set_text(labelCancel, strCancel);
                lv_obj_center(labelCancel);
                hardLvglUnlock();
            }

            usbSetNbrOk(0);
            scnSendFile = 2;
            idxLct = idxEcr = 0;
            okReceived = 0;
            bufferSize = 0;

            uint8_t start[] = "G90\n";
            sendLengths[idxEcr++] = strlen((char *)start);
            bufferSize += strlen((char *)start);
            usbSend(start, strlen((char *)start));
        }
        else if (scnSendFile == 2)
        {
            uint32_t oks = usbGetNbrOk();
            while (okReceived < oks)
            {
                bufferSize -= sendLengths[idxLct++];
                if (idxLct == 64)
                    idxLct = 0;
                okReceived++;
            }

            if ((idxusb > 0) && ((bufferSize + idxusb) < 128))
            {
                sendLengths[idxEcr++] = idxusb;
                if (idxEcr == 64)
                    idxEcr = 0;
                bufferSize += idxusb;
                usbSend(carusb, idxusb);
                idxusb = 0;
            }

            if (idxusb == 0)
            {
                while (true)
                {
                    if (readSize < size)
                    {
                        uint8_t car = fgetc(f);
                        readSize++;
                        if (car != '\r' && car != ' ')
                        {
                            carusb[idxusb++] = car;
                        }

                        if (car == '\n')
                            break;
                    }
                    else
                        break;
                }
            }

            if ((readSize >= size) && (bEnd))
            {
                strcpy((char *)carusb, "G0X0Y0Z0\n");
                idxusb = 9;
                bEnd = false;
            }

            if ((readSize >= size) && (bufferSize == 0))
            {

                fclose(f);
                sdCardUnmount();

                if (hardLvglLock(-1))
                {
                    lv_obj_del(win);

                    bar = NULL;
                    barBuffer = NULL;
                    label = NULL;

                    hardLvglUnlock();
                }

                scnSendFile = 0;
            }
        }
        else if (scnSendFile == 3)
        {
            fclose(f);
            sdCardUnmount();

            if (hardLvglLock(-1))
            {
                lv_obj_del(win);

                bar = NULL;
                barBuffer = NULL;
                label = NULL;

                hardLvglUnlock();
            }
            scnSendFile = 0;
        }
        vTaskDelay(1);
    }
}

static lv_obj_t *tv;
static lv_style_t style_text_muted;

static void color_changer_anim_cb(void *var, int32_t v)
{
    lv_obj_t *obj = var;
    lv_coord_t max_w = lv_obj_get_width(lv_obj_get_parent(obj)) - LV_DPX(20);
    lv_coord_t w;

    w = lv_map(v, 0, 256, LV_DPX(60), max_w);
    lv_obj_set_width(obj, w);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(10), -LV_DPX(10));

    if (v > LV_OPA_COVER)
        v = LV_OPA_COVER;

    uint32_t i;
    for (i = 0; i < lv_obj_get_child_cnt(obj); i++)
    {
        lv_obj_set_style_opa(lv_obj_get_child(obj, i), v, 0);
    }
}

static void color_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_FOCUSED)
    {
        lv_obj_t *color_cont = lv_obj_get_parent(obj);
        if (lv_obj_get_width(color_cont) < LV_HOR_RES / 2)
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
    }
    else if (code == LV_EVENT_CLICKED)
    {
        lv_palette_t *palette_primary = lv_event_get_user_data(e);
        lv_palette_t palette_secondary = (*palette_primary) + 3; /*Use another palette as secondary*/
        if (palette_secondary >= _LV_PALETTE_LAST)
            palette_secondary = 0;
#if LV_USE_THEME_DEFAULT
        lv_theme_default_init(NULL, lv_palette_main(*palette_primary), lv_palette_main(palette_secondary),
                              LV_THEME_DEFAULT_DARK, &my_lv_font_montserrat_18);
#endif
        // lv_color_t color = lv_palette_main(*palette_primary);
        //  lv_style_set_text_color(&style_icon, color);
    }
}

static void color_changer_event_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        lv_obj_t *color_cont = lv_event_get_user_data(e);
        if (lv_obj_get_width(color_cont) < LV_HOR_RES / 2)
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
        else
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 256, 0);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
    }
}

static void color_changer_create(lv_obj_t *parent)
{
    static lv_palette_t palette[] = {
        LV_PALETTE_BLUE, LV_PALETTE_GREEN, LV_PALETTE_BLUE_GREY, LV_PALETTE_ORANGE,
        LV_PALETTE_RED, LV_PALETTE_PURPLE, LV_PALETTE_TEAL, _LV_PALETTE_LAST};

    lv_obj_t *color_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(color_cont);
    lv_obj_set_flex_flow(color_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(color_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(color_cont, LV_OBJ_FLAG_FLOATING);

    lv_obj_set_style_bg_color(color_cont, lv_color_white(), 0);
    lv_obj_set_style_pad_right(color_cont, LV_DPX(55), 0);
    lv_obj_set_style_bg_opa(color_cont, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(color_cont, LV_RADIUS_CIRCLE, 0);

    lv_obj_set_size(color_cont, LV_DPX(60), LV_DPX(60));

    lv_obj_align(color_cont, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(10), -LV_DPX(10));

    uint32_t i;
    for (i = 0; palette[i] != _LV_PALETTE_LAST; i++)
    {
        lv_obj_t *c = lv_btn_create(color_cont);
        lv_obj_set_style_bg_color(c, lv_palette_main(palette[i]), 0);
        lv_obj_set_style_radius(c, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_opa(c, LV_OPA_TRANSP, 0);
        lv_obj_set_size(c, 20, 20);
        lv_obj_add_event_cb(c, color_event_cb, LV_EVENT_ALL, &palette[i]);
        lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    }

    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(btn, 10, 0);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(btn, color_changer_event_cb, LV_EVENT_ALL, color_cont);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_text_font(btn, &lv_font_montserrat_18, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_TINT, 0);

    lv_obj_set_size(btn, LV_DPX(50), LV_DPX(50));
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(15), -LV_DPX(15));
}

static void event_up(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[48];
        sprintf(data, "$J=G91Y%.2fF%d\n", distance, speed);
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_down(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[48];
        sprintf(data, "$J=G91Y-%.2fF%d\n", distance, speed);
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_left(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[48];
        sprintf(data, "$J=G91X-%.2fF%d\n", distance, speed);
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_right(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[48];
        sprintf(data, "$J=G91X%.2fF%d\n", distance, speed);
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_bottom_left(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[48];
        sprintf(data, "$J=G91X-%.2fY-%.2fF%d\n", distance, distance, speed);
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_bottom_right(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[48];
        sprintf(data, "$J=G91X%.2fY-%.2fF%d\n", distance, distance, speed);
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_top_left(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[48];
        sprintf(data, "$J=G91X-%.2fY%.2fF%d\n", distance, distance, speed);
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_top_right(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[48];
        sprintf(data, "$J=G91X%.2fY%.2fF%d\n", distance, distance, speed);
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_home(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[] = "$H\n";
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_unlock(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        lastMsg[0] = 0;
        char data[] = "$X\n";
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void event_marker(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char data[] = "G92X0Y0Z0\n";
        usbSend((uint8_t *)data, strlen(data));
    }
}

static void speed_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_REFR_EXT_DRAW_SIZE)
    {
        lv_coord_t *s = lv_event_get_param(e);
        *s = LV_MAX(*s, 60);
    }
    else if (code == LV_EVENT_VALUE_CHANGED)
    {
        speed = (int)lv_slider_get_value(obj);
    }
    else if (code == LV_EVENT_DRAW_PART_END)
    {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_param(e);
        if (dsc->part == LV_PART_KNOB && lv_obj_has_state(obj, LV_STATE_PRESSED))
        {
            char buf[8];
            lv_snprintf(buf, sizeof(buf), "%" LV_PRId32, lv_slider_get_value(obj));

            lv_point_t text_size;
            lv_txt_get_size(&text_size, buf, &my_lv_font_montserrat_18, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            lv_area_t txt_area;
            txt_area.x1 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2 - text_size.x / 2;
            txt_area.x2 = txt_area.x1 + text_size.x;
            txt_area.y2 = dsc->draw_area->y1 - 10;
            txt_area.y1 = txt_area.y2 - text_size.y;

            lv_area_t bg_area;
            bg_area.x1 = txt_area.x1 - LV_DPX(8);
            bg_area.x2 = txt_area.x2 + LV_DPX(8);
            bg_area.y1 = txt_area.y1 - LV_DPX(8);
            bg_area.y2 = txt_area.y2 + LV_DPX(8);

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.bg_color = lv_palette_darken(LV_PALETTE_GREY, 3);
            rect_dsc.radius = LV_DPX(5);
            lv_draw_rect(dsc->draw_ctx, &rect_dsc, &bg_area);

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = lv_color_white();
            label_dsc.font = &my_lv_font_montserrat_18;
            lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
        }
    }
}

static void radio_event_handler(lv_event_t *e)
{
    uint32_t *active_id = lv_event_get_user_data(e);
    lv_obj_t *cont = lv_event_get_current_target(e);
    lv_obj_t *act_cb = lv_event_get_target(e);
    lv_obj_t *old_cb = lv_obj_get_child(cont, *active_id);

    if (act_cb == cont)
        return;

    lv_obj_clear_state(old_cb, LV_STATE_CHECKED); /*Uncheck the previous radio button*/
    lv_obj_add_state(act_cb, LV_STATE_CHECKED);   /*Uncheck the current radio button*/

    *active_id = lv_obj_get_index(act_cb);
    distance = 0.1;
    for (int i = 0; i < *active_id; i++)
    {
        distance *= 10;
    }

}

static void radiobutton_create(lv_obj_t *parent, const char *txt)
{
    lv_obj_t *obj = lv_checkbox_create(parent);
    lv_checkbox_set_text(obj, txt);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_style(obj, &style_radio, LV_PART_INDICATOR);
    lv_obj_add_style(obj, &style_radio_chk, LV_PART_INDICATOR | LV_STATE_CHECKED);
}

void CreateJoystick(lv_obj_t *parent)
{
    // Jog
    //
    lv_obj_t *panelJog = lv_obj_create(parent);
    lv_obj_align(panelJog, LV_ALIGN_TOP_LEFT, -10, -10);

    lv_obj_t *btn_up = lv_btn_create(panelJog);
    lv_obj_add_event_cb(btn_up, event_up, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_up, 64);
    lv_obj_set_width(btn_up, 64);
    lv_obj_align(btn_up, LV_ALIGN_TOP_LEFT, 64 + 8, 0);

    lv_obj_t *img_up = lv_img_create(btn_up);
    lv_img_set_src(img_up, &img_arrow_up);
    lv_obj_center(img_up);
    lv_obj_add_style(img_up, &image_style, 0);

    lv_obj_t *btn_down = lv_btn_create(panelJog);
    lv_obj_add_event_cb(btn_down, event_down, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_down, 64);
    lv_obj_set_width(btn_down, 64);
    lv_obj_align(btn_down, LV_ALIGN_TOP_LEFT, 64 + 8, 64 + 8 + 64 + 8);

    lv_obj_t *img_down = lv_img_create(btn_down);
    lv_img_set_src(img_down, &img_arrow_down);
    lv_obj_center(img_down);
    lv_obj_add_style(img_down, &image_style, 0);

    lv_obj_t *btn_left = lv_btn_create(panelJog);
    lv_obj_add_event_cb(btn_left, event_left, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_left, 64);
    lv_obj_set_width(btn_left, 64);
    lv_obj_align(btn_left, LV_ALIGN_TOP_LEFT, 0, 64 + 8);

    lv_obj_t *img_left = lv_img_create(btn_left);
    lv_img_set_src(img_left, &img_arrow_left);
    lv_obj_center(img_left);
    lv_obj_add_style(img_left, &image_style, 0);

    lv_obj_t *btn_right = lv_btn_create(panelJog);
    lv_obj_add_event_cb(btn_right, event_right, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_right, 64);
    lv_obj_set_width(btn_right, 64);
    lv_obj_align(btn_right, LV_ALIGN_TOP_LEFT, 64 + 8 + 64 + 8, 64 + 8);

    lv_obj_t *img_right = lv_img_create(btn_right);
    lv_img_set_src(img_right, &img_arrow_right);
    lv_obj_center(img_right);
    lv_obj_add_style(img_right, &image_style, 0);

    lv_obj_t *btn_bottom_left = lv_btn_create(panelJog);
    lv_obj_add_event_cb(btn_bottom_left, event_bottom_left, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_bottom_left, 64);
    lv_obj_set_width(btn_bottom_left, 64);
    lv_obj_align(btn_bottom_left, LV_ALIGN_TOP_LEFT, 0, 64 + 8 + 64 + 8);

    lv_obj_t *img_bottom_left = lv_img_create(btn_bottom_left);
    lv_img_set_src(img_bottom_left, &img_arrow_bottom_left);
    lv_obj_center(img_bottom_left);
    lv_obj_add_style(img_bottom_left, &image_style, 0);

    lv_obj_t *btn_bottom_right = lv_btn_create(panelJog);
    lv_obj_add_event_cb(btn_bottom_right, event_bottom_right, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_bottom_right, 64);
    lv_obj_set_width(btn_bottom_right, 64);
    lv_obj_align(btn_bottom_right, LV_ALIGN_TOP_LEFT, 64 + 8 + 64 + 8, 64 + 8 + 64 + 8);

    lv_obj_t *img_bottom_right = lv_img_create(btn_bottom_right);
    lv_img_set_src(img_bottom_right, &img_arrow_bottom_right);
    lv_obj_center(img_bottom_right);
    lv_obj_add_style(img_bottom_right, &image_style, 0);

    lv_obj_t *btn_top_left = lv_btn_create(panelJog);
    lv_obj_add_event_cb(btn_top_left, event_top_left, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_top_left, 64);
    lv_obj_set_width(btn_top_left, 64);
    lv_obj_align(btn_top_left, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *img_top_left = lv_img_create(btn_top_left);
    lv_img_set_src(img_top_left, &img_arrow_top_left);
    lv_obj_center(img_top_left);
    lv_obj_add_style(img_top_left, &image_style, 0);

    lv_obj_t *btn_top_right = lv_btn_create(panelJog);
    lv_obj_add_event_cb(btn_top_right, event_top_right, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_top_right, 64);
    lv_obj_set_width(btn_top_right, 64);
    lv_obj_align(btn_top_right, LV_ALIGN_TOP_LEFT, 64 + 8 + 64 + 8, 0);

    lv_obj_t *img_top_right = lv_img_create(btn_top_right);
    lv_img_set_src(img_top_right, &img_arrow_top_right);
    lv_obj_center(img_top_right);
    lv_obj_add_style(img_top_right, &image_style, 0);

    lv_obj_t *btn_home = lv_btn_create(panelJog);
    lv_obj_add_event_cb(btn_home, event_home, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_home, 64);
    lv_obj_set_width(btn_home, 64);
    lv_obj_align(btn_home, LV_ALIGN_TOP_LEFT, 64 + 8, 64 + 8);

    lv_obj_t *image_home = lv_img_create(btn_home);
    lv_img_set_src(image_home, &img_home);
    lv_obj_center(image_home);
    lv_obj_add_style(image_home, &image_style, 0);

    lv_obj_set_height(panelJog, LV_SIZE_CONTENT);
    lv_obj_set_width(panelJog, LV_SIZE_CONTENT);

    // Paramètres
    //
    lv_obj_t *panelParams = lv_obj_create(parent);
    lv_obj_align(panelParams, LV_ALIGN_TOP_LEFT, 374, -10);

    lv_obj_t *speed_label = lv_label_create(panelParams);
    lv_label_set_text(speed_label, strSpeed);
    lv_obj_add_style(speed_label, &style_text_muted, 0);
    lv_obj_align(speed_label, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *speed_slider = lv_slider_create(panelParams);
    lv_obj_set_width(speed_slider, 200);
    lv_slider_set_range(speed_slider, 10, 2000);
    lv_slider_set_value(speed_slider, speed, LV_ANIM_OFF);
    lv_obj_add_event_cb(speed_slider, speed_event, LV_EVENT_ALL, NULL);
    lv_obj_refresh_ext_draw_size(speed_slider);
    lv_obj_align(speed_slider, LV_ALIGN_TOP_LEFT, 0, 35);

    lv_obj_t *distance_label = lv_label_create(panelParams);
    lv_label_set_text(distance_label, strDistance);
    lv_obj_add_style(distance_label, &style_text_muted, 0);
    lv_obj_align(distance_label, LV_ALIGN_TOP_LEFT, 240, 0);

    uint32_t i;
    char buf[32];

    lv_obj_t *cont1 = lv_obj_create(panelParams);
    lv_obj_set_flex_flow(cont1, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(cont1, radio_event_handler, LV_EVENT_CLICKED, &active_index);
    lv_obj_align(cont1, LV_ALIGN_TOP_LEFT, 240, 35);

    lv_obj_set_height(cont1, LV_SIZE_CONTENT);
    lv_obj_set_width(cont1, LV_SIZE_CONTENT);

    float f = 0.1;
    for (i = 0; i < 4; i++)
    {
        if (i == 0)
            snprintf(buf, sizeof(buf), "%.1f", f);
        else
            snprintf(buf, sizeof(buf), "%.0f", f);
        radiobutton_create(cont1, buf);
        f *= 10;
    }
    lv_obj_add_state(lv_obj_get_child(cont1, active_index), LV_STATE_CHECKED);

    lv_obj_set_height(panelParams, LV_SIZE_CONTENT);
    lv_obj_set_width(panelParams, LV_SIZE_CONTENT);

    // Tools
    //
    lv_obj_t *panelTools = lv_obj_create(parent);
    lv_obj_align(panelTools, LV_ALIGN_TOP_LEFT, 254, -10);

    lv_obj_t *btn_unlock = lv_btn_create(panelTools);
    lv_obj_add_event_cb(btn_unlock, event_unlock, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_unlock, 64);
    lv_obj_set_width(btn_unlock, 64);
    lv_obj_align(btn_unlock, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *image_unlock = lv_img_create(btn_unlock);
    lv_img_set_src(image_unlock, &img_unlock);
    lv_obj_center(image_unlock);
    lv_obj_add_style(image_unlock, &image_style, 0);

    lv_obj_t *btn_marker = lv_btn_create(panelTools);
    lv_obj_add_event_cb(btn_marker, event_marker, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_marker, 64);
    lv_obj_set_width(btn_marker, 64);
    lv_obj_align(btn_marker, LV_ALIGN_TOP_LEFT, 0, 64 + 8);

    lv_obj_t *image_marker = lv_img_create(btn_marker);
    lv_img_set_src(image_marker, &img_marker);
    lv_obj_center(image_marker);
    lv_obj_add_style(image_marker, &image_style, 0);

    lv_obj_set_height(panelTools, LV_SIZE_CONTENT);
    lv_obj_set_width(panelTools, LV_SIZE_CONTENT);
}

static void event_folder(lv_event_t *e);
static void event_file(lv_event_t *e);

static void populate_list(const char *path)
{

    lv_obj_clean(main_list);

    if (sdCardPresent() == false)
    {
        lv_label_set_text(folder_label, strNoSDReader);
    }
    else
    {
        if (!sdCardMount())
        {
            lv_label_set_text(folder_label, strNoSDCard);
            return;
        }
    }

    lv_label_set_text(folder_label, path);

    bool isDir;
    struct dirent *entry;

    DIR *dir = opendir(path);
    while ((entry = readdir(dir)) != NULL)
    {
        isDir = (entry->d_type == DT_DIR ? true : false);
        if (strcmp(entry->d_name, "System Volume Information"))
        {
            lv_obj_t *list_btn = lv_list_add_btn(main_list, (isDir ? &img_folder : &img_file), entry->d_name);

            if (isDir)
            {
                lv_obj_add_event_cb(list_btn, event_folder, LV_EVENT_ALL, NULL);
            }
            else
            {
                lv_obj_add_event_cb(list_btn, event_file, LV_EVENT_ALL, NULL);
            }
            lv_obj_set_style_text_font(list_btn, &my_lv_font_montserrat_18, 0);
        }
    }
    closedir(dir);
    sdCardUnmount();
}

static void event_folder(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {

        char *texte = lv_list_get_btn_text(main_list, obj);
        if ((strlen(texte) + strlen(folderName) + 1) < MAX_FOLDER_SIZE)
        {
            strcat(folderName, texte);
            strcat(folderName, "/");
            populate_list(folderName);
        }
    }
}

static void event_file_msgbox(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_current_target(e);

    if (!strcmp(lv_msgbox_get_active_btn_text(obj), strEngrave))
    {
        scnSendFile = 1;
    }
    else if (!strcmp(lv_msgbox_get_active_btn_text(obj), strMeasure))
    {
        if ((strlen(folderName) + strlen(fileName)) >= MAX_FILE_SIZE)
        {
            lv_msgbox_close(msg_box);
            errorMessage(strFilenameTooLong);
            return;
        }

        if (sdCardPresent() == false)
        {
            lv_msgbox_close(msg_box);
            errorMessage(strNoSDReader);
            return;
        }
        else
        {
            if (!sdCardMount())
            {
                lv_msgbox_close(msg_box);
                errorMessage(strNoSDCard);
                return;
            }
        }

        char path[MAX_FILE_SIZE];
        strcpy(path, folderName);
        strcat(path, fileName);

        FILE *f = fopen(path, "r");
        if (f == NULL)
        {
            sdCardUnmount();
            lv_msgbox_close(msg_box);
            errorMessage(strFileReadError);
            return;
        }

        surfaceX = 0;
        surfaceY = 0;
        char line[128];
        while (fgets(line, sizeof(line), f) != NULL)
        {
            char *car = line;
            while (*car != 0)
            {
                if (*car == 'X')
                {
                    car++;
                    float x = atof(car);
                    if (x > surfaceX)
                        surfaceX = x;
                }
                else if (*car == 'Y')
                {
                    car++;
                    float y = atof(car);
                    if (y > surfaceY)
                        surfaceY = y;
                }
                car++;
            }
        }
        fclose(f);

        sdCardUnmount();

        char str[32];
        sprintf(str, "X : %.1f", surfaceX);
        lv_label_set_text(labelSurfaceX, str);
        sprintf(str, "Y : %.1f", surfaceY);
        lv_label_set_text(labelSurfaceY, str);
    }
    lv_msgbox_close(msg_box);
}

static void event_file(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        char *texte = lv_list_get_btn_text(main_list, obj);
        strcpy(fileName, texte);
        static const char *btns[] = {strEngrave, strMeasure, strCancel, ""};

        msg_box = lv_msgbox_create(NULL, "Utilisation ?", texte, btns, false);
        lv_obj_set_style_text_font(msg_box, &my_lv_font_montserrat_18, 0);
        lv_obj_add_event_cb(msg_box, event_file_msgbox, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_center(msg_box);
        lv_obj_set_size(msg_box, 300, 170);
    }
}

static void event_folder_up(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        int len = strlen(folderName);
        if (len > MIN_FOLDER_SIZE)
        {
            len--;
            while ((len > MIN_FOLDER_SIZE) && (folderName[len - 1] != '/'))
            {
                len--;
                folderName[len] = 0;
            }
        }
        populate_list(folderName);
    }
}

void CreateFileExplorer(lv_obj_t *parent)
{
    lv_obj_t *btn_folder_up = lv_btn_create(parent);
    lv_obj_add_event_cb(btn_folder_up, event_folder_up, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_folder_up, 64);
    lv_obj_set_width(btn_folder_up, 64);
    lv_obj_align(btn_folder_up, LV_ALIGN_TOP_LEFT, 0 - 5, 0 - 5);

    lv_obj_t *image_folder_up = lv_img_create(btn_folder_up);
    lv_img_set_src(image_folder_up, &img_folder_up);
    lv_obj_center(image_folder_up);
    lv_obj_add_style(image_folder_up, &image_style, 0);

    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_align(panel, LV_ALIGN_TOP_LEFT, 80 - 5, 0 - 5);
    lv_obj_set_width(panel, 600);

    folder_label = lv_label_create(panel);
    lv_label_set_text(folder_label, "spiffs");
    lv_obj_align(folder_label, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_set_height(panel, LV_SIZE_CONTENT);

    main_list = lv_list_create(parent);
    lv_obj_set_size(main_list, 600, 190);
    lv_obj_align(main_list, LV_ALIGN_TOP_LEFT, 80 - 5, 75 - 5);

    strcpy(folderName, "/sdcard/");

    populate_list(folderName);
}

static void event_read_params(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        memoIdxPar = idxPar = 0;
        lv_obj_clean(list_params);
        uint8_t data[] = "$$\n";
        usbSend(data, strlen((char *)data));
    }
}

void CreateParameters(lv_obj_t *parent)
{
    lv_obj_t *btn_read_params = lv_btn_create(parent);
    lv_obj_add_event_cb(btn_read_params, event_read_params, LV_EVENT_ALL, NULL);
    lv_obj_align(btn_read_params, LV_ALIGN_TOP_LEFT, 0 - 5, 0 - 5);

    lv_obj_t *labelReadParams = lv_label_create(btn_read_params);
    lv_label_set_text(labelReadParams, strRead);
    lv_obj_center(labelReadParams);

    list_params = lv_list_create(parent);
    lv_obj_set_size(list_params, 440, 262);
    lv_obj_align(list_params, LV_ALIGN_TOP_LEFT, 95 - 5, 0 - 5);
}

static void event_focus(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    static bool bFocus = false;

    if (code == LV_EVENT_CLICKED)
    {
        if (!bFocus)
        {
            usbSend((uint8_t *)"M3S30.000\nG1F1000\n", 18);
            bFocus = true;
        }
        else
        {
            usbSend((uint8_t *)"M5S0\nG0\n", 8);
            bFocus = false;
        }
    }
}

static void event_square(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char str[128];
        sprintf(str, "M3S30.000\nG1F1000\nX0Y0\nX%.1f\nY%.1f\nX0\nY0\nM5S0\nG0\n", surfaceX, surfaceY);
        usbSend((uint8_t *)str, strlen(str));
    }
}

static void event_center(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char str[128];
        sprintf(str, "M3S30.000\nG1F1000\nX%.1fY%.1f\nM5S0\nG0\n", surfaceX / 2, surfaceY / 2);
        usbSend((uint8_t *)str, strlen(str));
    }
}

static void event_corner(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        char str[128];
        sprintf(str, "M3S30.000\nG1F1000\nX0Y0\nM5S0\nG0\n");
        usbSend((uint8_t *)str, strlen(str));
    }
}

static void event_execute(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        scnSendFile = 1;
    }
}

static void event_language(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        if ( language != lv_dropdown_get_selected(obj))
           nvsWrite("language", lv_dropdown_get_selected(obj));
    }
}

void CreateTools(lv_obj_t *parent)
{

    char str[32];

    lv_obj_t *btn_focus = lv_btn_create(parent);
    lv_obj_add_event_cb(btn_focus, event_focus, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_focus, 64);
    lv_obj_set_width(btn_focus, 64);
    lv_obj_align(btn_focus, LV_ALIGN_TOP_LEFT, 0 - 5, 0 - 5);

    lv_obj_t *image_focus = lv_img_create(btn_focus);
    lv_img_set_src(image_focus, &img_focus);
    lv_obj_center(image_focus);
    lv_obj_add_style(image_focus, &image_style, 0);

    lv_obj_t *btn_square = lv_btn_create(parent);
    lv_obj_add_event_cb(btn_square, event_square, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_square, 64);
    lv_obj_set_width(btn_square, 64);
    lv_obj_align(btn_square, LV_ALIGN_TOP_LEFT, 0 - 5, 64 + 8 - 5);

    lv_obj_t *image_square = lv_img_create(btn_square);
    lv_img_set_src(image_square, &img_square);
    lv_obj_center(image_square);
    lv_obj_add_style(image_square, &image_style, 0);

    lv_obj_t *btn_center = lv_btn_create(parent);
    lv_obj_add_event_cb(btn_center, event_center, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_center, 64);
    lv_obj_set_width(btn_center, 64);
    lv_obj_align(btn_center, LV_ALIGN_TOP_LEFT, 64 + 8 - 5, 0 - 5);

    lv_obj_t *btn_corner = lv_btn_create(parent);
    lv_obj_add_event_cb(btn_corner, event_corner, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_corner, 64);
    lv_obj_set_width(btn_corner, 64);
    lv_obj_align(btn_corner, LV_ALIGN_TOP_LEFT, 64 + 8 - 5, 64 + 8 - 5);

    lv_obj_t *image_corner = lv_img_create(btn_corner);
    lv_img_set_src(image_corner, &img_corner);
    lv_obj_center(image_corner);
    lv_obj_add_style(image_corner, &image_style, 0);

    lv_obj_t *image_center = lv_img_create(btn_center);
    lv_img_set_src(image_center, &img_center);
    lv_obj_center(image_center);
    lv_obj_add_style(image_center, &image_style, 0);

    lv_obj_t *btn_execute = lv_btn_create(parent);
    lv_obj_add_event_cb(btn_execute, event_execute, LV_EVENT_ALL, NULL);
    lv_obj_set_height(btn_execute, 64);
    lv_obj_set_width(btn_execute, 64);
    lv_obj_align(btn_execute, LV_ALIGN_TOP_LEFT, 0 - 5, 64 + 8 + 64 + 8- 5);

    lv_obj_t *image_execute = lv_img_create(btn_execute);
    lv_img_set_src(image_execute, &img_execute);
    lv_obj_center(image_execute);
    lv_obj_add_style(image_execute, &image_style, 0);

    labelFileTools = lv_label_create(parent);
    lv_obj_align(labelFileTools, LV_ALIGN_TOP_LEFT, 170 - 5, 0 - 5);
    lv_obj_set_style_text_color(labelFileTools, lv_palette_darken(LV_PALETTE_GREY, 3), 0);

    lv_obj_t *panelMax = lv_obj_create(parent);
    lv_obj_align(panelMax, LV_ALIGN_TOP_LEFT, 170 - 5, 40 - 5);

    lv_obj_t *size_label = lv_label_create(panelMax);
    lv_label_set_text(size_label, strSize);
    lv_obj_add_style(size_label, &style_text_muted, 0);
    lv_obj_align(size_label, LV_ALIGN_TOP_LEFT, 0, 0);

    labelSurfaceX = lv_label_create(panelMax);
    sprintf(str, "X : %.1f", surfaceX);
    lv_label_set_text(labelSurfaceX, str);
    lv_obj_align(labelSurfaceX, LV_ALIGN_TOP_LEFT, 0, 35);

    labelSurfaceY = lv_label_create(panelMax);
    sprintf(str, "Y : %.1f", surfaceY);
    lv_label_set_text(labelSurfaceY, str);
    lv_obj_align(labelSurfaceY, LV_ALIGN_TOP_LEFT, 0, 75);

    lv_obj_set_height(panelMax, LV_SIZE_CONTENT);
    lv_obj_set_width(panelMax, LV_SIZE_CONTENT);

    lv_obj_t * dd = lv_dropdown_create(parent);
    
    lv_dropdown_set_options(dd, "Français\n"
                                "English");
    lv_dropdown_set_selected(dd, language);
    lv_dropdown_set_symbol(dd, &img_caret_down);
    lv_obj_set_style_transform_angle(dd, 1800, LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_align(dd, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_event_cb(dd, event_language, LV_EVENT_ALL, NULL);

}

void CreatePanels()
{
    folderName[0] = fileName[0] = 0;

    lv_obj_set_style_text_font(lv_scr_act(), &my_lv_font_montserrat_18, 0);

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_70);

    lv_style_init(&image_style);
    lv_style_set_img_recolor_opa(&image_style, LV_OPA_COVER);
    lv_style_set_img_recolor(&image_style, lv_palette_lighten(LV_PALETTE_GREY, 4));

    lv_style_init(&style_radio);
    lv_style_set_radius(&style_radio, LV_RADIUS_CIRCLE);

    lv_style_init(&style_radio_chk);
    lv_style_set_bg_img_src(&style_radio_chk, NULL);

    tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 60);

    lv_obj_t *tabJoystick = lv_tabview_add_tab(tv, strJoystick);
    lv_obj_t *tabFileExplorer = lv_tabview_add_tab(tv, strFiles);
    lv_obj_t *tabParameters = lv_tabview_add_tab(tv, strParameters);
    lv_obj_t *tabTools = lv_tabview_add_tab(tv, strTools);

    CreateJoystick(tabJoystick);
    CreateFileExplorer(tabFileExplorer);
    CreateParameters(tabParameters);
    CreateTools(tabTools);

    state_label = lv_label_create(lv_scr_act());
    lv_label_set_text(state_label, "???");
    lv_obj_add_style(state_label, &style_text_muted, 0);
    lv_obj_align(state_label, LV_ALIGN_BOTTOM_LEFT, 15, -15);

    status_label = lv_label_create(lv_scr_act());
    lv_label_set_text(status_label, "???");
    lv_obj_add_style(status_label, &style_text_muted, 0);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_LEFT, 200, -15);

    alarm_label = lv_label_create(lv_scr_act());
    lv_label_set_text(alarm_label, "");
    lv_obj_add_style(alarm_label, &style_text_muted, 0);
    lv_obj_align(alarm_label, LV_ALIGN_BOTTOM_LEFT, 300, -15);
    lv_obj_set_style_text_color(alarm_label, lv_palette_main(LV_PALETTE_RED), 0);

    lv_obj_t *panelXYZ = lv_obj_create(lv_scr_act());
    lv_obj_align(panelXYZ, LV_ALIGN_BOTTOM_LEFT, 10, -60);

    objPosX = lv_label_create(panelXYZ);
    lv_label_set_text(objPosX, "X : ");
    lv_obj_align(objPosX, LV_ALIGN_TOP_LEFT, 0, 0);

    objPosY = lv_label_create(panelXYZ);
    lv_label_set_text(objPosY, "Y : ");
    lv_obj_align(objPosY, LV_ALIGN_TOP_LEFT, 100, 0);

    objPosZ = lv_label_create(panelXYZ);
    lv_label_set_text(objPosZ, "Z : ");
    lv_obj_align(objPosZ, LV_ALIGN_TOP_LEFT, 200, 0);

    lv_obj_set_height(panelXYZ, LV_SIZE_CONTENT);
    lv_obj_set_width(panelXYZ, 780);

    color_changer_create(tv);

    lv_timer_create(updateVals, 300, NULL);

    xTaskCreate(read_usb, "ReadUsb", 4096, NULL, 4, NULL);
    xTaskCreate(send_file, "SendFile", 4096, NULL, 3, NULL);
}

void app_main()
{

    hardwareInit();

    language = nvsRead("language");
    languageInit(language);

    if (hardLvglLock(-1))
    {
        CreatePanels();
        hardLvglUnlock();
    }
    
    usbLoop();
}
