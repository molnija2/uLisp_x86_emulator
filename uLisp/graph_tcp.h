#ifndef GRAPH_TCP_H
#define GRAPH_TCP_H


int MouseStateButtons();
int MouseStateX() ;
int MouseStateY() ;
int tcp_getch() ;
int tcp_kbhit() ;


int InitTcpGraphics() ;
void tcp_drawsymbol(int c) ;
void tcp_drawPixel(int x, int y, int color) ;
void tcp_drawLine(int x1, int y1, int x2, int y2, int color);
void tcp_drawRect(int x1, int y1, int x2, int y2, int color);
void tcp_fillRect(int x1, int y1, int x2, int y2, int color);
void tcp_drawCircle(int x, int y, int r, int color) ;
void tcp_fillCircle(int x, int y, int r, int color) ;
void tcp_drawRoundRect(int x1, int y1, int x2, int y2, int r, int color);
void tcp_fillRoundRect(int x1, int y1, int x2, int y2, int r, int color);
void tcp_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color);
void tcp_fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color);

void tcp_drawChar(int x, int y, int ch, int color, int bkcolor, int size);
void tcp_setCursor(int x, int y);
void tcp_setTextColor(int color);
void tcp_setTextBkColor(int color);
void tcp_setRegularTextColors (int color, int bkcolor) ;
void tcp_setTextSize(int sz);
void tcp_setTextWrap(int On) ;
void tcp_fillScreen(int colour) ;
void tcp_setRotation(int On) ;
void tcp_invertDisplay(int On) ;

int tcp_getmaxx ();
int tcp_getmaxy ();
int tcp_getx ();
int tcp_gety ();
int tcp_getfontheight ();
int tcp_getfontwidth ();
int tcp_setfontname(int height, int style, char *name) ;
int tcp_gettextwidth (char *str) ;


#endif // GRAPH_TCP_H
