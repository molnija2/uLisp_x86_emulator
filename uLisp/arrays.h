#ifndef ARRAYS_H
#define ARRAYS_H

#include "ulisp.h"


typedef struct {
  int32_t dim[8];
  int32_t dimstep[8];
  int ndim;
  int element_size;
  size_t size;
  uintptr_t  type ;
  object bufferobj ;
  object *obj_pointer ;
} array_desc_t;


const char string_makearray2[] = "make-array*";
const char string_aref2[] = "aref*";
const char string_delarray2[] = "del-array*";
const char string_array2p[] = "arrayp*";

const char string_getimage[] = "get-image";
const char string_putimage[] = "put-image";
const char string_loadbitmap[] = "load-bitmap";
const char string_savebitmap[] = "save-bitmap";
const char string_drawbitmap[] = "draw-bitmap";
const char string_imagewidth[] = "bitmap-width";
const char string_imageheight[] = "bitmap-height";


const char string_integer[] = "integer";
const char string_char[] = "character";
const char string_singlefloat[] = "single-float";
const char string_doublefloat[] = "double-float";




const char doc_makearray2[] = "(make-array* size [:initial-element element] [:element-type 'bit])\n"
"If size is an integer it creates a one-dimensional array with elements from 0 to size-1.\n"
"If size is a list of n integers it creates an n-dimensional array with those dimensions.\n"
"If :element-type 'bit is specified the array is a bit array.";

const char doc_aref2[] = "(aref* array index [index*])\n"
"Returns an element from the specified array.";

const char doc_delarray2[] = "(del-array* array)\n"
"Delete array.";

const char doc_array2p[] = "(arrayp* item)\n"
"Returns t if its argument is an array.\n";

const char doc_getimage[] = "(get-image x y w h)\n"
"Get image from screen rectangle area.";

const char doc_putimage[] = "(put-image x y array2)\n"
"Put image from array to screen rectangle area.";

const char doc_imagewidth[] = "(bitmap-width array2)\n"
"Returns image width in pixels.";

const char doc_imageheight[] = "(bitmap-height array2)\n"
"Returns image height in pixels.";

const char doc_loadbitmap[] = "(load-bitmap file)\n"
"Get image from file.";

const char doc_savebitmap[] = "(save-bitmap file)\n"
"Save image to file.";

const char doc_drawbitmap[] = "(draw-bitmap x y file)\n"
"Draw image from file.";



#endif // ARRAYS_H
