#ifndef BMP_H
#define BMP_H

#include <sys/types.h>
#include <inttypes.h>


#define WORD    uint16_t
#define DWORD   uint32_t
#define LONG    int32_t


typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD  bfSize;
  DWORD  bfReserved1;
  //WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
  WORD  bfReserved1;
  WORD  bfReserved2;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;


#endif // BMP_H
