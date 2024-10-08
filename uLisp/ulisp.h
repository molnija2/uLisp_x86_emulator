#ifndef ___ULISP_H___
#define ___ULISP_H___

#include <inttypes.h>
#include <stdbool.h>

#include <dirent.h>
#include <string.h>

#define lisplibrary


// Compile options

// #define resetautorun
#define printfreespace
// #define printgcs
#define sdcardsupport
#define gfxsupport
#define touchscreen_support

#define tcp_keyboard
#define tcp_stdout
//#define lisplibrary

//#define assemblerlist
//#define lineeditor
#define vt100
#define extensions

#define BOARD_HAS_PSRAM

#define LINUX_X64

// Includes

// #include "LispLibrary.h"

#include <limits.h>


#define SDSIZE 100


// Platform specific settings

#define WORDALIGNED __attribute__((aligned (4)))
#define BUFFERSIZE 36  // Number of bits+4
#define MEMBANK


#define WORKSPACESIZE (4096*8)     /* Objects (8*bytes) */
#define EEPROMFLASH
#define FLASHSIZE 32768                 /* Bytes */
#define CODESIZE 128                    /* Bytes */
#define STACKDIFF 320

#define gfxsupport




// Typedefs

typedef uint32_t chars_t;
typedef uint32_t builtin_t;


#ifdef LINUX_X64

typedef uint64_t symbol_t;

typedef int64_t integer_t;
typedef double sfloat_t;


typedef struct sobject {
  union
  {
    struct {
      sobject *car;
      sobject *cdr;
    };
    struct {
      //unsigned long int type;
      uintptr_t  type ;
      union {
        void*  pointer ;
        symbol_t name;
        long int integer;
        long int chars; // For strings
        sfloat_t single_float;
      };
    };
  };
} object;
#else

typedef uint64_t symbol_t;
typedef float sfloat_t;

typedef struct sobject {
  union {
    struct {
      sobject *car;
      sobject *cdr;
    };
    struct {
      unsigned int type;
      union {
        symbol_t name;
        int integer;
        chars_t chars; // For strings
        float single_float;
      };
    };
  };
} object;

#endif




typedef object *(*fn_ptr_type)(object *, object *);
typedef void (*mapfun_t)(object *, object **);
typedef int (*intfn_ptr_type)(int w, int x, int y, int z);
object *eval (object *form, object *env) ;


typedef struct {
  const char *string;
  fn_ptr_type fptr;
  uint8_t minmax;
  const char *doc;
} tbl_entry_t;



typedef int (*gfun_t)();
typedef void (*pfun_t)(char);



/*enum builtins: builtin_t { NIL, TEE, NOTHING, OPTIONAL, INITIALELEMENT, ELEMENTTYPE, BIT, AMPREST, LAMBDA, LET, LETSTAR,
CLOSURE, PSTAR, QUOTE, DEFUN, DEFVAR, DEFCODE, CAR, FIRST, CDR, REST, NTH, AREF, STRINGFN, PINMODE,
DIGITALWRITE, ANALOGREAD, ANALOGREFERENCE, REGISTER, FORMAT , TEST, EQ,
 };
*/
enum builtins: builtin_t { NIL, TEE, NOTHING, OPTIONAL, FEATURES, INITIALELEMENT, ELEMENTTYPE, TEST, BIT, AMPREST,
LAMBDA, LET, LETSTAR, CLOSURE, PSTAR, QUOTE, DEFUN, DEFVAR, EQ, CAR, FIRST, CDR, REST, NTH, AREF, CHAR,
STRINGFN, PINMODE, DIGITALWRITE, ANALOGREAD, REGISTER, FORMAT //, AREF2
 };


#define HIGH    1
#define LOW     0

#define  INPUT  0
#define  OUTPUT  1
#define INPUT_PULLDOWN  2
#define INPUT_PULLUP    3
 

#define LED_BUILTIN 13


// C Macros

#define nil                NULL
#define car(x)             (((object *) (x))->car)
#define cdr(x)             (((object *) (x))->cdr)

#define first(x)           (((object *) (x))->car)
#define second(x)          (car(cdr(x)))
#define cddr(x)            (cdr(cdr(x)))
#define third(x)           (car(cdr(cdr(x))))

#define push(x, y)         ((y) = cons((x),(y)))
#define pop(y)             ((y) = cdr(y))

#define protect(y)         push((y), GCStack)
#define unprotect()        pop(GCStack)

#define integerp(x)        ((x) != NULL && (x)->type == NUMBER)
#define floatp(x)          ((x) != NULL && (x)->type == FLOAT)
#define symbolp(x)         ((x) != NULL && (x)->type == SYMBOL)
#define stringp(x)         ((x) != NULL && (x)->type == STRING)
#define characterp(x)      ((x) != NULL && (x)->type == CHARACTER)
#define arrayp(x)          ((x) != NULL && (x)->type == ARRAY)
#define array2p(x)          ((x) != NULL && (x)->type == ARRAY2)
#define streamp(x)         ((x) != NULL && (x)->type == STREAM)

#define mark(x)            (car(x) = (object *)(((uintptr_t)(car(x))) | MARKBIT))
#define unmark(x)          (car(x) = (object *)(((uintptr_t)(car(x))) & ~MARKBIT))
#define marked(x)          ((((uintptr_t)(car(x))) & MARKBIT) != 0)
#define MARKBIT            1

#define setflag(x)         (Flags = Flags | 1<<(x))
#define clrflag(x)         (Flags = Flags & ~(1<<(x)))
#define tstflag(x)         (Flags & (1<<(x)))

#define issp(x)            (x == ' ' || x == '\n' || x == '\r' || x == '\t')
#define isbr(x)            (x == ')' || x == '(' || x == '"' || x == '#')
#define fntypef(x)          (getminmax((uint16_t)(x))>>6)  //AR2  old:(getminmax((uint8_t)(x))>>6)
#define longsymbolp(x)     (((x)->name & 0x03) == 0)
#define longnamep(x)       (((x) & 0x03) == 0)
#define twist(x)           ((uint32_t)((x)<<2) | (((x) & 0xC0000000)>>30))
#define untwist(x)         (((x)>>2 & 0x3FFFFFFF) | ((x) & 0x03)<<30)
#define arraysize(x)       (sizeof(x) / sizeof(x[0]))
//#define PACKEDS            0x43238000
//#define BUILTINS           0xF4240000
#define PACKEDS            0x43238000
#define BUILTINS           0xF4240000
#define ENDFUNCTIONS       1536

// Code marker stores start and end of code block
#define startblock(x)      ((x->integer) & 0xFFFF)
#define endblock(x)        ((x->integer) >> 16 & 0xFFFF)

// Constants

#define TRACEMAX  3 // Number of traced functions


enum type { ZZERO=0, SYMBOL=2, CODE=4, NUMBER=6, STREAM=8, CHARACTER=10, FLOAT=12, ARRAY=14, ARRAY2=16 /*AR2*/,
            STRING=18, PAIR=20 };  // ARRAY STRING and PAIR must be last
enum token { UNUSED, BRA, KET, QUO, DOT };
enum stream { SERIALSTREAM, I2CSTREAM, SPISTREAM, SDSTREAM, WIFISTREAM, STRINGSTREAM, GFXSTREAM };
enum fntypes_t { OTHER_FORMS, TAIL_FORMS, FUNCTIONS, SPECIAL_FORMS };



typedef   const char*  PGM_P ;
#define   PSTR (char*)




char *lookupdoc (builtin_t name) ;
tbl_entry_t *table (int n) ;
void printsymbol (object *form, pfun_t pfun);
bool findsubstring (char *part, builtin_t name);
void pbuiltin (builtin_t name, pfun_t pfun);
object *tf_progn (object *args, object *env);
int gserial ();
inline void spiwrite(char c) { c = c ; return  ; } ;
inline void i2cwrite(char c) { c = c ; return  ; } ;
gfun_t gstreamfun(object *args);
void pintbase (uint32_t i, uint8_t base, pfun_t pfun);
int subwidthlist (object *form, int w);
void supersub (object *form, int lm, int super, pfun_t pfun);
void pfl (pfun_t pfun);
void printhex4 (int i, pfun_t pfun);
void testescape ();
void repl (object *env);
object *fn_princtostring (object *args, object *env);
int glibrary ();
object *fn_analogwriteresolution (object *args, object *env);
symbol_t sym (builtin_t x);
object *intern (symbol_t name);
bool listp (object *x) ;
object *apply (object *function, object *args, object *env);

void printobject (object *form, pfun_t pfun) ;
void printstring (object *form, pfun_t pfun);
void indent (uint8_t spaces, char ch, pfun_t pfun);
uint8_t nthchar (object *string, int n);
void pint (int i, pfun_t pfun);
object *lispstring (char *s);
void pserial (char c) ;
object *read_r (gfun_t gfun) ;
inline void pln (pfun_t pfun) ;
void psymbol(symbol_t s, pfun_t pfun);

void PrintToucscreenParameters();
object *fn_touch_press (object *args, object *env);
object *fn_touch_x (object *args, object *env);
object *fn_touch_y (object *args, object *env);
object *fn_touch_printcal (object *args, object *env);
object *fn_touch_setcal(object *args, object *env);
object *fn_touch_calibrate(object *args, object *env);

object *fn_quit (object *args, object *env);

object *fn_copylist (object *args, object *env) ;
object *fn_stringnoteq (object *args, object *env) ;
object *fn_stringlesseq (object *args, object *env) ;
object *fn_stringgreatereq (object *args, object *env) ;
object *fn_return (object *args, object *env) ;

int listlength (object *list);
void checkminmax (builtin_t name, int nargs);
object *number (int n) ;
int checkinteger (object *obj) ;
void error2 (PGM_P string) ;
object *cons (object *arg1, object *arg2);
tbl_entry_t *table (int n) ;
unsigned int tablesize (int n);
object *myalloc ();
object *buildarray (int n, int s, object *def);
int nextpower2 (int n) ;
void error (PGM_P string, object *symbol);
object **getarray (object *array, object *subs, object *env, int *bit);
object **arrayref (object *array, int index, int size);
int checkbitvalue (object *obj);
int isbuiltin (object *obj, builtin_t n);
bool consp (object *x);
void pslice (object *array, int size, int slice, object *dims, pfun_t pfun, bool bitp) ;
void pfstring (const char *s, pfun_t pfun) ;
char *cstring (object *form, char *buffer, int buflen);
void ulispreset ();

object **getarray2 (object *array, object *subs, object *env, int *bit) ;
object **arrayref2 (object *array, int index, int size) ;
object *fn_makearray2 (object *args, object *env);
object *fn_aref2 (object *args, object *env);
void delarray2 (object *array) ;
object *fn_array2p (object *args, object *env) ;
object *fn_delarray2 (object *args, object *env) ;
void printarray2 (object *array, pfun_t pfun);
builtin_t lookupbuiltin (char* c);
void array2info (object *array) ;
void InitArray2();
object *array2dimensions(object *array);
int32_t array2_lenght(object *arg) ;

int fillpattern(char *mask, char *pattern);
int findpattern(char *pattern, char *name);
int selection(char *name, char *filemask );
object *fn_probefile (object *args, object *env);
object *fn_deletefile (object *args, object *env);
object *fn_renamefile (object *args, object *env);
object *fn_copyfile (object *args, object *env);
object *fn_ensuredirectoriesexist(object *args, object *env);
object *fn_deletedir (object *args, object *env);
object *fn_uiopchdir(object *args, object *env);
object *fn_uiopgetcwd(object *args, object *env);


object *fn_putimage(object *args, object *env) ;
object *fn_getimage(object *args, object *env) ;
object *fn_loadbitmap(object *args, object *env) ;
object *fn_savebitmap(object *args, object *env) ;
object *fn_drawbitmap(object *args, object *env) ;
object *fn_imagewidth(object *args, object *env) ;
object *fn_imageheight(object *args, object *env) ;

object *fn_getfontheight (object *args, object *env) ;
object *fn_gettextwidth (object *args, object *env) ;


#endif //___ULISP_H___
