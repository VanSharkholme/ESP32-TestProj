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

#ifndef LV_ATTRIBUTE_IMG_INFO_ICON_WAVETYPE_FIT
#define LV_ATTRIBUTE_IMG_INFO_ICON_WAVETYPE_FIT
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_INFO_ICON_WAVETYPE_FIT uint8_t Info_Icon_Wavetype_fit_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Alpha 8 bit, Red: 3 bit, Green: 3 bit, Blue: 2 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x36, 0xff, 0xb3, 0xff, 0xbc, 0xff, 0x93, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x04, 0xff, 0x98, 0xff, 0x53, 0xff, 0x12, 0xff, 0x8c, 0xff, 0x5c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0x40, 0xff, 0x41, 0xff, 0x40, 0xff, 0x61, 0xff, 0xa0, 0xff, 0x08, 0x00, 0x00, 0xff, 0x38, 0xff, 0x99, 0xff, 0x06, 0x00, 0x00, 0x00, 0x00, 0xff, 0x21, 0xff, 0x46, 0xff, 0x46, 0xff, 0x46, 0xff, 0x44, 
  0xff, 0x86, 0xff, 0x89, 0xff, 0x88, 0xff, 0x8d, 0xff, 0x45, 0x00, 0x00, 0x00, 0x00, 0xff, 0x0b, 0xff, 0xa0, 0xff, 0x2c, 0x00, 0x00, 0xff, 0x08, 0xff, 0x9c, 0xff, 0x95, 0xff, 0x84, 0xff, 0x85, 0xff, 0x81, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x6b, 0xff, 0x7f, 0xff, 0x0c, 0xff, 0x56, 0xff, 0x95, 0xff, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x18, 0xff, 0xa1, 0xff, 0xbb, 0xff, 0xb4, 0xff, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x36, 0xff, 0xff, 0xb3, 0xff, 0xff, 0xbc, 0xff, 0xff, 0x93, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x04, 0xff, 0xff, 0x98, 0xff, 0xff, 0x53, 0xff, 0xff, 0x12, 0xff, 0xff, 0x8c, 0xff, 0xff, 0x5c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0xff, 0x40, 0xff, 0xff, 0x41, 0xff, 0xff, 0x40, 0xff, 0xff, 0x61, 0xff, 0xff, 0xa0, 0xff, 0xff, 0x08, 0x00, 0x00, 0x00, 0xff, 0xff, 0x38, 0xff, 0xff, 0x99, 0xff, 0xff, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x21, 0xff, 0xff, 0x46, 0xff, 0xff, 0x46, 0xff, 0xff, 0x46, 0xff, 0xff, 0x44, 
  0xff, 0xff, 0x86, 0xff, 0xff, 0x89, 0xff, 0xff, 0x88, 0xff, 0xff, 0x8d, 0xff, 0xff, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x0b, 0xff, 0xff, 0xa0, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0xff, 0xff, 0x08, 0xff, 0xff, 0x9c, 0xff, 0xff, 0x95, 0xff, 0xff, 0x84, 0xff, 0xff, 0x85, 0xff, 0xff, 0x81, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x6b, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x0c, 0xff, 0xff, 0x56, 0xff, 0xff, 0x95, 0xff, 0xff, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x18, 0xff, 0xff, 0xa1, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xb4, 0xff, 0xff, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit  BUT the 2  color bytes are swapped*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x36, 0xff, 0xff, 0xb3, 0xff, 0xff, 0xbc, 0xff, 0xff, 0x93, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x04, 0xff, 0xff, 0x98, 0xff, 0xff, 0x53, 0xff, 0xff, 0x12, 0xff, 0xff, 0x8c, 0xff, 0xff, 0x5c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0xff, 0x40, 0xff, 0xff, 0x41, 0xff, 0xff, 0x40, 0xff, 0xff, 0x61, 0xff, 0xff, 0xa0, 0xff, 0xff, 0x08, 0x00, 0x00, 0x00, 0xff, 0xff, 0x38, 0xff, 0xff, 0x99, 0xff, 0xff, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x21, 0xff, 0xff, 0x46, 0xff, 0xff, 0x46, 0xff, 0xff, 0x46, 0xff, 0xff, 0x44, 
  0xff, 0xff, 0x86, 0xff, 0xff, 0x89, 0xff, 0xff, 0x88, 0xff, 0xff, 0x8d, 0xff, 0xff, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x0b, 0xff, 0xff, 0xa0, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0xff, 0xff, 0x08, 0xff, 0xff, 0x9c, 0xff, 0xff, 0x95, 0xff, 0xff, 0x84, 0xff, 0xff, 0x85, 0xff, 0xff, 0x81, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x6b, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x0c, 0xff, 0xff, 0x56, 0xff, 0xff, 0x95, 0xff, 0xff, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x18, 0xff, 0xff, 0xa1, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xb4, 0xff, 0xff, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x36, 0xff, 0xff, 0xff, 0xb3, 0xff, 0xff, 0xff, 0xbc, 0xff, 0xff, 0xff, 0x93, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x04, 0xff, 0xff, 0xff, 0x98, 0xff, 0xff, 0xff, 0x53, 0xff, 0xff, 0xff, 0x12, 0xff, 0xff, 0xff, 0x8c, 0xff, 0xff, 0xff, 0x5c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0xff, 0xff, 0x40, 0xff, 0xff, 0xff, 0x41, 0xff, 0xff, 0xff, 0x40, 0xff, 0xff, 0xff, 0x61, 0xff, 0xff, 0xff, 0xa0, 0xff, 0xff, 0xff, 0x08, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x38, 0xff, 0xff, 0xff, 0x99, 0xff, 0xff, 0xff, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x21, 0xff, 0xff, 0xff, 0x46, 0xff, 0xff, 0xff, 0x46, 0xff, 0xff, 0xff, 0x46, 0xff, 0xff, 0xff, 0x44, 
  0xff, 0xff, 0xff, 0x86, 0xff, 0xff, 0xff, 0x89, 0xff, 0xff, 0xff, 0x88, 0xff, 0xff, 0xff, 0x8d, 0xff, 0xff, 0xff, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x0b, 0xff, 0xff, 0xff, 0xa0, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x08, 0xff, 0xff, 0xff, 0x9c, 0xff, 0xff, 0xff, 0x95, 0xff, 0xff, 0xff, 0x84, 0xff, 0xff, 0xff, 0x85, 0xff, 0xff, 0xff, 0x81, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x6b, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x0c, 0xff, 0xff, 0xff, 0x56, 0xff, 0xff, 0xff, 0x95, 0xff, 0xff, 0xff, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x18, 0xff, 0xff, 0xff, 0xa1, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xb4, 0xff, 0xff, 0xff, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
};

const lv_img_dsc_t Info_Icon_Wavetype_fit = {
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 17,
  .header.h = 6,
  .data_size = 102 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .data = Info_Icon_Wavetype_fit_map,
};
