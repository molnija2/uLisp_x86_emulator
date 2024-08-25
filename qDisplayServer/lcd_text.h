/*
 * lcd_text.h
 *
 *  Created on: Oct 21, 2023
 *      Author: sap
 */

#ifndef USER_LCD_TEXT_H_
#define USER_LCD_TEXT_H_

#define FONT_STYLE_NORMAL		0
#define FONT_STYLE_ITALIC		1
#define FONT_STYLE_BOLD			2
#define FONT_STYLE_BOLD_ITALIC	3


#include <inttypes.h>


#include "fonts/font_Nimb_Regular_14_22_hx.h"
#include "fonts/font_Nimb_Regular_Oblique_16_21_hx.h"
#include "fonts/font_Nimb_Bold_15_21_hx.h"
#include "fonts/font_Nimb_Bold_Oblique_16_21_hx.h"


#include "fonts/font_Nimb_Regular_8_14_hx.h"
#include "fonts/font_Nimb_Regular_Oblique_9_14_hx.h"
#include "fonts/font_Nimb_Bold_9_13_hx.h"
#include "fonts/font_Nimb_Bold_Oblique_10_13_hx.h"


#include "fonts/font_Nimb_Bold_Oblique_15_20_hx.h"
#include "fonts/font_Nimb_Bold_Oblique_13_18_hx.h"

#include "fonts/font_Deja_Book_15_27_hx.h"
#include "fonts/font_Deja_Bold_15_27_hx.h"
#include "fonts/font_Deja_Oblique_16_27_hx.h"
#include "fonts/font_Deja_Bold_Oblique_16_27_hx.h"


#include "fonts/font_Deja_Book_10_19_hx.h"
#include "fonts/font_Deja_Oblique_10_18_hx.h"
#include "fonts/font_Deja_Bold_11_19_hx.h"
#include "fonts/font_Deja_Bold_Oblique_11_19_hx.h"


#include "fonts/font_Droi_Regular_8_15_hx.h"
#include "fonts/font_Deja_Book_9_16_hx.h"
#include "fonts/font_Deja_Oblique_9_16_hx.h"
#include "fonts/font_Deja_Bold_9_15_hx.h"
#include "fonts/font_Deja_Bold_Oblique_9_15_hx.h"


#include "fonts/font_Nimb_Regular_Oblique_12_16_hx.h"


#include "fonts/font_Nimb_Regular_7_12_hx.h"
#include "fonts/font_Nimb_Regular_Oblique_8_12_hx.h"
#include "fonts/font_Nimb_Bold_8_11_hx.h"
#include "fonts/font_Nimb_Bold_Oblique_9_11_hx.h"





typedef struct{
    uint16_t Height ;
    uint16_t Width ;
    uint16_t Style ;
    uint16_t Dir ;
    char name[64] ;
    uint16_t *DATA ;
} FONT_INFO;



#endif /* USER_LCD_TEXT_H_ */
