#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDialog>
#include <QFontDatabase>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPicture>

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>


#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QTextCodec>
#include <QMutex>
#include <QPen>
#include <QBrush>
#include <QTimer>
#include <QPainter>
#include <QPointF>
#include <QKeyEvent>
#include <QColor>
#include <QRgb>
#include <QStatusBar>
#include <QCursor>
#include <QRect>
#include <QRectF>
#include <QImage>
#include <QTextBrowser>

#include "qscrollscreenarea.h"

#include "lcd_text.h"


#define DEF_QRLISP_MAXX         800
#define DEF_QRLISP_MAXY         600




QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


    QTimer server_timer ;
    int recieveMessage() ;

    QPainter painter ;
    QPainter painter_scr ;



    unsigned int textcolor, textbkcolor;
    short _Char_Sizex=8, _Char_Sizey=15;
    char *_CurrentFont;

    char cBuffer[256] ;

    FONT_INFO FontsBase[100] , FontTmp ;
    FONT_INFO *CurrentFont ;

    int iFontsNumber = 0 ;


    int iMaxX, iMaxY ;
    int CurrentDrawX, CurrentDrawY, CharSize ;

    QScrollScreenArea *scrollArea_Screen ;

    QTextBrowser *textBrowser_tcp ;



    QLabel imageLabel ;
    QImage pixmapScreen, debug_display_image ;
    QPainter *pmpainter;

    QTextCodec *codec;
    QRect ClipRect;
    QPen qCurrentForePen ;
    QPen qCurrentBackPen ;
    QPen qCurrentBackBrash ;
    QColor qCurrentForeColor ;
    QColor qCurrentBackColor ;

    QRect CurrentViewport ;

    //int points_x[200], points_y[200] ;
    //int points2_x[200], points2_y[200] ;

    char cOperand[10][32] ;
    char buffer_tcp[1024*1024] ;

    int penwidth ;
    int volatile iUseTextWindow ;

public slots:

    void repaint(int x, int y, int w, int h) ;

  void acceptConnection();
  void startRead();
  void TimerProcessing() ;





private slots:
  void on_checkBox_textwindow_stateChanged(int arg1);

  void on_toolButton_Exit_triggered(QAction *arg1);

  void on_toolButton_Exit_clicked();

private:
    QTcpServer server;
    QTcpSocket* client;

    int GetArguments(char *str, int len) ;
    void BinaryArgCall(char * buffer) ;

  void set_attrs(int col,int bkcol);
  void setSolidFill(void);
  int _Getx();
  int _Gety();
  short _GetMaxX();
  short _GetMaxY();
  short _GetScreenMaxX();
  short _GetScreenMaxY();
  QColor GetColorExtended(int col);

  unsigned int GetVGAcolor(int icolor_vga);
  unsigned int GetRGBcolor(int r,int g, int b);
  void _SetVGAcolor(int icolor);
  void _SetVGABkcolor(int icolor_vga);
  void SetColor(unsigned int color) ;
  void SetBkColor(unsigned int color) ;
  void _set_rgb_color(int r, int g, int b) ;
  void _set_rgb_Bkcolor(int r, int g, int b) ;

  void _restore_current_color();
  void _Get_correct_rectangle(   int x1,int y1,int x2,int y2, int *bx, int *by, int *ex,int *ey);
  void _update_rectangle(    int x1,int y1,int x2,int y2);
  void _rectangle(int type,    int x1,int y1,int w,int h);
  void P_Rectangle(int x1, int y1, int w, int h);
  void P_Bar(int x1, int y1, int w, int h);
  void R_Rectangle(int x1, int y1, int w, int h, int r);
  void R_Bar(int x1, int y1, int w, int h, int r);
  void P_SetFillStyle(char *mask);
  void Clearscreen(QColor color) ;
  void DrawEllipse(int x, int y, int w, int h) ;
  void FillEllipse(int x, int y, int w, int h) ;
  void SetViewPort( int x,  int y,  int w, int h);
  void DrawArc( int x, int y, int w,int h, int astart, int aspan) ;
  void DrawChord( int x, int y, int w,int h, int astart, int aspan) ;
  void DrawLine( int x, int y, int x2,int y2) ;
  void DrawPoint( int x, int y) ;
  void DrawPoints( int *xx, int n) ;



    void RegisterNewFont(uint16_t Height, uint16_t Width,
                     uint16_t Style, uint16_t Dir, char name[32], uint16_t *DATA ) ;
  void lcd_RegisterFonts() ;
  void lcd_DisplaySymbol(int x,int y, int ch);
  void lcd_DisplaySymbolRus(int x,int y, int ch);
  int lcd_getx();
  int lcd_gety();
  void lcd_gotoxy(int x, int y);
  void lcd_goto_text_xy(int col, int row) ;


  void lcd_setcolor(int i) ;
  void lcd_setBkcolor(int i) ;
  void lcd_PutString(int x, int y, char *str);
  void lcd_PutStringRus(int x, int y, char *str);
  int lcd_getFontHeight();
  int lcd_getFontWidth();
  void lcd_setFont(int i );
  int lcd_getFontsNumber() ;
  int lcd_setFontName(char *family,int iHeight, int istyle);
  int lcd_getTextWidth(char *str);
  void lcd_SetCharSize(int Size);
  void lcd_PutChar(unsigned int  c) ;
  void lcd_PutCharRus(unsigned int  c) ;

    QImage GetImage(int x, int y, int w, int h) ;
    void PutImage(int x, int y, QImage &image) ;
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
