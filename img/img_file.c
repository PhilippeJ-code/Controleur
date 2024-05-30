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

#ifndef LV_ATTRIBUTE_IMG_IMG_FILE
#define LV_ATTRIBUTE_IMG_IMG_FILE
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_IMG_FILE uint8_t img_file_map[] = {
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x1f, 0xe0, 0x00, 
  0x1f, 0xd0, 0x00, 
  0x1f, 0xc8, 0x00, 
  0x1f, 0xc4, 0x00, 
  0x1f, 0xc6, 0x00, 
  0x1f, 0xfe, 0x00, 
  0x1f, 0xfe, 0x00, 
  0x1f, 0xfe, 0x00, 
  0x1f, 0xfe, 0x00, 
  0x1f, 0xfe, 0x00, 
  0x1f, 0xfe, 0x00, 
  0x1f, 0xfe, 0x00, 
  0x1f, 0xfe, 0x00, 
  0x1f, 0xfe, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
};

const lv_img_dsc_t img_file = {
  .header.cf = LV_IMG_CF_ALPHA_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 18,
  .header.h = 18,
  .data_size = 54,
  .data = img_file_map,
};
