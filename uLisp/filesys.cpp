#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <stdlib.h>


#include "ulisp.h"


#define atom(x) (!consp(x))
extern object *tee;



/*
  (directory [pattern])
  Returns a list of the filenames of the files on the SD card.
  Pattern is string which contains '*' symbols.
*/
 //  (directory "/home/*/")  - search directories
 //  (directory "/home/*")  ("/home/*.*")  ("/home/*.txt") search files

object *fn_probefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x8 ;  // Files and directories
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
        }

        char *pattern_bgn = &dirname_string[strlen(dirname_string)-1] ;
        if(pattern_bgn)
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
            strcpy(dirname_string, "/"); // Dir name "/" restore
        }
        //strcat(pattern_string, "*");
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
      if(!Dirent) break;

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



object *fn_deletefile (object *args, object *env) {
#if defined(sdcardsupport)
  (void) env;
  int type = 0x8 ;  // Files and directories
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
        }

        char *pattern_bgn = &dirname_string[strlen(dirname_string)-1] ;
        if(pattern_bgn)
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
            strcpy(dirname_string, "/"); // Dir name "/" restore
        }
        //strcat(pattern_string, "*");
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
      if(!Dirent) break;

      if((Dirent->d_type & type)
          &&(strcmp((char*)Dirent->d_name, pattern_string ))==0)
      {
          sprintf(pattern_string,"rm %s//%s", dirname_string, (char*)Dirent->d_name);
          closedir(Dir);
          system(pattern_string);
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
