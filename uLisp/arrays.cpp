#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <stdlib.h>


#include "ulisp.h"

#include "arrays.h"
#include "bmp.h"




/*object *buildarray2(int n, object *def);
object **getarray2(object *array, object *subs, object *env, int *bit) ;
object **arrayref2(object *array, int index, int size);*/

#define atom(x) (!consp(x))
extern object *tee;


enum { CHARACTER_ARRAY, SINGLEFLOAT_ARRAY, DOUBLEFLOAT_ARRAY, INTEGER_ARRAY };


symbol_t array2_type_name[8] = {0,0,0,0,0,0,0,0}, Aref2_name ;

/*symbol_t array2_char_name ;
symbol_t array2_singlefl_name ;
symbol_t array2_doublefl_name ;
symbol_t array2_integer_name ;*/

void InitArray2()
{
    if(array2_type_name[0] == 0)
    {
        array2_type_name[SINGLEFLOAT_ARRAY] = sym(lookupbuiltin ((char*)"single-float")) ;
        array2_type_name[DOUBLEFLOAT_ARRAY] = sym(lookupbuiltin ((char*)"double-float")) ;
        array2_type_name[INTEGER_ARRAY] = sym(lookupbuiltin ((char*)"integer")) ;
        array2_type_name[CHARACTER_ARRAY] = sym(lookupbuiltin ((char*)"character")) ;
        Aref2_name = sym(lookupbuiltin ((char*)"aref*")) ;
    }
}


void filldimensionsteps(array_desc_t *descriptor)
{
    int32_t sz = 1 ;
    int i ;
    for(i=descriptor->ndim-1;i>=0;i--)
    {
        descriptor->dimstep[i] = sz ;
        sz *= descriptor->dim[i] ;
    }
}


object *makearray2 (object *dims, object *def, bool bitp, int type) {
  int size = 1, i = 0;
  array_desc_t descriptor ;
  //object *dimensions = dims;
  while (dims != NULL) {
    int d = car(dims)->integer;
    if (d < 0) error2(PSTR("dimension can't be negative"));
    descriptor.dim[i] = d;
    size = size * d;
    dims = cdr(dims);
    i++;
  }
  descriptor.ndim = i;
  descriptor.size = size;
  filldimensionsteps(&descriptor);

  if(def)
      descriptor.type = def->type ;
  else
      descriptor.type = type ;

  // Bit array identified by making first dimension negative
  /*if (bitp) {
    size = (size + sizeof(int)*8 - 1)/(sizeof(int)*8);
    car(dimensions) = number(-(car(dimensions)->integer));
  }*/
  object *ptr = myalloc();
  ptr->type = ARRAY2;


    switch(descriptor.type)
    {
    case FLOAT:
       {
         descriptor.element_size = sizeof(sfloat_t) ;
         ptr->pointer = (void*)malloc(sizeof(sfloat_t)*size+sizeof(descriptor));
         uintptr_t array_pointer = (uintptr_t)(ptr->pointer) + sizeof (array_desc_t) ;
         sfloat_t *Ptr = (sfloat_t*)(array_pointer) ;
         if(def) for(i=0;i<size;i++)  *Ptr ++= def->single_float ;
         else for(i=0;i<size;i++)  *Ptr ++= 0.0 ;
       }
        break ;
    case CHAR:
       {
         descriptor.element_size = sizeof(char) ;
         ptr->pointer = (void*)malloc(sizeof(char)*size+sizeof(descriptor));
         uintptr_t array_pointer = (uintptr_t)(ptr->pointer) + sizeof (array_desc_t) ;
         char *Ptr = (char*)(array_pointer) ;
         if(def) for(i=0;i<size;i++)  *Ptr ++= (def->chars)&0x0ff ;
          else for(i=0;i<size;i++)  *Ptr ++= 0 ;
       }
        break;
    case NUMBER:
    default : ;
       {
         descriptor.element_size = sizeof(long int) ;
         ptr->pointer = (void*)malloc(sizeof(long int)*size+sizeof(descriptor));
         uintptr_t array_pointer = (uintptr_t)(ptr->pointer) + sizeof (array_desc_t) ;
         long int *Ptr = (long int*)(array_pointer) ;
         if(def) for(i=0;i<size;i++)  *Ptr ++= def->integer ;
         else for(i=0;i<size;i++)  *Ptr ++= 0 ;
       }
    }

    *(array_desc_t*)(ptr->pointer) = descriptor;

    //ptr->cdr = cons(buffer, nil);
  return ptr;
}






/*
  (make-array size [:initial-element element] [:element-type 'bit])
  If size is an integer it creates a one-dimensional array with elements from 0 to size-1.
  If size is a list of n integers it creates an n-dimensional array with those dimensions.
  If :element-type 'bit is specified the array is a bit array.
*/
object *fn_makearray2 (object *args, object *env) {
  (void) env;
  object *def = nil;
  bool bitp = false;
  int type = NUMBER ;
  object *dims = first(args);
  if (dims == NULL) error2(PSTR("dimensions can't be nil"));
  else if (atom(dims)) dims = cons(dims, NULL);
  args = cdr(args);
  while (args != NULL && cdr(args) != NULL) {
    object *var = first(args);
    if (isbuiltin(first(args), INITIALELEMENT)) def = second(args);
    else
      if (isbuiltin(first(args), ELEMENTTYPE))
       {
          object *typeobj = second(args) ;

          InitArray2() ;

          if( isbuiltin(typeobj, BIT)) { bitp = true; type = CHAR ; }
          else if( typeobj->name == array2_type_name[CHARACTER_ARRAY]) type = CHAR ;
          else if( typeobj->name == array2_type_name[INTEGER_ARRAY]) type = NUMBER ;
          else if( typeobj->name == array2_type_name[SINGLEFLOAT_ARRAY]) type = FLOAT ;
          else if( typeobj->name == array2_type_name[DOUBLEFLOAT_ARRAY]) type = FLOAT ;
      }
    else error(PSTR("argument not recognised"), var);
    args = cddr(args);
  }
  if (bitp) {
    if (def == nil) def = number(0);
    else def = number(-checkbitvalue(def)); // 1 becomes all ones
  }
  return makearray2(dims, def, bitp, type);
}

/*
  (aref* array index [index*])
  Returns an element from the specified array.
*/
object *fn_aref2 (object *args, object *env) {
  (void) env;
  int bit;
  object *array = first(args);
  if (!array2p(array))
      error(PSTR("first argument is not an array"), array);
  object *loc = myalloc(); //&((array_desc_t*)array->pointer)->bufferobj ;
  loc->type = ((array_desc_t*)array->pointer)->type ;
  switch(loc->type)
  {
  case FLOAT:
      loc->single_float = *(sfloat_t*)(*getarray2(array, cdr(args), 0, &bit));
      break ;
  case CHAR:
      loc->type = NUMBER ;
      loc->integer = *(char*)(*getarray2(array, cdr(args), 0, &bit)) & 0x0ff ;
      break ;
  default:
      loc->integer = *(long int*)(*getarray2(array, cdr(args), 0, &bit)) ;
      if(loc->integer==0x72)
      {
          bit = 0 ;
      }
  }

  return loc;
  //if (bit == -1) return loc;
  //else return number((loc->integer)>>bit & 1);
}



/*
  getarray - gets a pointer to an element in a multi-dimensional array, given a list of the subscripts subs
  If the first subscript is negative it's a bit array and bit is set to the bit number
*/
object **getarray2 (object *array, object *subs, object *env, int *bit) {
  int index = 0, size = 1, s;
  *bit = -1;
  bool bitp = false;
  array_desc_t *descriptor = (array_desc_t*)array->pointer ;
  int *dimstep = (int*)descriptor->dimstep;
  int n = descriptor->ndim ;
  while (n != 0 && subs != NULL) {
    int d = *dimstep;
    //if (d < 0) { d = -d; bitp = true; }
    if (env) s = checkinteger(eval(car(subs), env)); else s = checkinteger(car(subs));
    if (s < 0 || s >= d) error(PSTR("subscript out of range"), car(subs));
    size = d;
    index += s*d;
    n--;
    dimstep++;
    subs = cdr(subs);
  }
  if (n != 0) error2(PSTR("too few subscripts"));
  if (subs != NULL) error2(PSTR("too many subscripts"));
  /*if (bitp) {
    size = (size + sizeof(int)*8 - 1)/(sizeof(int)*8);
    *bit = index & (sizeof(int)==4 ? 0x1F : 0x0F);
    index = index>>(sizeof(int)==4 ? 5 : 4);
  }*/

  return arrayref2(array, index, size);
}




//extern volatile int isAref ;

/*
  arrayref - returns a pointer to the element specified by index in the array of size s
*/
object **arrayref2 (object *array, int index, int size) {

/*  object **p = &car(cdr(array));
  while (mask) {
    if ((index & mask) == 0) p = &(car(*p)); else p = &(cdr(*p));
    mask = mask>>1;
  }*/
    //int i ;
    //object *ret ; //= myalloc(); //= car(cdr(array));
    array_desc_t *descriptor = (array_desc_t*)array->pointer ;
    object **p = &descriptor->obj_pointer ; //&car(cdr(array));

    //ret->type = descriptor->type ;
    uintptr_t array_pointer = (uintptr_t)(array->pointer) + sizeof(array_desc_t);
    char *Ptr = (char*)(array_pointer) ;

    //printf("array %u\n", Ptr); fflush(stdout);
    *p =(object *)&Ptr[index*descriptor->element_size] ;

  return p;
}


/*
  (arrayp item)
  Returns t if its argument is an array.
*/
object *fn_array2p (object *args, object *env) {
  (void) env;
  return array2p(first(args)) ? tee : nil;
}




/*
  delarray
*/
void delarray2 (object *array) {
  array_desc_t *descriptor = (array_desc_t*)array->pointer ;
  descriptor->ndim = 0;
  descriptor->dim[0] = 0 ;
  descriptor->size = 0 ;
  descriptor->element_size = 0 ;
  free(descriptor) ;
  array->pointer = (void*)NULL ;
  array->type = SYMBOL ;
}


object *fn_delarray2p (object *args, object *env) {
  (void) env;
    object *arg = first(args) ;

    if(arg)
    {
        if(array2p(arg))
        {
            delarray2(arg) ;
        }
    }
  return array2p(first(args)) ? tee : nil;
}



/*
  pslice2 - prints a slice of an array recursively
*/
void pslice2 (object *array, int size, int slice, int32_t *dim, int32_t *dimstep, int n, pfun_t pfun, bool bitp) {
  extern void myfree (object *obj) ;

  bool spaces = true;
  if (slice == -1) { spaces = false; slice = 0; }
  int d = *dim ;
  int ds = *dimstep ;
  if (d < 0) d = -d;
  for (int i = 0; i < d; i++) {
    if (i && spaces) pfun(' ');
    int index = slice + ds * i;
    if (n == 1)
    {
      /*if (bitp) pint(((*arrayref2(array, index>>(sizeof(int)==4 ? 5 : 4), size))->integer)>>
        (index & (sizeof(int)==4 ? 0x1F : 0x0F)) & 1, pfun);
      else*/
        array_desc_t *descriptor = (array_desc_t*)array->pointer ;
        object *obj = myalloc() ;
        obj->type = descriptor->type ;

        switch(obj->type)
        {
        case FLOAT:
            obj->single_float = *(sfloat_t*)(*arrayref2(array, index, size));
            break ;
        case CHAR:
            obj->type = NUMBER ;
            obj->integer = *(char*)(*arrayref2(array, index, size)) & 0x0ff ;
            break ;
        default:
            obj->integer = *(long int*)(*arrayref2(array, index, size)) ;
        }
        //*arrayref2(array, index, size)
        printobject(obj, pfun);
        myfree(obj);
    } else { pfun('(');  pslice2(array, size, index, &dim[1], &dimstep[1], n-1, pfun, bitp); pfun(')'); }
  }
}


/*
  printarray2 - prints an array2 in the appropriate Lisp format
*/
void printarray2 (object *array, pfun_t pfun) {
  array_desc_t *descriptor = (array_desc_t*)array->pointer ;
  bool bitp = false;
  int size =  descriptor->size, n = descriptor->ndim;

  //if (bitp) size = (size + sizeof(int)*8 - 1)/(sizeof(int)*8);
  pfun('#');
  /*if (n == 1 && bitp) { pfun('*'); pslice(array, size, -1, dimensions, pfun, bitp); }
  else */{
    if (n > 1) { pint(n, pfun); pfun('A'); }
    pfun('('); pslice2(array, size, 0, descriptor->dim, descriptor->dimstep, n, pfun, bitp); pfun(')');
  }
}


object *array2dimensions(object *array)
{
    array_desc_t *descriptor = (array_desc_t*)array->pointer ;
    int i, j;

    j = descriptor->dim[0] ;
    object *num = number(j);

    object *dimensions = cons(num, NULL);
    object *ptr = dimensions ;

    for(i=1;i<descriptor->ndim;i++){
        j = descriptor->dim[i] ;
        num = number(j);
        cdr(ptr) = cons(num, NULL);
        ptr = cdr(ptr);
    }
    return dimensions ;
}


void array2info (object *array) {
  array_desc_t *descriptor = (array_desc_t*)array->pointer ;
  printf("ndim   =%d \n", descriptor->ndim);
  if((descriptor->ndim>0)&&(descriptor->ndim<10))
    for(int i=0;i<descriptor->ndim;i++) printf(" %d", descriptor->dim[i]);
  printf("\nsize   =%d \n", (int)descriptor->size) ;
  printf("elsize =%d \n", descriptor->element_size);
  printf("type   =%d \n", (int)descriptor->type) ;
  fflush(stdout);
}



char *tcp_getimage(int x,int y,int w,int h) ;
int tcp_putimage(int x, int y, char  *array ) ;


object *fn_getimage(object *args, object *env)
{
  (void) env;

  int x=0, y=0, h=0, w=0 ;
  object *obj = args ;

  if(obj) { x = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { y = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { w = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { h = checkinteger(car(obj)) ; };

  char *cPtr = tcp_getimage(x,y,w,h) ;

  if(!cPtr) return nil ;

  object *ptr = myalloc();
  ptr->type = ARRAY2;
  ptr->pointer = cPtr ;

return ptr ;
}




object *fn_putimage(object *args, object *env)
{
  (void) env;

  int x=0, y=0, sz = 0 ;
  object *obj = args ;
  object *a ;

  if(obj) { x = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { y = checkinteger(car(obj)) ; obj = cdr(obj); };
  if(obj) { a = car(obj) ; };

  if(a->type!= ARRAY2) {
      pfstring("\nError: argument must be array2-type", pserial);
      return nil ;
  }
  //array_desc_t *desctriptor ;
  int n = tcp_putimage(x,y, (char*)(a->pointer) ) ;

    if(n==0) return nil ;

return tee ;
}




WORD gettrueWORD(WORD val)
{
    WORD ret ;
    char *b, *c ;

    b = (char*)&val ;
    c = (char*)&ret ;

    c[0] = b[1] ;
    c[1] = b[0] ;

    return ret ;
}

DWORD gettrueDWORD(DWORD val)
{
    WORD ret ;
    char *b, *c ;

    b = (char*)&val ;
    c = (char*)&ret ;

    c[0] = b[2] ;
    c[1] = b[3] ;
    c[2] = b[0] ;
    c[3] = b[1] ;

    return ret ;
}

LONG gettrueLONG(LONG val)
{
    LONG ret ;
    char *b, *c ;

    b = (char*)&val ;
    c = (char*)&ret ;

    c[0] = b[3] ;
    c[1] = b[2] ;
    c[2] = b[1] ;
    c[3] = b[0] ;

    return ret ;
}




object *fn_loadbitmap(object *args, object *env)
{
  (void) env;

    FILE *fp ;
    //BITMAPFILEHEADER filedesc ;
    BITMAPINFOHEADER bitmapdesc ;

    if(!stringp(car(args)) )
    {
          pfstring("\nError: file name must be string\n", pserial);
          return nil ;
    }

    char fname[256] ;

    cstring(car(args), fname, 256) ;

    fp = fopen(fname, "rb") ;

    if(!fp) {
          pfstring("\nError: cannot open file.\n", pserial);
          return nil ; }

    char fdescbuffer[14] ;
    fread(fdescbuffer, 1, 14, fp);

    WORD ftype = (*(WORD*)(&fdescbuffer[0])) ;
    DWORD pixelsbgn = (*(DWORD*)(&fdescbuffer[10])) ;
    DWORD filesize = (*(DWORD*)(&fdescbuffer[2])) ;
    //memcpy(fdescbuffer,(char*)&filedesc,  14);

    if(ftype!=0x4d42) {
          pfstring("\nError: it is not bitmap file.\n", pserial);
          return nil ; }

    fread(&bitmapdesc, 40, 1, fp);

    array_desc_t desctriptor ;

    desctriptor.ndim = 3 ;
    desctriptor.dim[0] = bitmapdesc.biHeight ;
    desctriptor.dim[1] = bitmapdesc.biWidth ;
    desctriptor.dim[2] = 4 ;
    desctriptor.size = desctriptor.dim[2]
            * desctriptor.dim[0] * desctriptor.dim[1] ;
    desctriptor.type = CHAR ;
    desctriptor.element_size = 1 ;

    filldimensionsteps(&desctriptor) ;

    if((bitmapdesc.biBitCount<4)||(bitmapdesc.biBitCount>32)){
          pfstring("\nError: unkown bitmap pixel format.\n", pserial);
          return nil ; }

    if(bitmapdesc.biCompression!=0){
        pfstring("\nError: compressed bitmaps not supported.\n", pserial);
        return nil ; }



    char *cPtr = (char*)malloc(desctriptor.size*desctriptor.element_size + sizeof(array_desc_t)) ;

    if(!cPtr){
        pfstring("\nError: cannot to create bitmap array.\n", pserial);
        return nil ; }

    *(array_desc_t*)(cPtr) = desctriptor ;

  object *ptr = myalloc();
  ptr->type = ARRAY2;
  ptr->pointer = cPtr ;

  char *cPtrData = cPtr + sizeof(array_desc_t) ;

    int iPixelCount = 0;

  if(bitmapdesc.biBitCount == 16)
  {
      int i, j ;
      uint16_t uiPixel ;

      //fseek(fp, gettrueDWORD(filedesc.bfOffBits) , SEEK_SET) ;
      int w = desctriptor.dim[1] ;
      int h = desctriptor.dim[0] ;

      for(i=0;i<h;i++)
          for(j=0;j<w;j+=2)
      {
          int k = (h-i-1) * w + j ;
          fread(&uiPixel, 1, 2, fp) ;

          uint32_t value = 0 ;
          value = (uiPixel & 0x1f)<<3 ;
          value |= ((uiPixel>>5) & 0x1f)<<3 ;
          value |= ((uiPixel>>10) & 0x1f)<<3 ;
          ((uint32_t*)cPtrData)[k] = value ;
          cPtrData += 4 ;
      }
  }
  else if(bitmapdesc.biBitCount == 24)
  {
      int i, j ;
      char uiPixel[4] ;

      //fseek(fp, pixelsbgn , SEEK_SET) ;
      int w = desctriptor.dim[1] ;
      int h = desctriptor.dim[0] ;

      for(i=0;i<h;i++)
          for(j=0;j<w;j+=2)
      {
          int k = (h-i-1) * w + j ;
          fread(uiPixel, 1, 3, fp) ;

          uint32_t value = 0 ;
          value = uiPixel[0] ;
          value |= uiPixel[1]<<8 ;
          value |= uiPixel[2]<<16 ;
          ((uint32_t*)cPtrData)[k] = value ;
          cPtrData += 4 ;
      }
  }
  else if(bitmapdesc.biBitCount == 32)
  {
      int i, j ;
      char uiPixel[4] ;

      //fseek(fp, pixelsbgn , SEEK_SET) ;

      int w = desctriptor.dim[1] ;
      int h = desctriptor.dim[0] ;

      for(i=0;i<h;i++)
          for(j=0;j<w;j+=2)
      {
          int k = (h-i-1) * w + j ;
          fread(uiPixel, 1, 4, fp) ;

          ((uint32_t*)cPtrData)[k] = *(uint32_t*)(uiPixel) ;
          cPtrData += 4 ;
      }
  }
  else if(bitmapdesc.biBitCount == 4)
  {
      int i, j ;
      char uiPixel[4] ;
      //int NumColors = 16 ;
      uint32_t Colors[16] ;

      fread(Colors, 1, sizeof(Colors),fp) ;

      //fseek(fp, pixelsbgn , SEEK_SET) ;

      int w = desctriptor.dim[1] ;
      int h = desctriptor.dim[0] ;

      for(i=0;i<h;i++)
          for(j=0;j<w;j+=2)
      {
          int k = (h-i-1) * w + j ;

          fread(uiPixel, 1, 1, fp) ;
          ((uint32_t*)cPtrData)[k] = Colors[(uiPixel[0]>>4) & 0xf] ;
          //cPtrData += 4;
          ((uint32_t*)cPtrData)[k+1] = Colors[uiPixel[0] & 0xf] ;
          //cPtrData += 4;
          iPixelCount += 2 ;
      }
  }
  else if(bitmapdesc.biBitCount == 8)
  {
      int i, j ;
      unsigned char uiPixel[4] ;
      //int NumColors = 256 ;
      uint32_t Colors[256] ;

      fread(Colors, 1, sizeof(Colors),fp) ;

      //fseek(fp, pixelsbgn , SEEK_SET) ;

      int w = desctriptor.dim[1] ;
      int h = desctriptor.dim[0] ;

      for(i=0;i<h;i++)
          for(j=0;j<w;j++)
      {
          int k = (h-i-1) * w + j ;

          fread(uiPixel, 1, 1, fp) ;
          ((uint32_t*)cPtrData)[k] = Colors[uiPixel[0] & 0xf] ;
          iPixelCount += 1 ;
      }
  }

  fclose(fp) ;

  //array2info(ptr) ;

return ptr ;
}








object *fn_savebitmap(object *args, object *env)
{
  (void) env;

    FILE *fp ;
    //BITMAPFILEHEADER filedesc ;
    BITMAPINFOHEADER bitmapdesc = { 0 } ;

    int x=0, y=0, h=0, w=0 ;
    object *obj = args ;

    object *array = car(obj) ;

    if(!array2p(array))
    {
          pfstring("\nError: file name must be string\n", pserial);
          return nil ;
    }

    obj = cdr(obj);

    if(!stringp(car(args)) )
    {
          pfstring("\nError: file name must be string\n", pserial);
          return nil ;
    }


    if(!stringp(car(args)) )
    {
          pfstring("\nError: file name must be string\n", pserial);
          return nil ;
    }

    char fname[256] ;

    cstring(car(args), fname, 256) ;

    fp = fopen(fname, "wb") ;

    if(!fp) {
          pfstring("\nError: cannot open file.\n", pserial);
          return nil ; }

    array_desc_t desctriptor = *(array_desc_t*)array->pointer ;
    char *cPtrData = (char*)(array->pointer) + sizeof(array_desc_t) ;

    desctriptor.ndim = 3 ;
    bitmapdesc.biHeight = desctriptor.dim[0] ;
    bitmapdesc.biWidth = desctriptor.dim[1] ;
    bitmapdesc.biCompression = 0 ;
    bitmapdesc.biPlanes = 1 ;
    bitmapdesc.biSize = 0 ;

    char fdescbuffer[14] ;

    WORD *ftype = (WORD*)(&fdescbuffer[0]) ;
    DWORD *pixelsbgn = (DWORD*)(&fdescbuffer[10]) ;
    DWORD *filesize = (DWORD*)(&fdescbuffer[2]) ;

    *ftype=0x4d42 ;

    int NumColors = 0 ;
    uint32_t Colors[257] ;


    //*pixelsbgn =
    int i, j ;
    uint32_t uiPixel ;

    for(i=0;i<desctriptor.dim[0];i++)
        for(j=0;j<desctriptor.dim[1];j++)
    {
        uiPixel = *(uint32_t*)cPtrData ;

        for(int k=0;k<NumColors;k++)
        {
            if(Colors[k] != uiPixel)
            {
                Colors[NumColors] = uiPixel ;
                NumColors++ ;
            }
            if(NumColors > 256) break ;
        }
        cPtrData += 4 ;
    }

    if(NumColors > 256) {
        bitmapdesc.biBitCount = 24 ; bitmapdesc.biClrUsed = 0 ;
        bitmapdesc.biClrImportant = 0 ; }
    else if(NumColors <= 16) {
        bitmapdesc.biBitCount = 4 ;  bitmapdesc.biClrUsed = 16 ; }
    else {
        bitmapdesc.biBitCount = 8 ;  bitmapdesc.biClrUsed = 256 ; }

    bitmapdesc.biClrImportant = NumColors ;

    *pixelsbgn = bitmapdesc.biClrUsed*4 + 40 + 14 ;

    fwrite(fdescbuffer, 1, 14, fp);
    fwrite(&bitmapdesc, 1, 40, fp);
    if(bitmapdesc.biBitCount <= 8)
        fwrite(&Colors, 1, (uint32_t)bitmapdesc.biClrUsed, fp);


    char *cPtr = cPtrData ;

    if(bitmapdesc.biBitCount == 24)
    {
        int i, j ;
        char uiPixel[4] ;
        int w = desctriptor.dim[1] ;
        int h = desctriptor.dim[0] ;

        for(i=0;i<h;i++)
            for(j=0;j<w;j+=2)
        {
            int k = (h-i-1) * w + j ;

            uint32_t value = ((uint32_t*)cPtr)[k] ;

            uiPixel[0] = value & 0xff ;
            uiPixel[1] = (value >>8) & 0xff ;
            uiPixel[2] = (value >>16) & 0xff ;
            cPtr += 4 ;

            fwrite(uiPixel, 1, 3, fp) ;
        }
    }
    else if(bitmapdesc.biBitCount == 4)
    {
        int i, j ;
        char uiPixel ;
        uint8_t iColorIndex ;
        int w = desctriptor.dim[1] ;
        int h = desctriptor.dim[0] ;

        for(i=0;i<h;i++)
            for(j=0;j<w;j+=2)
        {
            int k = (h-i-1) * w + j ;

            uint32_t value = ((uint32_t*)cPtr)[k] ;
            iColorIndex=0;
            while((iColorIndex<NumColors)&&(Colors[iColorIndex]!=value)) iColorIndex++ ;
            uiPixel = iColorIndex & 0x0f ;
            cPtr += 4 ;

            value = ((uint32_t*)cPtr)[k+1] ;
            iColorIndex=0;
            while((iColorIndex<NumColors)&&(Colors[iColorIndex]!=value)) iColorIndex++ ;
            uiPixel |= (iColorIndex & 0x0f)<<4 ;
            cPtr += 4 ;

            fwrite(&uiPixel, 1, 1, fp) ;
        }
    }
    else if(bitmapdesc.biBitCount == 8)
    {
        int i, j ;
        uint8_t uiPixel ;
        int iColorIndex = 0 ;
        int w = desctriptor.dim[1] ;
        int h = desctriptor.dim[0] ;

        for(i=0;i<h;i++)
            for(j=0;j<w;j++)
        {
            int k = (h-i-1) * w + j ;

            uint32_t value = ((uint32_t*)cPtr)[k] ;
            iColorIndex=0;
            while((iColorIndex<NumColors)&&(Colors[iColorIndex]!=value)) iColorIndex++ ;
            uiPixel = iColorIndex & 0xff ;
            cPtr += 4 ;

            fwrite(&uiPixel, 1, 1, fp) ;
        }
    }

    fclose(fp) ;

    return tee ;
}
