#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include "ulisp.h"


#define atom(x) (!consp(x))
extern object *tee;





/*(probe-file pathspec)  tests whether a file exists.
Returns nil if there is no file named pathspec,
and otherwise returns the truename of pathspec.
*/
object *fn_probefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x08 | 0x04;  // Files and directories
  char pattern_string[256] = "*" ;
  char dirname_string[512] = "/";

    if(stringp(car(args)))
    {
        cstring(car(args), dirname_string, 256) ;
        if(dirname_string[strlen(dirname_string)-1] == '/')
        {
            dirname_string[strlen(dirname_string)-1] = 0x0 ;
        }

        char *pattern_bgn = &dirname_string[strlen(dirname_string)-1] ;
        while((pattern_bgn!=dirname_string)&&(*pattern_bgn!='/')) pattern_bgn -- ;

        if(*pattern_bgn=='/')
        {
           *pattern_bgn = 0x0 ;
           pattern_bgn ++ ;
           strcpy(pattern_string, pattern_bgn);
        }
        else
        {
            strcpy(pattern_string, dirname_string);
            getcwd(dirname_string, 256);
        }
    }



#ifdef LINUX_X64
  DIR *Dir;
  Dir=opendir(dirname_string);
  if(Dir==NULL){  pfstring("problem reading from SD card", pserial); return nil; }

#else
  SD.begin(SDCARD_SS_PIN);
  File root = SD.open(dirname_string);
  if (!root){  pfstring("problem reading from SD card", pserial); return nil; }
#endif

  while (true) {

#ifdef LINUX_X64
      struct dirent *Dirent = readdir(Dir);
      if(!Dirent)
          break;

      if((Dirent->d_type & type)
          &&(strcmp((char*)Dirent->d_name, pattern_string ))==0)
      {
          sprintf(pattern_string,"%s/%s", dirname_string, (char*)Dirent->d_name);
          object *filename = lispstring((char*)pattern_string);
          closedir(Dir);
          return filename;
      }
#else
      File entry = root.openNextFile();
      if(!entry) break;

      if( (entry.isDirectory() && (type&0x4)) || (!entry.isDirectory() && (type&0x8)) )
         if(strcmp((char*)entry->name(), pattern_string )==0)
      {
         sprintf(pattern_string,"%s//%s", dirname_string, (char*)Dirent->d_name);
         object *filename = lispstring((char*)pattern_string);
         root.close();
         return filename;
      }
#endif
  };

#ifdef LINUX_X64
  closedir(Dir);
#else
  root.close();
#endif

  return nil;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}



/* (delete-file pathspec)   delete specified file.
Returns true if success and otherwise returns nil.
*/
object *fn_deletefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x8 ;  // Files only
  char pattern_string[256] = "*" ;
  char dirname_string[256] = "/";

  if(stringp(car(args)))
  {
     cstring(car(args), dirname_string, 256) ;
     if(dirname_string[strlen(dirname_string)-1] == '/')
     {
         dirname_string[strlen(dirname_string)-1] = 0x0 ;
         type = 0x04 ; // Directories only
     }

     char *pattern_bgn = &dirname_string[strlen(dirname_string)-1] ;

     while((pattern_bgn!=dirname_string)&&(*pattern_bgn!='/')) pattern_bgn -- ;
     if(*pattern_bgn=='/')
     {
        pattern_bgn ++ ;
        strcpy(pattern_string, pattern_bgn);
        *pattern_bgn = 0x0 ;
     }
     else
     {
         strcpy(pattern_string, dirname_string);
         getcwd(dirname_string, 256); // Current Dir name
     }
  }


#ifdef LINUX_X64
  DIR *Dir;
  Dir=opendir(dirname_string);
  if(Dir==NULL){  pfstring("problem reading directory from SD card", pserial); return nil; }

#else
  SD.begin(SDCARD_SS_PIN);
  File root = SD.open(dirname_string);
  if (!root){  pfstring("problem reading from SD card", pserial); return nil; }
#endif

  while (true) {

#ifdef LINUX_X64
      struct dirent *Dirent = readdir(Dir);
      if(!Dirent) break;

      if((Dirent->d_type & type)
          &&(strcmp((char*)Dirent->d_name, pattern_string ))==0)
      {
          closedir(Dir);
          //sprintf(pattern_string,"rm %s/%s", dirname_string, (char*)Dirent->d_name);
          //system(pattern_string);
          sprintf(pattern_string,"%s/%s", dirname_string, (char*)Dirent->d_name);
          if(false==remove(pattern_string))
            return tee;
          else return nil;
      }
#else
      File entry = root.openNextFile();
      if(!entry) break;

      if( (entry.isDirectory() && (type&0x4)) || (!entry.isDirectory() && (type&0x8)) )
         if(strcmp((char*)entry->name(), pattern_string ) == 0)
      {
         sprintf(pattern_string,"%s//%s", dirname_string, (char*)Dirent->d_name);
         root.remove((char*)pattern_string);
         root.close();
         return tee;
      }
#endif
  };

#ifdef LINUX_X64
  closedir(Dir);
#else
  root.close();
#endif

  return tee;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}




/* (delete-dir pathspec)   delete specified directory.
Returns true if success and otherwise returns nil.
*/
object *fn_deletedir (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
    int type = 0x4 ;  // Directories only
    char pattern_string[256] = "*" ;
    char dirname_string[256] = "/";

    if(stringp(car(args)))
    {
       cstring(car(args), dirname_string, 256) ;
       if(dirname_string[strlen(dirname_string)-1] == '/')
       {
           dirname_string[strlen(dirname_string)-1] = 0x0 ;
           type = 0x04 ; // Directories only
       }

       char *pattern_bgn = &dirname_string[strlen(dirname_string)-1] ;

       while((pattern_bgn!=dirname_string)&&(*pattern_bgn!='/')) pattern_bgn -- ;
       if(*pattern_bgn=='/')
       {
          pattern_bgn ++ ;
          strcpy(pattern_string, pattern_bgn);
          *pattern_bgn = 0x0 ;
       }
       else
       {
           strcpy(pattern_string, dirname_string);
           getcwd(dirname_string, 256); // Current Dir name
       }
    }


  #ifdef LINUX_X64
    DIR *Dir;
    Dir=opendir(dirname_string);
    if(Dir==NULL){  pfstring("problem reading directory from SD card", pserial); return nil; }

  #else
    SD.begin(SDCARD_SS_PIN);
    File root = SD.open(dirname_string);
    if (!root){  pfstring("problem reading from SD card", pserial); return nil; }
  #endif

    while (true) {

  #ifdef LINUX_X64
        struct dirent *Dirent = readdir(Dir);
        if(!Dirent) break;

        if((Dirent->d_type & type)
            &&(strcmp((char*)Dirent->d_name, pattern_string ))==0)
        {
            closedir(Dir);
            //sprintf(pattern_string,"rm %s/%s", dirname_string, (char*)Dirent->d_name);
            //system(pattern_string);
            sprintf(pattern_string,"%s/%s", dirname_string, (char*)Dirent->d_name);
            if(false==remove(pattern_string))
              return tee;
            else return nil;
        }
  #else
        File entry = root.openNextFile();
        if(!entry) break;

        if( (entry.isDirectory() && (type&0x4)) || (!entry.isDirectory() && (type&0x8)) )
           if(strcmp((char*)entry->name(), pattern_string ) == 0)
        {
           sprintf(pattern_string,"%s//%s", dirname_string, (char*)Dirent->d_name);
           root.remove((char*)pattern_string);
           root.close();
           return tee;
        }
  #endif
    };

  #ifdef LINUX_X64
    closedir(Dir);
  #else
    root.close();
  #endif

    return tee;
  #else
    (void) args, (void) env;
    error2("not supported");
    return nil;
  #endif
}



#include <filesystem>  // std::filesystem::rename
#include <string_view> // std::string_view
using namespace std;



/* (rename-file filespec newfile)  rename or moving specified file.
Returns true if success and otherwise returns nil.
*/
object *fn_renamefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;

  char filename_string[256] ;
  char newname_string[256] ;

  if(stringp(car(args))) cstring(car(args), filename_string, 256) ;
  else  {  pfstring("\nrename-file: First argument must be string.", pserial); return nil; }

  args = cdr(args);
  if(stringp(car(args)))
        cstring(car(args), newname_string, 256) ;
  else  {  pfstring("\nrename-file: Second argument must be string.", pserial); return nil; }



#ifdef LINUX_X64
  if (rename(filename_string, newname_string) != 0)
        pfstring("Error rename file", pserial);
      else
        return tee ;

#else
  SD.begin(SDCARD_SS_PIN);
  File root = SD.open(dirname_string);
  if (!root){  pfstring("problem reading from SD card", pserial); return nil; }
#endif


#ifdef LINUX_X64

#else
  root.close();
#endif

  return nil;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}




/* (copy-file filespec newfile)  copy specified file.
Returns true if success and otherwise returns nil.
*/
object *fn_copyfile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;

  char filename_string[256] ;
  char newname_string[256] ;

  if(stringp(car(args))) cstring(car(args), filename_string, 256) ;
  else  {  pfstring("\ncopy-file: First argument must be string.", pserial); return nil; }

  args = cdr(args);
  if(stringp(car(args)))
        cstring(car(args), newname_string, 256) ;
  else  {  pfstring("\ncopy-file: Second argument must be string.", pserial); return nil; }



#ifdef LINUX_X64
  FILE *fp_src, *fp_dest;

  fp_src = fopen(filename_string,"rb");
  if(!fp_src){  pfstring("\ncopy-file: Cannot open source file.", pserial); return nil; }
  fp_dest = fopen(newname_string,"wb");
  if(!fp_dest){ fclose(fp_src); pfstring("\ncopy-file: Cannot open source file.", pserial); return nil; }
  char b[8] ;
  while(0 == feof(fp_src)){
      fread(b, 1, 1, fp_src) ;
      fwrite(b, 1, 1, fp_dest) ;
  }


#else
  SD.begin(SDCARD_SS_PIN);
  File root = SD.open(dirname_string);
  if (!root){  pfstring("problem reading from SD card", pserial); return nil; }
#endif


#ifdef LINUX_X64

#else
  root.close();
#endif

  return tee;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}






/* (ensure-directories-exist pathspec)   Tests whether the specified
directories actually exist, and attempts to create them if they do not.
Returns true if success and otherwise returns nil.
*/
object *fn_ensuredirectoriesexist(object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x8 ;  // Files and directories
  char pattern_string[256] = "*" ;
  char dirname_string[256] = "/";

  if(stringp(car(args))) cstring(car(args), dirname_string, 256) ;
  else  {  pfstring("\nError: argument must be string", pserial); return nil; }



#ifdef LINUX_X64
  DIR *Dir;
  Dir=opendir(dirname_string);
  if(Dir==NULL){
    if(-1==mkdir(dirname_string,0777))
    {  pfstring("problem to create directory", pserial); return nil;  }
  }
  return tee;

#else
  SD.begin(SDCARD_SS_PIN);
  File root = SD.open(dirname_string);
  if (!root){  pfstring("problem reading from SD card", pserial); return nil; }
#endif

  while (true) {

#ifdef LINUX_X64
      struct dirent *Dirent = readdir(Dir);
      if(!Dirent) break;

      if((Dirent->d_type & type)
          &&(strcmp((char*)Dirent->d_name, pattern_string ))==0)
      {
          closedir(Dir);
          sprintf(pattern_string,"%s/%s", dirname_string, (char*)Dirent->d_name);
          remove(pattern_string);
          return tee;
      }
#else
      File entry = root.openNextFile();
      if(!entry) break;

      if( (entry.isDirectory() && (type&0x4)) || (!entry.isDirectory() && (type&0x8)) )
         if(strcmp((char*)entry->name(), pattern_string ) == 0)
      {
         sprintf(pattern_string,"%s//%s", dirname_string, (char*)Dirent->d_name);
         root.remove((char*)pattern_string);
         root.close();
         return tee;
      }
#endif
  };

#ifdef LINUX_X64
  closedir(Dir);
#else
  root.close();
#endif

  return nil;
#else
  (void) args, (void) env;
  error2("not supported");
  return nil;
#endif
}





