#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <stdlib.h>


#include "ulisp.h"





/*object *buildarray2(int n, object *def);
object **getarray2(object *array, object *subs, object *env, int *bit) ;
object **arrayref2(object *array, int index, int size);*/

#define atom(x) (!consp(x))
extern object *tee;


enum { CHARACTER_ARRAY, SINGLEFLOAT_ARRAY, DOUBLEFLOAT_ARRAY, INTEGER_ARRAY };


symbol_t array2_type_name[8] = {0,0,0,0,0,0,0,0}, Aref2_name ;


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
  (aref array index [index*])
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
  int *dim = (int*)descriptor->dim;
  int n = descriptor->ndim ;
  while (n != 0 && subs != NULL) {
    int d = *dim;
    //if (d < 0) { d = -d; bitp = true; }
    if (env) s = checkinteger(eval(car(subs), env)); else s = checkinteger(car(subs));
    if (s < 0 || s >= d) error(PSTR("subscript out of range"), car(subs));
    size = size * d;
    index = index * d + s;
    n--;
    dim++;
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
void pslice2 (object *array, int size, int slice, int *dim, int n, pfun_t pfun, bool bitp) {
  bool spaces = true;
  if (slice == -1) { spaces = false; slice = 0; }
  int d = *dim ;
  if (d < 0) d = -d;
  for (int i = 0; i < d; i++) {
    if (i && spaces) pfun(' ');
    int index = slice * d + i;
    if (n == 1)
    {
      /*if (bitp) pint(((*arrayref2(array, index>>(sizeof(int)==4 ? 5 : 4), size))->integer)>>
        (index & (sizeof(int)==4 ? 0x1F : 0x0F)) & 1, pfun);
      else*/
        printobject(*arrayref2(array, index, size), pfun);
    } else { pfun('(');  pslice2(array, size, index, &dim[1], n-1, pfun, bitp); pfun(')'); }
  }
}


/*
  printarray2 - prints an array2 in the appropriate Lisp format
*/
void printarray2 (object *array, pfun_t pfun) {
  array_desc_t *descriptor = (array_desc_t*)array->pointer ;
  bool bitp = false;
  int size =  descriptor->size, n = descriptor->ndim;

  /*if (bitp) size = (size + sizeof(int)*8 - 1)/(sizeof(int)*8);
  pfun('#');
  if (n == 1 && bitp) { pfun('*'); pslice(array, size, -1, dimensions, pfun, bitp); }
  else */{
    if (n > 1) { pint(n, pfun); pfun('A'); }
    pfun('('); pslice2(array, size, 0, descriptor->dim, n, pfun, bitp); pfun(')');
  }
}



void array2info (object *array) {
  array_desc_t *descriptor = (array_desc_t*)array->pointer ;
  printf("ndim   =%d \n", descriptor->ndim);
  printf("dim[0] =%d \n",descriptor->dim[0]);
  printf("size   =%d \n",descriptor->size) ;
  printf("elsize =%d \n",descriptor->element_size);
  printf("type   =%d \n",descriptor->type) ;
  fflush(stdout);
}
