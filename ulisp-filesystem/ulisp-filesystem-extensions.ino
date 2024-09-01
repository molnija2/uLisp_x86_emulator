/*
 User Extensions
*/

// Definitions




/*
 *   File search using '*'-type patterns
*/
int fillpattern(char *mask, char *pattern)
{
    int i = 0 ;
    if(*mask==0) return -1 ;
    while((*mask!=0)&&(*mask!='*'))
    {
        *pattern++ = *mask++ ;
        i++ ;
    }
    *pattern = 0 ;

    return i ;
}

int findpattern(char *pattern, char *name)
{
    int i = 0 , lenp, lenn ;
    if(*pattern==0) return -1 ;
    lenp = strlen(pattern) ;
    lenn = strlen(name) ;
    while(lenp<=lenn)
    {
        if(strncmp(name,pattern,lenp)==0) return i;
        name++ ;
        lenn-- ;
        i++ ;
    }
    return -1 ;
}




int selection(char *name, char *filemask )
{
    char file_pattern[256] ;
    int i;
    int imaskpos = 0, inamepos = 0 ;
    i = fillpattern(&filemask[imaskpos], file_pattern);
    if(i==-1) return 1 ;
    if(i>0)
    {
        if(strncmp(&file_pattern[imaskpos],&name[inamepos],i)!=0) return 0 ;
    }

    imaskpos += i+1 ;
    inamepos += i ;
    do{
        i = fillpattern(&filemask[imaskpos], file_pattern);
        if(i <= 0) return 1 ;
        inamepos = findpattern(file_pattern, &name[inamepos]);
        if(inamepos<0) return 0 ;
        inamepos += i ;
        imaskpos += i+1 ;
    }while(1) ;
return 0;
}

/*
  (directory [pattern])
  Returns a list of the filenames of the files on the SD card.
  Pattern is string which contains '*' symbols.
*/
//  (directory "/home/*/")  - search directories 
//  (directory "/home/*")  ("/home/*.*") ("/home/*.txt") search files 

object *fn_directory (object *args, object *env) {
  (void) env;
#if defined(sdcardsupport)
  int type = 0x4 | 0x8 ;  // Files and directories
  char pattern_string[256] = "*" ;
  char dirname_string[256] = "/";

  if (args != NULL)
  {   //  Directory name
      if(stringp(car(args)))
      {
        cstring(car(args), dirname_string, 256) ;
        if(dirname_string[strlen(dirname_string)-1] == '/')
        {
            dirname_string[strlen(dirname_string)-1] = 0x0 ;
            type = 0x4 ;
        }
        else type = 0x8 ;

        char *pattern_bgn = strchr(dirname_string,'*') ;
        if(pattern_bgn)
        {   // There is pattern string with '*'-symbols
            while((pattern_bgn!=dirname_string)&&(*pattern_bgn!='/')) pattern_bgn -- ;
            if(*pattern_bgn=='/')
            {
                *pattern_bgn = 0x0 ;
                pattern_bgn ++ ;
            }
            strcpy(pattern_string, pattern_bgn);
            if(!(*dirname_string))
            {
                strcpy(dirname_string, "/"); // Dir name "/" restore
            }
        }
      }
  }

  object *result = cons(NULL, NULL);
  object *ptr = result;

  SD.begin(SDCARD_SS_PIN);
  File root = SD.open(dirname_string);
  if (!root){  pfstring("problem reading from SD card", pserial); return nil; }

  while (true) {
      File entry = root.openNextFile();
      if(!entry) break;

      if( (entry.isDirectory() && (type&0x4)) || (!entry.isDirectory() && (type&0x8)) )
         if(selection((char*)entry.name(), pattern_string ))
      {
        object *filename = lispstring((char*)entry.name());
        cdr(ptr) = cons(filename, NULL);
        ptr = cdr(ptr);
      }
  };

  root.close();

  return cdr(result);
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
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

const char string_probefile[] PROGMEM = "probe-file";
const char string_deletefile[] PROGMEM = "delete-file";
const char string_renamefile[] PROGMEM = "rename-file";
const char string_copyfile[] PROGMEM = "copy-file";
const char string_ensuredirectoriesexist[] PROGMEM = "ensure-directories-exist";



// Documentation strings

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
