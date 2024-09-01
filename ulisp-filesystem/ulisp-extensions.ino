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
  } else if (nargs > 0) error2(PSTR("wrong number of arguments"));
  
  // Return time
  unsigned long secs = Offset + now;
  object *seconds = number(secs%60);
  object *minutes = number((secs/60)%60);
  object *hours = number((secs/3600)%24);
  return cons(hours, cons(minutes, cons(seconds, NULL)));
}




void initTouchscreen()
{
  #if defined(touchscreen_support)

  tft_touch.setResolution( HRES, VRES) ;
  tft_touch.setRotation(0) ;
  
  // TouchInitCalibration
  // Change it by calibrated values 
  /*tft_touch.setCal(0, 4095, 0, 4095, HRES, VRES, 1 );
  tft_touch._xflip = 1 ;
  tft_touch._yflip = 1 ;*/

  tft_touch.setCal(526, 3451, 678, 3443, 320, 240, 0 ) ;
  tft_touch._xflip = 1 ;
  tft_touch._yflip = 1 ;


  #endif
}

object *fn_touch_press (object *args, object *env) {
  (void) env;
#if defined(touchscreen_support)

  return  tft_touch.Pressed() ? tee : nil;
#else
  return nil ;
#endif
}

object *fn_touch_x (object *args, object *env) {
  (void) env;
#if defined(touchscreen_support)
  return number(tft_touch.X());
#else
  return nil ;
#endif
}

object *fn_touch_y (object *args, object *env) {
  (void) env;
#if defined(touchscreen_support)
  return number(tft_touch.Y());
#else
  return nil ;
#endif
}

void PrintToucscreenParameters()
{
#if defined(touchscreen_support)
  pfstring(PSTR("\n(touch-setcal "), pserial);
  pint(tft_touch._hmin, pserial);  pserial(' ');
  pint(tft_touch._hmax, pserial);  pserial(' ');
  pint(tft_touch._vmin, pserial);  pserial(' ');
  pint(tft_touch._vmax, pserial);  pserial(' ');
  pint(tft_touch._hres, pserial);  pserial(' ');
  pint(tft_touch._vres, pserial);  pserial(' ');
  pint(tft_touch._xyswap, pserial);  pserial(' ');
  pint(tft_touch._xflip, pserial);  pserial(' ');
  pint(tft_touch._yflip, pserial);
  pfstring(PSTR(")\n"), pserial);
#endif
}

object *fn_touch_printcal(object *args, object *env) 
{
  (void) env;

  PrintToucscreenParameters() ;

  return tee ;
}

object *fn_touch_setcal(object *args, object *env) 
{
  (void) env;

#if defined(touchscreen_support)
  int hmin, hmax, vmin, vmax, hres, vres, xyswap, xflip, yflip ;
  object *obj = args ;

  if(obj) { hmin = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { hmax = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { vmin = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { vmax = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { hres = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { vres = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { xyswap = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { xflip = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj)
  { 
    yflip = checkinteger(car(obj)) ;

    tft_touch.setCal(hmin, hmax, vmin, vmax, hres, vres, xyswap);
    tft_touch._xflip = xflip ;
    tft_touch._yflip = yflip ;

    return tee ;
  }
#endif
  

return nil ;
}

void drawCalibrationPrompt()
{
  tft.setTextColor(TFT_RED, TFT_BLACK) ;
  tft.drawString("Touch calibration", 100, 40) ;
  tft.setTextColor(TFT_GREEN, TFT_BLACK) ;
  tft.drawString("Press this point", 100, 50) ;
}


void drawCalibrationTestPrompt()
{
  tft.setTextColor(TFT_BLUE, TFT_BLACK) ;
  tft.drawString("Test touch calibration", 100, 40) ;
  tft.setTextColor(TFT_GREEN, TFT_BLACK) ;
  tft.drawString("Press this point", 100, 50) ;
}


void drawCalibrationCross(uint16_t x, uint16_t y, uint16_t color)
{
  tft.drawLine(x-10, y, x+10, y, color) ;
  tft.drawLine(x, y-10, x, y+10, color) ;
  }


int TestTouchPoint(int x, int y, int x0, int y0)
{
  if((abs(x-x0)>5)||(abs(y-y0)>5)) return 0 ;

  return 1 ;
}

#define DEF_CROSS_SHIFT 30


object *fn_touch_calibrate(object *args, object *env) 
{
  (void) env;

#if defined(touchscreen_support)
  int X_Raw, Y_Raw ;
  int x1, y1;
  int x2, y2;
  int x3, y3;
  bool xyswap  = 0, xflip = 0, yflip = 0;


  // Reset the calibration values
  tft_touch.setCal(0, 4095, 0, 4095, HRES, VRES, 0);//, 0, 0);
  
  // Set TFT the screen to landscape orientation
  tft.setRotation(0);
    // Set Touch the screen to the same landscape orientation
  tft_touch.setRotation(3);
  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Show the screen prompt
  drawCalibrationPrompt();

  drawCalibrationCross(DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_RED);
  while (!tft_touch.Pressed());
  delay(200);
  
  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("First point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  x1 = X_Raw;
  y1 = Y_Raw;

  drawCalibrationCross(HRES/2, VRES/2, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits for the centre area to be touched
  delay(200);
  
  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(HRES/2, VRES/2, TFT_BLACK);
  Serial.print("Second point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

// not used

  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits until the centre area is no longer pressed
  delay(200);           // Wait a little for touch bounces to stop after release
  
  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("Third point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  x2 = X_Raw;
  y2 = Y_Raw;

 
  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_RED);


  while (!tft_touch.Pressed()); // wait press touch
  delay(200);

  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch
 
  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_BLACK);

  Serial.print("Fourth point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

// not used

  drawCalibrationCross(DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits until the centre area is no longer pressed
  delay(200);           // Wait a little for touch bounces to stop after release
  
  X_Raw = tft_touch.RawX(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.RawY();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("Fifth point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  x3 = X_Raw;
  y3 = Y_Raw;

  int temp;
  if (abs(x1 - x3) > 1000) {
    xyswap = 1;
    temp = x1; x1 = y1; y1 = temp;
    temp = x2; x2 = y2; y2 = temp;
    temp = x3; x3 = y3; y3 = temp;
  }
  else xyswap = 0;
  


  if (x2 < x1) {
    temp = x2; x2 = x1; x1 = temp;
    xflip = 1;
  }
  
  if (y2 < y1) {
    temp = y2; y2 = y1; y1 = temp;
    yflip = 1;
  }

  int hmin = x1;// - (x2 - x1) * 3 / (HRES/10 - 6);
  int hmax = x2;// + (x2 - x1) * 3 / (HRES/10 - 6);

  int vmin = y1;// - (y2 - y1) * 3 / (VRES/10 - 6);
  int vmax = y2;// + (y2 - y1) * 3 / (VRES/10 - 6);


  tft_touch.setCal(hmin,hmax,vmin,vmax, HRES, VRES, xyswap);
  
  tft_touch._xflip = xflip ;
  tft_touch._yflip = yflip ;

  
  pfstring(PSTR("\nUse command:"), pserial);
  PrintToucscreenParameters() ;

  // *********  Test calibration result  *************
  drawCalibrationTestPrompt() ;

  Serial.print("\n\nTest of calibration\n");


  drawCalibrationCross(DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_RED);
  while (!tft_touch.Pressed());
  delay(200);
  
  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_BLACK);

  Serial.print("First point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw,DEF_CROSS_SHIFT, DEF_CROSS_SHIFT)) return nil ;

  drawCalibrationCross(HRES/2, VRES/2, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits for the centre area to be touched
  delay(200);
  
  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(HRES/2, VRES/2, TFT_BLACK);
  Serial.print("Second point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw, HRES/2, VRES/2)) return nil ;

  // point not used

  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits until the centre area is no longer pressed
  delay(200);           // Wait a little for touch bounces to stop after release
  
  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("Third point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw, HRES-DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT)) return nil ;


  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_RED);

  while (!tft_touch.Pressed()); // wait press touch
  delay(200);

  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch
 
  drawCalibrationCross(HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT, TFT_BLACK);

  Serial.print("Fourth point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw, HRES-DEF_CROSS_SHIFT, DEF_CROSS_SHIFT)) return nil ;

  //  point not used

  drawCalibrationCross(DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_RED);
  
  while (!tft_touch.Pressed()); // This waits until the centre area is no longer pressed
  delay(200);           // Wait a little for touch bounces to stop after release
  
  X_Raw = tft_touch.X(); // This function assigns values to X_Raw and Y_Raw
  Y_Raw = tft_touch.Y();

  while (tft_touch.Pressed()); // wait release touch

  drawCalibrationCross(DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT, TFT_BLACK);
  Serial.print("Fifth point : x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);
  if(!TestTouchPoint(X_Raw, Y_Raw, DEF_CROSS_SHIFT, VRES-DEF_CROSS_SHIFT)) return nil ;


  return tee;
#else
  return nil ;
#endif
}





object *fn_probefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x08 | 0x08 ;  // Files and directories
  char pattern_string[256] = "*" ;
  char dirname_string[256] = "/";

  if (args != NULL)
  {   //  Directory name
      if(stringp(car(args)))
      {
        cstring(car(args), pattern_string, 256) ;
      }
      else {  pfstring("\nprobe-file: First argument must be string.", pserial); return nil; }
  }

  //pfstring(dirname_string, pserial);

  SD.begin(SDCARD_SS_PIN);
  if(SD.exists(pattern_string))  return tee; 

  
  return nil;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}



object *fn_deletefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x8 | 0x4;  // Files and directories
  char pattern_string[256] = "*" ;

  if (args != NULL)
  {   //  Directory name
      if(stringp(car(args)))
      {
        cstring(car(args), pattern_string, 256) ;
      }
      else {  pfstring("\ndelete-file: First argument must be string.", pserial); return nil; }
  }

  SD.begin(SDCARD_SS_PIN);
  if(SD.exists(pattern_string))
  {
    if(SD.remove(pattern_string)) return tee;
  }
  else return tee;


  return nil;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}




object *fn_renamefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x8 ;  // Files and directories
  char filename_string[256] = "*" ;
  char newname_string[256] = "/";

  if (args != NULL)
  {   //  Directory name
      if(stringp(car(args)))
      {
        cstring(car(args), filename_string, 256) ;
        args = cdr(args);
        if (args != NULL)
            if(stringp(car(args)))
                cstring(car(args), newname_string, 256) ;
            else  {  pfstring("\nrename-file: Second argument must be string.", pserial); return nil; }
      }
      else  {  pfstring("\nrename-file: First argument must be string.", pserial); return nil; }
  }

  //pfstring("\nrename-file: not supported.", pserial);

  SD.begin(SDCARD_SS_PIN);
  if (!SD.exists(filename_string)) {  pfstring("file not exists", pserial); return nil; }

  File fp_source = SD.open(filename_string, FILE_READ);

  if (SD.exists(newname_string)) SD.remove(newname_string) ;
  File fp_dest = SD.open(newname_string, FILE_WRITE);
  if (!fp_dest) {  pfstring("Cannot open destination file.\n", pserial); return nil; }

  uint32_t i, sz ;
  sz = fp_source.size();

  for(i=0; i<sz;i++) fp_dest.write(fp_source.read()) ;

  fp_source.close();
  fp_dest.close();
  SD.remove(filename_string) ;

  return tee;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}


object *fn_copyfile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x8 ;  // Files and directories
  char filename_string[256] = "*" ;
  char newname_string[256] = "/";

  if (args != NULL)
  {   //  Directory name
      if(stringp(car(args)))
      {
        cstring(car(args), filename_string, 256) ;
        args = cdr(args);
        if (args != NULL)
            if(stringp(car(args)))
                cstring(car(args), newname_string, 256) ;
            else  {  pfstring("\nrename-file: Second argument must be string.", pserial); return nil; }
      }
      else  {  pfstring("\nrename-file: First argument must be string.", pserial); return nil; }
  }

  //pfstring("\nrename-file: not supported.", pserial);

  SD.begin(SDCARD_SS_PIN);
  if (!SD.exists(filename_string)) {  pfstring("file not exists", pserial); return nil; }

  File fp_source = SD.open(filename_string, FILE_READ);

  if (SD.exists(newname_string)) SD.remove(newname_string) ;
  File fp_dest = SD.open(newname_string,FILE_WRITE);
  if (!fp_dest) {  pfstring("Cannot open destination file.\n", pserial); return nil; }

  uint16_t i, sz ;
  sz = fp_source.size();

  for(i=0; i<sz;i++) fp_dest.write(fp_source.read()) ;

  fp_source.close();
  fp_dest.close();

  return tee;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}






object *fn_ensuredirectoriesexist(object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x8 ;  // Files and directories
  char pattern_string[256] = "*" ;
 
  if (args != NULL)
  {   //  Directory name
      if(stringp(car(args)))
      {
        cstring(car(args), pattern_string, 256) ;
      }
      else  {  pfstring("\nError: argument must be string", pserial); return nil; }
  }

  SD.begin(SDCARD_SS_PIN);
  if(!SD.exists(pattern_string))
  {
    if(SD.mkdir(pattern_string)) return tee;
  }
  else return tee;

  return nil;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}




// Symbol names
const char stringnow[] PROGMEM  = "now";
const char stringtouch_press[] PROGMEM = "touch-press";
const char stringtouch_x[] PROGMEM = "touch-x";
const char stringtouch_y[] PROGMEM = "touch-y";
const char stringtouch_calibrate[] PROGMEM = "touch-calibrate";
const char stringtouch_setcal[] PROGMEM = "touch-setcal";
const char stringtouch_printcal[] PROGMEM = "touch-printcal";

const char string_probefile[] PROGMEM = "probe-file";
const char string_deletefile[] PROGMEM = "delete-file";
const char string_renamefile[] PROGMEM = "rename-file";
const char string_copyfile[] PROGMEM = "copy-file";
const char string_ensuredirectoriesexist[] PROGMEM = "ensure-directories-exist";



// Documentation strings
const char docnow[] PROGMEM  = "(now [hh mm ss])\n"
"Sets the current time, or with no arguments returns the current time\n"
"as a list of three integers (hh mm ss).";

const char doctouch_press[] PROGMEM = "(touch-press)\n"
"Returns true if touchscreen is pressed and false otherwise.";
const char doctouch_x[] PROGMEM = "(touch-x)\n"
"Returns pressed touchscreen x-coordinate.";
const char doctouch_y[] PROGMEM = "(touch-y)\n"
"Returns pressed touchscreen y-coordinate.";
const char doctouch_calibrate[] PROGMEM = "(touch-calibrate)\n"
"Runs touchscreen calibration.";
const char doctouch_setcal[] PROGMEM = "(touch-setcal minx maxx miny maxy\n      hres vres axisswap xflip yflip)\n"
"Set touchscreen calibration parameters.";
const char doctouch_printcal[] PROGMEM = "(touch-printcal)\n"
"Print touchscreen calibration parameters.";
const char doc_readserial[] PROGMEM = "(read-serial)\n"
"Reads a byte from a serial port and returns it.";



const char doc_probefile[] PROGMEM = "(probe-file pathspec)\n"
"tests whether a file exists.\n"
" Returns nil if there is no file named pathspec,"
" and otherwise returns the truename of pathspec.";
const char doc_deletefile[] PROGMEM = "(delete-file pathspec)\n"
"delete specified file.\n"
" Returns true if success and otherwise returns nil.";
const char doc_renamefile[] PROGMEM = "(rename-file filespec newfile)\n"
"rename or moving specified file.\n"
" Returns true if success and otherwise returns nil.";
const char doc_copyfile[] PROGMEM = "(copy-file filespec newfile)\n"
"copy specified file.\n"
" Returns true if success and otherwise returns nil.";
const char doc_ensuredirectoriesexist[] PROGMEM = "(ensure-directories-exist pathspec)\n"
"Tests whether the directories containing the specified file actually exist,"
" and attempts to create them if they do not.\n"
" Returns true if success and otherwise returns nil.";



// Symbol lookup table
const tbl_entry_t lookup_table2[] PROGMEM  = {
    { stringnow, fn_now, 0203, docnow },
    { stringtouch_press, fn_touch_press, 0200, doctouch_press },
    { stringtouch_x, fn_touch_x, 0200, doctouch_x },
    { stringtouch_y, fn_touch_y, 0200, doctouch_y },
    { stringtouch_calibrate, fn_touch_calibrate, 0200, doctouch_calibrate },
    { stringtouch_setcal, fn_touch_setcal, 0217, doctouch_setcal },
    { stringtouch_printcal, fn_touch_printcal, 0200, doctouch_printcal },

    { string_probefile, fn_probefile, 0211, doc_probefile },
    { string_renamefile, fn_renamefile, 0222, doc_renamefile },
    { string_copyfile, fn_copyfile, 0222, doc_copyfile },
    { string_deletefile, fn_deletefile, 0211, doc_deletefile },
    { string_ensuredirectoriesexist, fn_ensuredirectoriesexist, 0211, doc_ensuredirectoriesexist },

};




// Table cross-reference functions

tbl_entry_t *tables[] = {lookup_table, lookup_table2};
const unsigned int tablesizes[] PROGMEM = { arraysize(lookup_table), arraysize(lookup_table2) };

const tbl_entry_t *table (int n) {
  return tables[n];
}

unsigned int tablesize (int n) {
  return tablesizes[n];
}
