#include <unistd.h>
#include <stdlib.h>


#include "ulisp.h"
#include "graph_tcp.h"

#include "arrays.h"
#include "filesys.h"

//extern object *tee;

/*
 User Extensions
*/



// Definitions




object *fn_now (object *args, object *env) {
  (void) env;
  static unsigned long Offset;
  unsigned long now = millis()/1000;
  int nargs = listlength(args);

  // Set time
  if (nargs == 3) {
    Offset = (unsigned long)((checkinteger(first(args))*60 + checkinteger(second(args)))*60
      + checkinteger(third(args)) - now);
  } else if (nargs > 0) error2("wrong number of arguments");
  
  // Return time
  unsigned long secs = Offset + now;
  object *seconds = number(secs%60);
  object *minutes = number((secs/60)%60);
  object *hours = number((secs/3600)%24);
  return cons(hours, cons(minutes, cons(seconds, NULL)));
}



object *fn_touch_press (object *args, object *env) {
  (void) env;
#ifdef touchscreen_support

  return  MouseStateButtons() ? tee : nil;
#else
  return nil ;
#endif
}

object *fn_touch_x (object *args, object *env) {
  (void) env;
#ifdef  touchscreen_support
  return number(MouseStateX());
#else
  return nil ;
#endif
}

object *fn_touch_y (object *args, object *env) {
  (void) env;
#ifdef touchscreen_support


    return number(MouseStateY());
#else
  return nil ;
#endif
}

void PrintToucscreenParameters ()
{
#ifdef touchscreen_support

#endif
}

object *fn_touch_printcal (object *args, object *env)
{
  (void) env;

  PrintToucscreenParameters() ;

  return tee ;
}

object *fn_touch_setcal(object *args, object *env)
{
  (void) env;

#ifdef touchscreen_support
    return tee ;

#endif


return nil ;
}

void drawCalibrationPrompt()
{
  //tft.setTextColor(TFT_RED, TFT_BLACK) ;
  //tft.drawString("Touch calibration", 100, 40) ;
  //tft.setTextColor(TFT_GREEN, TFT_BLACK) ;
  //tft.drawString("Press this point", 100, 50) ;
}


void drawCalibrationTestPrompt()
{
  //tft.setTextColor(TFT_BLUE, TFT_BLACK) ;
  //tft.drawString("Test touch calibration", 100, 40) ;
  //tft.setTextColor(TFT_GREEN, TFT_BLACK) ;
  //tft.drawString("Press this point", 100, 50) ;
}


void drawCalibrationCross(uint16_t x, uint16_t y, uint16_t color)
{
  //tft.drawLine(x-10, y, x+10, y, color) ;
  //tft.drawLine(x, y-10, x, y+10, color) ;
}






object *fn_touch_calibrate(object *args, object *env)
{
  (void) env;

#ifdef touchscreen_support


  return tee;
#else
  return nil ;
#endif
}



/*
(kbhit)  -  test whether any keyboard keys hits"
*/

object *fn_kbhit (object *args, object *env) {
  (void) env;
    int tcp_kbhit(void) ;

  if(tcp_kbhit()) return tee ;

  return  nil;
}





// Symbol names
const char stringnow[]  = "now";
const char stringtouch_press[] = "touch-press";
const char stringtouch_x[] = "touch-x";
const char stringtouch_y[] = "touch-y";
const char stringtouch_calibrate[] = "touch-calibrate";
const char stringtouch_setcal[] = "touch-setcal";
const char stringtouch_printcal[] = "touch-printcal";
const char stringquit[] = "quit";
const char string_kbhit[] = "kbhit" ;


// Documentation strings
const char docnow[]  = "(now [hh mm ss])\n"
"Sets the current time, or with no arguments returns the current time\n"
"as a list of three integers (hh mm ss).";

const char doctouch_press[] = "(touch-press)\n"
"Returns true if touchscreen is pressed and false otherwise.";
const char doctouch_x[] = "(touch-x)\n"
"Returns pressed touchscreen x-coordinate.";
const char doctouch_y[] = "(touch-y)\n"
"Returns pressed touchscreen y-coordinate.";
const char doctouch_calibrate[] = "(touch-calibrate)\n"
"Runs touchscreen calibration.";
const char doctouch_setcal[] = "(touch-setcal minx maxx miny maxy\n      hres vres axisswap xflip yflip)\n"
"Set touchscreen calibration parameters.";
const char doctouch_printcal[] = "(touch-printcal)\n"
"Print touchscreen calibration parameters.";

const char docquit[] = "(quit)\n"
"Exit from Lisp.";

const char doc_kbhit[] = "(kbhit) - test whether any keyboard keys hits.\n"
" Returns t if ney char symbols are available"
"and otherwise returnsnil.";


// Symbol lookup table
const tbl_entry_t lookup_table2[]  = {

    { stringnow, fn_now, 0203, docnow },
    { stringtouch_press, fn_touch_press, 0200, doctouch_press },
    { stringtouch_x, fn_touch_x, 0200, doctouch_x },
    { stringtouch_y, fn_touch_y, 0200, doctouch_y },
    { stringtouch_calibrate, fn_touch_calibrate, 0200, doctouch_calibrate },
    { stringtouch_setcal, fn_touch_setcal, 0217, doctouch_setcal },
    { stringtouch_printcal, fn_touch_printcal, 0200, doctouch_printcal },
    { string_makearray2, fn_makearray2, 0215, doc_makearray2 },

    { string_aref2, fn_aref2, 0227, doc_aref2 },
    { string_probefile, fn_probefile, 0211, doc_probefile },
    { string_renamefile, fn_renamefile, 0222, doc_renamefile },
    { string_copyfile, fn_copyfile, 0222, doc_copyfile },
    { string_deletefile, fn_deletefile, 0211, doc_deletefile },
    { string_ensuredirectoriesexist, fn_ensuredirectoriesexist, 0211, doc_ensuredirectoriesexist },

    { string_kbhit, fn_kbhit, 0200, doc_kbhit },
    { string_getimage, fn_getimage, 0244, doc_getimage },
    { string_putimage, fn_putimage, 0233, doc_putimage },
    { string_loadbitmap, fn_loadbitmap, 0211, doc_loadbitmap },
    { string_savebitmap, fn_savebitmap, 0222, doc_savebitmap },


    { stringquit, fn_quit, 0203, docquit },

    { string_char, NULL, 0000, NULL },
    { string_singlefloat, NULL, 0000, NULL },
    { string_doublefloat, NULL, 0000, NULL },
    { string_integer, NULL, 0000, NULL },

};

// Table cross-reference functions

const tbl_entry_t *tables[] = {lookup_table, lookup_table2};
unsigned int tablesizes[] = { arraysize(lookup_table), arraysize(lookup_table2) };

tbl_entry_t *table (int n) {
  return (tbl_entry_t *)tables[n];
}

unsigned int tablesize (int n) {
  return tablesizes[n];
}
