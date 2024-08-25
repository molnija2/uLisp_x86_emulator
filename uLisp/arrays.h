#ifndef ARRAYS_H
#define ARRAYS_H



const char string_makearray2[] = "make-array*";
const char string_aref2[] = "aref*";
const char string_delarray2[] = "del-array*";


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



#endif // ARRAYS_H
