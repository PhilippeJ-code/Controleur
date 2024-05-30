#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMG_FOLDER
#define LV_ATTRIBUTE_IMG_IMG_FOLDER
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_IMG_FOLDER uint8_t img_folder_map[] = {
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x3f, 0x00, 0x00, 
  0x3f, 0x80, 0x00, 
  0x3f, 0xff, 0x00, 
  0x3f, 0xff, 0x00, 
  0x3f, 0xff, 0x00, 
  0x3f, 0xff, 0x00, 
  0x3f, 0xff, 0x00, 
  0x3f, 0xff, 0x00, 
  0x3f, 0xff, 0x00, 
  0x3f, 0xff, 0x00, 
  0x3f, 0xff, 0x00, 
  0x3f, 0xff, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
};

const lv_img_dsc_t img_folder = {
  .header.cf = LV_IMG_CF_ALPHA_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 18,
  .header.h = 18,
  .data_size = 54,
  .data = img_folder_map,
};
