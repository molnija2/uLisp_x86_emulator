#include <stdio.h>
#include <unistd.h>

#include "widget.h"
#include "ui_widget.h"

#include <QLayoutItem>

#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
//#include "tcp_display.h"

#define _GFILLINTERIOR   1
#define _GBORDER         0
#define L_GetImage  _getimage
#define L_PutImage  _putimage
#define L_ImageSize  _imagesize

QMutex  keyboardMutex;
QMutex pmpaintMutex;

static int x1_p=0,y1_p=0,x2_p=DEF_QRLISP_MAXX,y2_p=DEF_QRLISP_MAXY ;
static int x1_p2=0,y1_p2=0,x2_p2=DEF_QRLISP_MAXX,y2_p2=DEF_QRLISP_MAXY;

static QColor QT16Colors[16]={
    Qt::black,   Qt::darkBlue,    Qt::darkGreen, Qt::darkCyan,
    Qt::darkRed, Qt::darkMagenta,    Qt::darkYellow, Qt::gray,
    Qt::darkGray,    Qt::blue,        Qt::green,     Qt::cyan,
    Qt::red,     Qt::magenta,        Qt::yellow,    Qt::white
};

static unsigned int VGAColors[16]={
    0x0,   0x07f,    0x07f00, 0x07f7f,
    0x07f0000, 0x07f007f,   0x07f7f00,  0x07f7f7f,
    0x03f3f3f,   0x0ff,    0x0ff00, 0x0ffff,
    0x0ff0000, 0x0ff00ff,   0x0ffff00,  0x0ffffff,
};




Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    codec= QTextCodec::codecForName(/*"IBM866"*/ "Windows-1251" ) ;
    QTextCodec::setCodecForLocale(codec);

    //painter = new QPainter();
    //painter->begin(this);
    //painter->setViewport(0,menubar->heightForWidth( width() )+2.5, w, menubar->heightForWidth( width() )+h+2.5);
    pixmapScreen = QImage( DEF_QRLISP_MAXX, DEF_QRLISP_MAXY, QImage::Format_RGB32 ) ;
    pixmapScreen_buffer = QImage( DEF_QRLISP_MAXX, DEF_QRLISP_MAXY, QImage::Format_RGB32 ) ;

    //pmpainter = new QPainter(&pixmapScreen);

    //ui->verticalLayout->removeWidget(ui->frame);

    scrollArea_Screen = new QScrollScreenArea( this );
    ui->verticalLayout->addWidget(scrollArea_Screen) ;
    scrollArea_Screen->SetPixmap( &pixmapScreen ) ;

    //ui->verticalLayout->addWidget(ui->checkBox_textwindow);
    //ui->verticalLayout->addWidget(ui->toolButton_Exit);
    ui->groupBox_Options->hide();

    textBrowser_tcp = NULL ;
    iUseTextWindow = 0 ;


    textBrowser_tcp = new QTextBrowser(this) ;
    textBrowser_tcp->setMaximumHeight(100) ;
     ui->verticalLayout->addWidget(textBrowser_tcp) ;
    textBrowser_tcp->hide() ;

    iMaxX = DEF_QRLISP_MAXX ;
    iMaxY = DEF_QRLISP_MAXY ;
    //pixmap = QPixmap(iMaxX, iMaxY) ;

    resize(DEF_QRLISP_MAXX, DEF_QRLISP_MAXY) ;

    connect(&server, SIGNAL(newConnection()),
      this, SLOT(acceptConnection()));

    connect(&server_timer, SIGNAL(timeout()),
      this, SLOT(TimerProcessing()));

    //server_timer.start(100) ;

    server.listen(QHostAddress::Any, 1500);

    penwidth = 1 ;

    _SetVGABkcolor(0);
    lcd_SetViewPort(0,0,_GetScreenMaxX(),_GetScreenMaxY()) ;
    Clearscreen(qCurrentBackColor) ;
    //SetViewPort(10,10,_GetScreenMaxX()-10,_GetScreenMaxY()-10) ;
    //SetViewPort(10,10, 80, 80 ) ;
    //P_Rectangle(0,0, 79, 79 ) ;
    lcd_RegisterFonts() ;

    lcd_SetCharSize(1) ;
    lcd_gotoxy(0,8);


    lcd_setFontName("Nimbus Mono L",11,FONT_STYLE_BOLD);


    /*_SetVGAcolor(10) ;
    P_Bar(10,100, 280, 180 ) ;

    _SetVGAcolor(12) ;
    FillEllipse( 100, 100,  30,  20) ;

    _SetVGAcolor(13) ;
    DrawEllipse( 50, 50,  300,  200) ;*/

    _SetVGAcolor(10) ;
    lcd_PutString(100, 10, QString("uLisp GFX-консоль").toLocal8Bit().data() );

//Clearscreen(/*qCurrentBackColor*/Qt::blue) ;
}

Widget::~Widget()
{
    delete scrollArea_Screen ;
    if(textBrowser_tcp) delete textBrowser_tcp ;

    delete ui;
}


void Widget::TimerProcessing()
{

    /*painter_scr.begin(ui->scrollArea_Canvas->viewport()) ;
    painter_scr.drawPixmap(0,0, pixmap) ;
    painter_scr.end() ;*/

    //ui->scrollArea_Canvas->show() ;
    //ui->scrollArea_Canvas->update() ;*/
    //image_label.update() ;
    //image_label.setPixmap(pixmap) ;

}




void Widget::repaint(int x, int y, int w, int h)
{
    /*painter_scr.begin(ui->scrollArea_Canvas) ;
    painter_scr.drawPixmap(0,0, pixmap) ;
    painter_scr.end() ;*/
}



void Widget::on_checkBox_textwindow_stateChanged(int arg1)
{
    if(ui->checkBox_textwindow->isChecked() )
    {
        iUseTextWindow = 1 ;
        textBrowser_tcp = new QTextBrowser(this) ;
        textBrowser_tcp->setMaximumHeight(100) ;
         ui->verticalLayout->addWidget(textBrowser_tcp) ;
    }
    else
    {
        iUseTextWindow = 0 ;
        delete textBrowser_tcp ;
        textBrowser_tcp = NULL ;
        ui->verticalLayout->removeWidget(textBrowser_tcp) ;
    }
}

void Widget::on_toolButton_Exit_triggered(QAction *arg1)
{
}

void Widget::on_toolButton_Exit_clicked()
{
    close();
}


void Widget::acceptConnection()
{
    iWaitImage = 0 ;
    iWaitContinue = 0 ;

  client = server.nextPendingConnection();

  connect(client, SIGNAL(readyRead()),
    this, SLOT(startRead()));


  puts("\nNew connection.") ; ;
}







/**********************************************************************

  Keyboard and mouse functions

*/

char ExtCharsTable[256]={
0,0,0,0,0,0,82,83,0,0,0,0,0,    // 61,62,63,64,65,66...
0,0,0,71,79,75,72,77,80,73,
81,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,59,60,61,62,63,
64,65,66,67,68,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,

};










int Widget::GetArguments(char *str, int len)
{
    int i = 0, iArg = 0 ;

  while(i<len)
  {
    char *cPtr = cOperand[iArg] ;
    while(((*str==' ')||(*str==',')||(*str==0)||(*str=='\n'))&&(i<len))
    {
        str++ ;
        i++ ;
    }

    if(i == len) return iArg ;


    while(((*str!=' ')&&(*str!=',')&&(*str!='\n')&&(*str!=0))&&(i<len))
    {
        *cPtr++ = *str++ ;
        i++ ;
    }

    *cPtr = 0 ;
    iArg++ ;
  }
    return iArg ;
}



void Widget::startRead()
{
    char *buffer, buffer_stk[TCP_BUFFER_SIZE], *buffer_tmp = NULL ;

    //int butter_len = TCP_BUFFER_SIZE ;

    buffer = buffer_stk ;

    int len = client->bytesAvailable() ;

    if((len>256)||((iWaitContinue)||(iWaitImage>0)))
    {
        //butter_len = TCP_LONGBUFFER_SIZE ;
        buffer_tmp = (char*)pixmapScreen_buffer.bits()+iWaitContinue ;//new char[len] ;
        buffer = buffer_tmp ;
    }


    buffer[0] = 0 ;

    qint64 sz = client->read(buffer, len);

    if(len<TCP_BUFFER_SIZE) buffer[len] = 0 ;
    //client->write(buffer, len );  // Echo to client

    //textBrowser_tcp->append(buffer) ;
    //printf("%s  len=%d\n", buffer, len) ; fflush(stdout); // Echo to server console

    if(iWaitImage>0)
    {
        iWaitImage -=sz ;


        if(iWaitImage <= 0)
        {
            QImage image = QImage((uchar*)pixmapScreen_buffer.bits(), iW,iH, QImage::Format_RGB32 ) ;
            //image.fill(Qt::magenta) ;
            PutImage(iX,iY, image) ;

            strcpy(buffer,"OK\n") ;
            client->write(buffer, strlen(buffer) );
            iWaitImage = 0 ;
            iWaitContinue = 0 ;
        }
        else
            iWaitContinue += sz ;


    }
    else
     switch(buffer[0])
    {
    case '*':
        BinaryArgCall(buffer) ;
        break ;
    case 'I':
        if(strncmp(buffer,"INFO:",5)==0)
        {
            strcpy(buffer,"Server example test.") ;
            client->write(buffer, strlen(buffer) );
        }
        break ;

    case 'c':
        if(strncmp(buffer,"close",5)==0)
        {
            puts("Connection closed.") ;
            client->close();
        }
        else if(strncmp(buffer,"clrscr",6)==0)
        {

            Clearscreen(qCurrentBackColor) ;

            strcpy(buffer,"OK\n") ;
            client->write(buffer, strlen(buffer) );
        }
        else
          if(strncmp(buffer,"chrwidth", 8)==0)
        {
            if(buffer[8]=='b')
            {
                unsigned int v ;

                v = this-> lcd_getFontWidth() ;
                client->write((char*)(&v), sizeof(int) );
                break ;
            }

            sprintf(buffer,"%d\n",  lcd_getFontWidth()) ;
            client->write(buffer, strlen(buffer) );
        }
        else
          if(strncmp(buffer,"chrheight", 9)==0)
        {
            if(buffer[9]=='b')
            {
                unsigned int v ;

                v = this-> lcd_getFontHeight() ;
                client->write((char*)(&v), sizeof(int) );
                break ;
            }

            sprintf(buffer,"%d\n",  lcd_getFontHeight()) ;
            client->write(buffer, strlen(buffer) );
        }
        else
        if(strncmp(buffer,"chord",5)==0)
        {

            if(buffer[5]=='b')
            {
                DrawChord(*(int*)(&buffer[6]), *(int*)(&buffer[6+sizeof(int)]),
                        *(int*)(&buffer[6+sizeof(int)*2]), *(int*)(&buffer[6+sizeof(int)*3]),
                        *(int*)(&buffer[6+sizeof(int)*4]), *(int*)(&buffer[6+sizeof(int)*5])) ;
                client->write("OK\n", 3);
                break ;
           }
           int n = GetArguments(buffer, len) ;

           if(n>6)
           {
               DrawChord(atoi(cOperand[1]),atoi(cOperand[2]),
                    atoi(cOperand[3]), atoi(cOperand[4]),
                       atoi(cOperand[5]), atoi(cOperand[6])) ;
               client->write("OK\n", 3);
            }
            else
               client->write("ARG\n", 4);
        }
        break ;

     case 't':
         if(strncmp(buffer,"textwidth", 9)==0)
         {
           if(buffer[9]=='b')
           {
               unsigned int v ;
               //int l = len - 10 ;

               // miss 0x00-symbols
               //while((buffer[l-1]==0)&&(l>0)) l-- ;
               buffer[len] = 0x0 ;

               v = lcd_getTextWidth(&buffer[10]) ;
               client->write((char*)(&v), sizeof(int) );
               break ;
           }

           int n = GetArguments(buffer, len) ;

           if(n>1)
           {
               //int l = len - 10,  k = 10 ;

               // miss ' '-symbols
               //while((buffer[k]==' ')&&(l>0)) { l-- ; k++ ;}
               // miss 0x00-symbols
               //while((buffer[l-1]==0)&&(l>0)) l-- ;

                sprintf(buffer,"%d\n", lcd_getTextWidth(&buffer[10])) ;
                client->write(buffer, strlen(buffer) );
           }
         }
         break ;

     case 'a':
        if(strncmp(buffer,"arc__",5)==0)
        {

            if(buffer[5]=='b')
            {
                DrawArc(*(int*)(&buffer[6]), *(int*)(&buffer[6+sizeof(int)]),
                        *(int*)(&buffer[6+sizeof(int)*2]), *(int*)(&buffer[6+sizeof(int)*3]),
                        *(int*)(&buffer[6+sizeof(int)*4]), *(int*)(&buffer[6+sizeof(int)*5])) ;
                client->write("OK\n", 3);
                break ;
           }
           int n = GetArguments(buffer, len) ;

           if(n>6)
           {
               DrawArc(atoi(cOperand[1]),atoi(cOperand[2]),
                    atoi(cOperand[3]), atoi(cOperand[4]),
                       atoi(cOperand[5]), atoi(cOperand[6])) ;
               client->write("OK\n", 3);
            }
            else
               client->write("ARG\n", 4);
        }
        break ;


    case 'b':
        if(strncmp(buffer,"bar", 3)==0)
        {
            if(buffer[3]=='b')
            {
                P_Bar(*(int*)(&buffer[4]), *(int*)(&buffer[4+sizeof(int)]),
                        *(int*)(&buffer[4+sizeof(int)*2]), *(int*)(&buffer[4+sizeof(int)*3])) ;
                client->write("OK\n", 3);
                break ;
            }

            if(GetArguments(buffer, len) >4)
            {
             P_Bar(atoi(cOperand[1]),atoi(cOperand[2]),
                     atoi(cOperand[3]), atoi(cOperand[4]) ) ;

              strcpy(buffer,"OK\n") ;
              client->write(buffer, strlen(buffer) );
            }
            else
                client->write("ARG\n", 4);
        }
        else
        if(strncmp(buffer,"barround", 8)==0)
        {
            if(buffer[8]=='b')
            {
                R_Bar(*(int*)(&buffer[9]), *(int*)(&buffer[9+sizeof(int)]),
                        *(int*)(&buffer[9+sizeof(int)*2]),
                        *(int*)(&buffer[9+sizeof(int)*3]),
                        *(int*)(&buffer[9+sizeof(int)*4])) ;
                client->write("OK\n", 3);
                break ;
            }

            if(GetArguments(buffer, len) >5)
            {
             R_Bar(atoi(cOperand[1]),atoi(cOperand[2]),
                     atoi(cOperand[3]), atoi(cOperand[4]), atoi(cOperand[5]) ) ;

              strcpy(buffer,"OK\n") ;
              client->write(buffer, strlen(buffer) );
            }
            else
                client->write("ARG\n", 4);
        }
        break ;


    case 'r':
        if(strncmp(buffer,"rectangle", 9)==0)
        {
            if(buffer[9]=='b')
            {
                P_Rectangle(*(int*)(&buffer[10]), *(int*)(&buffer[10+sizeof(int)]),
                        *(int*)(&buffer[10+sizeof(int)*2]), *(int*)(&buffer[10+sizeof(int)*3])) ;
                client->write("OK\n", 3);
                break ;
            }
            if(GetArguments(buffer, len) >4)
            {
             P_Rectangle(atoi(cOperand[1]),atoi(cOperand[2]),
                     atoi(cOperand[3]), atoi(cOperand[4]) ) ;

              strcpy(buffer,"OK\n") ;
              client->write(buffer, strlen(buffer) );
            }
            else
                client->write("ARG\n", 4);
        }
        else if(strncmp(buffer,"rectround", 9)==0)
        {
            if(buffer[9]=='b')
            {
                R_Rectangle(*(int*)(&buffer[10]), *(int*)(&buffer[10+sizeof(int)]),
                     *(int*)(&buffer[10+sizeof(int)*2]), *(int*)(&buffer[10+sizeof(int)*3]),
                     *(int*)(&buffer[10+sizeof(int)*4])) ;
                client->write("OK\n", 3);
                break ;
            }
            if(GetArguments(buffer, len) >5)
            {
             R_Rectangle(atoi(cOperand[1]),atoi(cOperand[2]),
                     atoi(cOperand[3]), atoi(cOperand[4]), atoi(cOperand[5]) ) ;

              strcpy(buffer,"OK\n") ;
              client->write(buffer, strlen(buffer) );
            }
            else
                client->write("ARG\n", 4);
        }
        break ;

    case 'k':

        if(strncmp(buffer,"kbhit", 5)==0)
        {
            if(buffer[5]=='b')
            {
                unsigned int v ;

                v = this->scrollArea_Screen->KBHIT();
                client->write((char*)(&v), sizeof(int) );
                break ;
            }

            sprintf(buffer,"%d\n",this->scrollArea_Screen->KBHIT()) ;
            client->write(buffer, strlen(buffer) );
        }
        break ;

    case 'i':
        if(strncmp(buffer,"imgget", 6)==0)
        {
            int x, y, h, w ;
            if(buffer[6]=='b')
            {
                x = *(int*)(&buffer[7]);
                y = *(int*)(&buffer[7+sizeof(int)]);
                w = *(int*)(&buffer[7+sizeof(int)*2]);
                h = *(int*)(&buffer[7+sizeof(int)*3]);
            }
            else
            if(GetArguments(buffer, len) >4)
            {
                x = atoi(cOperand[1]) ;
                y = atoi(cOperand[2]) ;
                w = atoi(cOperand[3]) ;
                h = atoi(cOperand[4]) ;
            }
            else { client->write("ARG\n", 4);  break ;  }

            QImage image = GetImage(x,y, w,h) ;
            //image.fill(Qt::magenta) ;
            int32_t sz = image.sizeInBytes() ;
            client->write((char*)(&sz),sizeof(sz));

            int res = client->write((char*)(image.bits()),  sz);
            if(res<sz)
            { printf("TCP write error\n") ; fflush(stdout) ; }
            break ;
        }
        if(strncmp(buffer,"imgput", 6)==0)
        {
            int x, y, h, w ;
            int32_t sz ;

            if(buffer[6]=='b')
            {
                x = *(int*)(&buffer[7]);
                y = *(int*)(&buffer[7+sizeof(int)]);
                w = *(int*)(&buffer[7+sizeof(int)*2]);
                h = *(int*)(&buffer[7+sizeof(int)*3]);
                sz = *(int*)(&buffer[7+sizeof(int)*4]);
            }
            else
            if(GetArguments(buffer, len) >2)
            {
                x = atoi(cOperand[1]) ;
                y = atoi(cOperand[2]) ;
                w = atoi(cOperand[1]) ;
                h = atoi(cOperand[2]) ;
                sz = atoi(cOperand[3]) ;
            }
            else {
                client->write("ARG\n", 4);  break ;
            }

            strcpy(buffer,"WAIT\n") ;
            client->write(buffer, strlen(buffer) );

            iWaitImage = sz ;
            iWaitContinue = 0 ;
            iW = w ;
            iH = h ;
            iX = x ;
            iY = y ;

            break ;
        }

    case 'g':
        if(strncmp(buffer,"gotoxy", 6)==0)
        {
            if(buffer[6]=='b')
            {

                CurrentDrawX = *(int*)(&buffer[7]);
                CurrentDrawY = *(int*)(&buffer[7+sizeof(int)]);
                strcpy(buffer,"OK\n") ;
                client->write(buffer, strlen(buffer) );
                break ;
            }
            if(GetArguments(buffer, len) >2)
            {
              CurrentDrawX = atoi(cOperand[1]) ;
              CurrentDrawY = atoi(cOperand[2]) ;

              strcpy(buffer,"OK\n") ;
              client->write(buffer, strlen(buffer) );
            }
            else
                client->write("ARG\n", 4);
            //sprintf(buffer,"%u\n",this->scrollArea_Screen->GETCH()&0xff) ;
        }
        else
        if(strncmp(buffer,"getch", 5)==0)
        {
            if(buffer[5]=='b')
            {
                unsigned int v ;

                while(this->scrollArea_Screen->keypressed==0) usleep(200) ;
                v = this->scrollArea_Screen->GETCH()&0xff;
                client->write((char*)(&v), sizeof(int) );
                break ;
            }

            while(this->scrollArea_Screen->keypressed==0) usleep(200) ;
            sprintf(buffer,"%u\n",this->scrollArea_Screen->GETCH()&0xff) ;
            client->write(buffer, strlen(buffer) );
        }
        else
            if(strncmp(buffer,"getx", 4)==0)
        {
            if(buffer[4]=='b')
            {
                int v ;

                v = _Getx();
                client->write((char*)(&v), sizeof(int) );
                break ;
            }
            sprintf(buffer,"%d\n",_Getx()) ;
            client->write(buffer, strlen(buffer) );
        }
        else  if(strncmp(buffer,"gety", 4)==0)
        {
            if(buffer[4]=='b')
            {
                int v ;

                v = _Gety();
                client->write((char*)(&v), sizeof(int) );
                break ;
            }
            sprintf(buffer,"%d\n",_Gety()) ;
            client->write(buffer, strlen(buffer) );
        }
        else if(strncmp(buffer,"getmaxx", 7)==0)
        {
           if(buffer[7]=='b')
           {
              int v ;

               v = _GetMaxX();
               client->write((char*)(&v), sizeof(int) );
               break ;
            }
            sprintf(buffer,"%d\n", _GetMaxX()) ;
            client->write(buffer, strlen(buffer) );
        }
        else  if(strncmp(buffer,"getmaxy", 7)==0)
        {
           if(buffer[7]=='b')
           {
              int v ;

               v = _GetMaxY();
               client->write((char*)(&v), sizeof(int) );
               break ;
            }

            sprintf(buffer,"%d\n", _GetMaxY()) ;
            client->write(buffer, strlen(buffer) );
        }
        else if(strncmp(buffer,"getscrmaxx", 10)==0)
        {
            if(buffer[10]=='b')
            {
               int v ;

                v = _GetScreenMaxX();
                client->write((char*)(&v), sizeof(int) );
                break ;
             }

             sprintf(buffer,"%d\n", _GetScreenMaxX()) ;
             client->write(buffer, strlen(buffer) );
        }
        else if(strncmp(buffer,"getscrmaxy:", 10)==0)
        {
            if(buffer[10]=='b')
            {
               int v ;

                v = _GetScreenMaxY();
                client->write((char*)(&v), sizeof(int) );
                break ;
             }

             sprintf(buffer,"%d\n", _GetScreenMaxY()) ;
             client->write(buffer, strlen(buffer) );
        }
        else if(strncmp(buffer,"getmousestat", 12)==0)
        {
            if(buffer[12]=='b')
            {
               int *v = (int*)(buffer) ;

               *v++ = this->scrollArea_Screen->mouseX;
               *v++ = this->scrollArea_Screen->mouseY;
               *v++ = this->scrollArea_Screen->LeftButtonState;
               *v++ = this->scrollArea_Screen->MidButtonState;
               *v++ = this->scrollArea_Screen->RightButtonState;
               client->write(buffer, sizeof(int)*5 );
               break ;
             }
             sprintf(buffer,"%d %d %d %d %d\n",
                     this->scrollArea_Screen->mouseX, this->scrollArea_Screen->mouseY,
                     this->scrollArea_Screen->LeftButtonState,
                     this->scrollArea_Screen->MidButtonState,
                     this->scrollArea_Screen->RightButtonState) ;
             client->write(buffer, strlen(buffer) );
        }
        else
        if(strncmp(buffer,"getrgbcolor", 11)==0)
        {
            if(buffer[11]=='b')
            {
                unsigned int color ;

                color = GetRGBcolor(*(int*)(&buffer[12]), *(int*)(&buffer[12+sizeof(int)]),
                        *(int*)(&buffer[12+sizeof(int)*2])) ;
                /*sprintf(buffer,"%u\n", c) ;
                client->write(buffer, strlen(buffer) );*/
                client->write((char*)(&color), sizeof(int) );
                break ;
            }
            int n = GetArguments(&buffer[12], len) ;
            int r, g, b ;


            if(n>3)
            {
                r = atoi(cOperand[0]) ;
                g = atoi(cOperand[1]) ;
                b = atoi(cOperand[2]) ;
                sprintf(buffer,"%u\n", GetRGBcolor(r,g,b));
                client->write(buffer, strlen(buffer) );
            }
        }
        else if(strncmp(buffer,"getvgacolor", 11)==0)
        {
            unsigned int color ;

            if(buffer[11]=='b')
            {

                color = GetVGAcolor(*(int*)(&buffer[12])) ;
                /*sprintf(buffer,"%u\n", c) ;
                client->write(buffer, strlen(buffer) );*/
                client->write((char*)(&color), sizeof(int) );
                break ;
            }
            int n = GetArguments(&buffer[12], len) ;

            if(n>0)
            {
                color = atoi(cOperand[0]) ;

                sprintf(buffer,"%u\n", GetVGAcolor(color));
                client->write(buffer, strlen(buffer) );
            }
        }
        else if(strncmp(buffer,"getfontinfo", 11)==0)
        {
            int ifont ;
            FONT_INFO *sfont ;

            if(buffer[11]=='b')
            {

                ifont = *(int*)(&buffer[12]) ;
                sfont = lcd_getFontInfo(ifont) ;
                if(sfont)
                    client->write((char*)(sfont), sizeof(FONT_INFO*) );
                else
                {
                    sprintf(buffer,"NO\n");
                    client->write((char*)buffer, strlen(buffer) );
                }
                break ;
            }

            int n = GetArguments(&buffer[12], len) ;

            if(n>0)
            {
                ifont = atoi(cOperand[0]) ;
                sfont = lcd_getFontInfo(ifont) ;

                if(sfont)
                {
                    sprintf(buffer,"%s,%d,%d", sfont->name, sfont->Height, sfont->Style);
                    client->write(buffer, strlen(buffer) );
                }
                else
                {
                    sprintf(buffer,"NO\n");
                    client->write((char*)buffer, strlen(buffer) );
                }
            }
        }

        break ;

    case 's':
        if(strncmp(buffer,"setcolor_rgb",12)==0)
        {
            if(buffer[12]=='b')
            {
                _set_rgb_color(*(int*)(&buffer[13]), *(int*)(&buffer[13+sizeof(int)]),
                        *(int*)(&buffer[15+sizeof(int)*2])) ;
                client->write("OK\n", 3);
                break ;
            }
            int n = GetArguments(&buffer[12], len) ;
            int r, g, b ;


            if(n>3)
            {
                r = atoi(cOperand[0]) ;
                g = atoi(cOperand[1]) ;
                b = atoi(cOperand[2]) ;
                _set_rgb_color(r, g, b) ;
                client->write("OK\n", 3);
            }
            else
                client->write("ARG\n", 4);
        }
        else
        if(strncmp(buffer,"setbkcolor_rgb",14)==0)
        {
            if(buffer[14]=='b')
            {
                _set_rgb_Bkcolor(*(int*)(&buffer[15]), *(int*)(&buffer[15+sizeof(int)]),
                        *(int*)(&buffer[15+sizeof(int)*2])) ;
                client->write("OK\n", 3);
                break ;
            }

            int n = GetArguments(&buffer[14], len) ;
            int r, g, b ;


            if(n>3)
            {
                r = atoi(cOperand[0]) ;
                g = atoi(cOperand[1]) ;
                b = atoi(cOperand[2]) ;
                _set_rgb_Bkcolor(r, g, b) ;
                client->write("OK\n", 3);
            }
            else
                client->write("ARG\n", 4);
        }
        else
          if(strncmp(buffer,"setcolor_vga",12)==0)
        {

            if(buffer[12]=='b')
            {
                _SetVGAcolor(*(int*)(&buffer[13])) ;
                client->write("OK\n", 3);
                break ;
            }

            int n = GetArguments(buffer, len) ;

            if(n>1)
            {
                int i = atoi(cOperand[1]) ;
                _SetVGAcolor(i) ;
                client->write("OK\n", 3);
            }
            else
                client->write("ARG\n", 4);

        }
          else
            if(strncmp(buffer,"setbkcolor_vga",14)==0)
        {
              if(buffer[14]=='b')
              {
                  _SetVGABkcolor(*(int*)(&buffer[15])) ;
                  client->write("OK\n", 3);
                  break ;
              }


              int n = GetArguments(buffer, len) ;

              if(n>1)
              {
                  int i = atoi(cOperand[1]) ;
                  _SetVGABkcolor(i) ;
                  client->write("OK\n", 3);
              }
              else
                  client->write("ARG\n", 4);

        }
        else
            if(strncmp(buffer,"setcolor",8)==0)
        {
                if(buffer[8]=='b')
                {
                     SetColor(*(int*)(&buffer[9])) ;
                     client->write("OK\n", 3);
                     break ;
                }


               int n = GetArguments(buffer, len) ;

               if(n>1)
               {
                    unsigned int i = atoi(cOperand[1]) ;
                    SetColor(i) ;
                    client->write("OK\n", 3);
               }
               else
                    client->write("ARG\n", 4);

        }
        else
            if(strncmp(buffer,"setbkcolor",10)==0)
        {

             if(buffer[10]=='b')
             {
                  SetBkColor(*(int*)(&buffer[11])) ;
                  client->write("OK\n", 3);
                  break ;
             }

                int n = GetArguments(buffer, len) ;

               if(n>1)
               {
                   unsigned int i = atoi(cOperand[1]) ;
                   SetBkColor(i) ;
                   client->write("OK\n", 3);
                }
                else
                   client->write("ARG\n", 4);

        }
        else if(strncmp(buffer,"setviewport",11)==0)
        {

            if(buffer[11]=='b')
            {
                lcd_SetViewPort(*(int*)(&buffer[12]), *(int*)(&buffer[12+sizeof(int)]),
                        *(int*)(&buffer[12+sizeof(int)*2]), *(int*)(&buffer[12+sizeof(int)*3])) ;
                client->write("OK\n", 3);
                break ;
           }
           int n = GetArguments(buffer, len) ;

           if(n>4)
           {
               lcd_SetViewPort(atoi(cOperand[1]),atoi(cOperand[2]),
                    atoi(cOperand[3]), atoi(cOperand[4]) ) ;
               client->write("OK\n", 3);
            }
            else
               client->write("ARG\n", 4);
        }
        else if(strncmp(buffer,"strwidth", 8)==0)
        {
            if(buffer[8]=='b')
            {
                int v = lcd_getTextWidth( &buffer[9]) ;

                client->write((char*)(&v), sizeof(int));
                break ;
            }

            if(GetArguments(buffer, len) >1)
            {
             int v = lcd_getTextWidth( &buffer[9]) ;

              sprintf(buffer,"%d\n", v ) ;
              client->write(buffer, strlen(buffer) );
            }
            else
                client->write("ARG\n", 4);
        }
        break ;

    case 'e':
        if(strncmp(buffer,"ellipce",7)==0)
        {
            if(buffer[7]=='b')
            {
                DrawEllipse(*(int*)(&buffer[8]), *(int*)(&buffer[8+sizeof(int)]),
                        *(int*)(&buffer[8+sizeof(int)*2]), *(int*)(&buffer[8+sizeof(int)*3])) ;
                client->write("OK\n", 3);
                break ;
            }

           int n = GetArguments(buffer, len) ;

           if(n>4)
           {
               DrawEllipse(atoi(cOperand[1]),atoi(cOperand[2]),
                    atoi(cOperand[3]), atoi(cOperand[4]) ) ;
               client->write("OK\n", 3);
            }
            else
               client->write("ARG\n", 4);

        }
        else
        if(strncmp(buffer,"ellipcf",7)==0)
        {
            if(buffer[7]=='b')
            {
                FillEllipse(*(int*)(&buffer[8]), *(int*)(&buffer[8+sizeof(int)]),
                        *(int*)(&buffer[8+sizeof(int)*2]), *(int*)(&buffer[8+sizeof(int)*3])) ;
                client->write("OK\n", 3);
                break ;
            }

           int n = GetArguments(buffer, len) ;

           if(n>4)
           {
               FillEllipse(atoi(cOperand[1]),atoi(cOperand[2]),
                    atoi(cOperand[3]), atoi(cOperand[4]) ) ;
               client->write("OK\n", 3);
            }
            else
               client->write("ARG\n", 4);

        }
        break ;

    case 'l':
        if(strncmp(buffer,"line",4)==0)
        {
            if(buffer[4]=='b')
            {
                DrawLine(*(int*)(&buffer[5]),*(int*)(&buffer[5+sizeof(int)]),
                        *(int*)(&buffer[5+sizeof(int)*2]),*(int*)(&buffer[5+sizeof(int)*3])) ;
                client->write("OK\n", 3);
                break ;
            }

           int n = GetArguments(buffer, len) ;

           if(n>4)
           {
               DrawLine(atoi(cOperand[1]),atoi(cOperand[2]),
                    atoi(cOperand[3]), atoi(cOperand[4]) ) ;
               client->write("OK\n", 3);
           }
           else
               client->write("ARG\n", 4);

        }
        break ;

    case 'p':
        if(strncmp(buffer,"point ",5)==0)
        {

            if(buffer[5]=='b')
            {
                DrawPoint(*(int*)(&buffer[6]),*(int*)(&buffer[6+sizeof(int)]) ) ;
                client->write("OK\n", 3);
                break ;
            }

           int n = GetArguments(buffer, len) ;

           if(n>2)
           {
               DrawPoint(atoi(cOperand[1]),atoi(cOperand[2]) ) ;
               client->write("OK\n", 3);
           }
           else
               client->write("ARG\n", 4);

        }
        else if(strncmp(buffer,"points",6)==0)
        {

           int np =  (len - 6)>>1 ;

           if(np>0)
           {
               DrawPoints((int*)(&buffer[6]),np ) ;
               client->write("OK\n", 3);
           }
           else
               client->write("ARG\n", 4);

        }else
        if(strncmp(buffer,"puts", 4)==0)
        {
            if(buffer[4]=='b')
            {
                lcd_PutString(CurrentDrawX, CurrentDrawY, &buffer[5]) ;
                client->write("OK\n", 3);
                break ;
            }

            if(GetArguments(buffer, len) >1)
            {
             lcd_PutString(CurrentDrawX, CurrentDrawY, &buffer[5] ) ;

              strcpy(buffer,"OK\n") ;
              client->write(buffer, strlen(buffer) );
            }
            else
                client->write("ARG\n", 4);
        } else
            if(strncmp(buffer,"putc", 4)==0)
        {
            if(buffer[4]=='b')
            {
                lcd_DisplaySymbol(CurrentDrawX, CurrentDrawY, *(int*)(&buffer[5])) ;
                client->write("OK\n", 3);
                break ;
            }

            if(GetArguments(buffer, len) >1)
            {
             lcd_DisplaySymbol(CurrentDrawX, CurrentDrawY, cOperand[1][0] ) ;

              strcpy(buffer,"OK\n") ;
              client->write(buffer, strlen(buffer) );
            }
            else
                client->write("ARG\n", 4);
        }
        break ;

    case 'f':
        if(strncmp(buffer,"fontnm",6)==0)
        {

            if(buffer[6]=='b')
            {
                if(1==lcd_setFontName(&buffer[7+sizeof(int)*2], *(int*)(&buffer[7]),*(int*)(&buffer[7+sizeof(int)]) ) )
                    client->write("OK\n", 3);
                else client->write("NO\n", 3);
                break ;
            }

           int n = GetArguments(buffer, len) ;

           if(n>2)
           {
               if(1==lcd_setFontName( cOperand[3], atoi(cOperand[1]), atoi(cOperand[2]) ) )
                   client->write("OK\n", 3);
                else client->write("NO\n", 3);
           }
           else
               client->write("ARG\n", 4);

        }
        break;

    default:
        strcpy(buffer,"Unknown command.\n") ;
        client->write(buffer, strlen(buffer) );
    }

    //if(buffer_tmp) delete buffer_tmp ;
}


void Widget::BinaryArgCall(char *buffer)
{

}



void Widget::set_attrs(int col,int bkcol)
{ textcolor=col; textbkcolor=bkcol;
}





void Widget::setSolidFill(void)
{
//  _setfillmask(&Solidmask);
}


int Widget::_Getx()
{

return CurrentDrawX;
}

int Widget::_Gety()
{

return CurrentDrawY;
}



short Widget::_GetMaxX()
{
    short int ret;
//_getvideoconfig( &vdconf );

    ret = iMaxX ;

return ret;
}



short Widget::_GetMaxY()
{
    short int iret;

    iret = iMaxY ;

return iret;
}



short Widget::_GetScreenMaxX()
{
    //short int ret;
return /*DEF_QRLISP_MAXX */ iMaxX ;
}


short Widget::_GetScreenMaxY()
{
    //short int ret;
return /*DEF_QRLISP_MAXX*/ iMaxY ;
}

//static int icurrentcolor;

QColor Widget::GetColorExtended(int col)
{
    char *cPtr;
    int r,g,b;

    cPtr=(char*)(&col);
    if( cPtr[3] != 0x00)
    {
        r=cPtr[0]&0xFF;
        g=cPtr[1]&0xFF;
        b=cPtr[2]&0xFF;
//printf("\nR=%u  G=%u  B=%u     cPtr[3]=%u \n",r,g,b,cPtr[3]&0xFF);
        return QColor(qRgb(r,g,b));
    }
    else
     return QT16Colors[col];
}


void Widget::_SetVGAcolor(int icolor_vga)
{
        textcolor=icolor_vga;
    //color=icolor;

        //pmpaintMutex.lock();    painter.begin(&(pixmapScreen)) ;
        painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                            CurrentViewport.width(),CurrentViewport.height());
        painter.setBrush(Qt::NoBrush) ;
        painter.end() ;

        //app->lock();
        qCurrentForeColor = QT16Colors[icolor_vga] ;
        //GetColorExtended(icolor_vga) ;
        qCurrentForePen = QPen(qCurrentForeColor);
        //LispViewer->pmpainter->setPen(GetColorExtended(icolor));
        //app->unlock();
        //pmpaintMutex.unlock();

}

void Widget::_SetVGABkcolor(int icolor_vga)
{
        textcolor=icolor_vga;
    //color=icolor;

        //pmpaintMutex.lock();
        //app->lock();
        qCurrentBackColor = QT16Colors[icolor_vga] ;
                //GetColorExtended(icolor_vga) ;
        qCurrentBackPen = QPen(qCurrentBackColor);
        //LispViewer->pmpainter->setPen(GetColorExtended(icolor));
        //app->unlock();
        //pmpaintMutex.unlock();

}


unsigned int Widget::GetVGAcolor(int icolor_vga)
{
        textcolor=icolor_vga;

        return VGAColors[icolor_vga] ;

}


unsigned int Widget::GetRGBcolor(int r,int g, int b)
{
        return ((r&0xff)<<16)|((g&0xff)<<8)|(b&0xff) ;
}


void Widget::SetColor (unsigned int color)
{
        qCurrentForeColor = QColor(qRgb(color&0xff,(color>>8)&0xff,(color>>16)&0xff));
        qCurrentForePen = QPen(qCurrentForeColor);
}

void Widget::SetBkColor(unsigned int color)
{
        qCurrentBackColor = QColor(qRgb(color&0xff,(color>>8)&0xff,(color>>16)&0xff));
        qCurrentBackPen = QPen(qCurrentBackColor);

}



void Widget::_set_rgb_Bkcolor(int r, int g, int b)
{
        //app->lock();
        qCurrentBackColor = qRgb(r,g,b) ;
        qCurrentBackPen = QPen(qCurrentForeColor);
        //app->unlock();
}


void Widget::_set_rgb_color(int r, int g, int b)
{
        //app->lock();
        qCurrentForeColor = qRgb(r,g,b) ;
        qCurrentForePen = QPen(qCurrentForeColor, Qt::SolidPattern);

        //app->unlock();
}



void Widget::_restore_current_color()
{
    _SetVGAcolor(textcolor);
}



void Widget::_Get_correct_rectangle(   int x1,int y1,int x2,int y2, int *bx, int *by, int *ex,int *ey)
{


  if(y1>y2) {  *by=y2; *ey=y1; } else { *by=y1; *ey=y2; }
  if(x1>x2) {  *bx=x2; *ex=x1; } else { *bx=x1; *ex=x2; }
  *bx+=x1_p;
  *by+=y1_p;
  *ex+=x1_p;
  *ey+=y1_p;
  if(*ex>x2_p) *ex=x2_p;
  if(*ey>y2_p) *ey=y2_p;
  if(*bx<x1_p) *bx=x1_p;
  if(*by<y1_p) *by=y1_p;
  if(*ex<x1_p) *ex=x1_p;
  if(*ey<y1_p) *ey=y1_p;
  if(*bx>x2_p) *bx=x2_p;
  if(*by>y2_p) *by=y2_p;

}


void Widget::_update_rectangle(    int x1,int y1,int x2,int y2)
{
    int bx,by,ex,ey;

    _Get_correct_rectangle( x1, y1, x2, y2, &bx, &by, &ex, &ey);


  //LispViewer->AddRedrawPoint_ext( bx,  by);
  //LispViewer->AddRedrawPoint_ext( ex , ey);

 }


void Widget::Clearscreen(QColor color)
{
    painter.begin(&(pixmapScreen)) ;
    //painter.setViewport(CurrentViewport) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
    painter.setBrush(Qt::SolidPattern) ;
    painter.fillRect(0,0,CurrentViewport.width(),
                     CurrentViewport.height(),QBrush(color));
    painter.end() ;
}


void Widget::_rectangle(int type, int x,int y,int w,int h)
{
   //int bx,by,color, ex,ey, x2,y2;
   //char *buff_ptr, imagesegment[700];




   /*x2 = x1 + w ;
   y2 = y1 + h ;

   _Get_correct_rectangle( x1, y1, x2, y2, &bx, &by, &ex, &ey);

  w=ex-bx;
  h=ey-by;*/



  //color=textcolor;

  pmpaintMutex.lock();

  if(type==1)
   {
        painter.begin(&(pixmapScreen)) ;
        //painter.setViewport(CurrentViewport) ;
        painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                            CurrentViewport.width(),CurrentViewport.height());
        painter.fillRect(x,y,w,h,QBrush(qCurrentForeColor));
        painter.end() ;
   }
  else
   {
        painter.begin(&(pixmapScreen)) ;
        //painter.setViewport(CurrentViewport) ;
        painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                            CurrentViewport.width(),CurrentViewport.height());
        painter.setPen(qCurrentForePen);
        painter.setBrush(Qt::NoBrush) ;
        painter.drawRect(x,y,w,h);
        painter.end() ;
   }
   //LispViewer->painter->flush();

  //LispViewer->AddRedrawPoint_ext( bx,  by);
  //LispViewer->AddRedrawPoint_ext( bx+w,  by+h);

  pmpaintMutex.unlock();
}





void Widget::P_Rectangle(int x1, int y1, int w, int h)
{
  _rectangle(_GBORDER, x1+CurrentViewport.left(), y1+CurrentViewport.top(), w, h);
}


void Widget::P_Bar(int x1, int y1, int w, int h)
{
  _rectangle(_GFILLINTERIOR,x1+CurrentViewport.left(),y1+CurrentViewport.top(), w, h);
}



void Widget::R_Rectangle(int x1, int y1, int w, int h, int r)
{
    painter.begin(&(pixmapScreen)) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
    painter.setBrush(Qt::NoBrush) ;
    painter.drawRoundedRect(x1,y1,w,h,r,r);
    painter.end() ;
}


void Widget::R_Bar(int x1, int y1, int w, int h, int r)
{
    painter.begin(&(pixmapScreen)) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
    painter.setBrush(QBrush(qCurrentForeColor));
    painter.drawRoundedRect(x1,y1,w,h,r,r);
    painter.end() ;
}



void Widget::P_SetFillStyle(char *mask)
{
//_setfillmask(mask);
    //qCurrentBackBrash.
}




void Widget::DrawEllipse( int x, int y,  int w,  int h)
{
    painter.begin(&(pixmapScreen)) ;
    //painter.setViewport(CurrentViewport) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
    painter.setBrush(QBrush(qCurrentBackColor));
    painter.setBrush(Qt::NoBrush) ;
    painter.setPen(qCurrentForePen);
    painter.drawEllipse(x+CurrentViewport.left(),y+CurrentViewport.top(),w,h);
    painter.end() ;
}


void Widget::FillEllipse( int x, int y,  int w,  int h)
{
    painter.begin(&(pixmapScreen)) ;
    //painter.setViewport(CurrentViewport) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());

    painter.setBrush(QBrush(qCurrentForeColor));
    painter.setPen(qCurrentForePen);
    painter.drawEllipse(x+CurrentViewport.left(),y+CurrentViewport.top(),w,h);
    painter.end() ;
}



void Widget::lcd_SetViewPort(int x, int y, int w, int h)
{
    CurrentViewport = QRect(x,y,w,h) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
}


void Widget::DrawArc( int x, int y, int w,int h, int astart, int aspan)
{
    painter.begin(&(pixmapScreen)) ;
    //painter.setViewport(CurrentViewport) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
    painter.setPen(qCurrentForePen);
    painter.drawArc(x+CurrentViewport.left(),y+CurrentViewport.top(),
                    w,h,astart, aspan);
    painter.end() ;
}



void Widget::DrawChord( int x, int y, int w,int h, int astart, int aspan)
{
    painter.begin(&(pixmapScreen)) ;
    //painter.setViewport(CurrentViewport) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
    painter.setPen(qCurrentForePen);
    painter.drawChord(x+CurrentViewport.left(),y+CurrentViewport.top(),
                    w,h,astart, aspan);
    painter.end() ;
}


void Widget::DrawLine( int x, int y, int x2,int y2)
{
    painter.begin(&(pixmapScreen)) ;
    //painter.setViewport(CurrentViewport) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
    painter.setPen(qCurrentForePen);
    painter.drawLine(x+CurrentViewport.left(),y+CurrentViewport.top(),
                    x2+CurrentViewport.left(),y2+CurrentViewport.top());
    painter.end() ;
}

void Widget::DrawPoint( int x, int y)
{
    painter.begin(&(pixmapScreen)) ;
    //painter.setViewport(CurrentViewport) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
    painter.setPen(qCurrentForePen);
    painter.drawPoint(x+CurrentViewport.left(),y+CurrentViewport.top());
    painter.end() ;
}



static QPoint qpoints[200] ;
static QPoint qpoints2[200] ;

void Widget::DrawPoints( int *xx, int n)
{
    int i ;

    for(i=0;i<n;i++)
    {
        qpoints[i].setX(xx[i+i]+CurrentViewport.left()) ;
        qpoints[i].setY(xx[i+i+1]+CurrentViewport.top()) ;

    }
    painter.begin(&(pixmapScreen)) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());
    painter.setPen(qCurrentForePen);
    painter.drawPoints(qpoints, n);
    painter.end() ;
}












static unsigned char iDecodeRusTable[256] = {
    0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,
    0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x10,  0x11,  0x12,  0x13,  0x14,  0x15,  0x16,  0x17,
    0x18,  0x19,  0x1A,  0x1B,  0x1C,  0x1D,  0x1E,  0x1F,
    0x20,  0x21,  0xDD,  0x23,  0x24,  0x25,  0x3F,  0xFD,
    0x28,  0x29,  0x2A,  0x2B,  0xE1,  0x2D,  0xFE,  0x2E,
    0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,
    0x38,  0x39,  0xC6,  0xE6,  0xC1,  0x3D,  0xDE,  0x2C,
    0x22,  0xD4,  0xC8,  0xD1,  0xC2,  0xD3,  0xC0,  0xCF,
    0xD0,  0xD8,  0xCE,  0xCB,  0xC4,  0xDC,  0xD2,  0xD9,
    0xC7,  0xC9,  0xCA,  0xDB,  0xC5,  0xC3,  0xCC,  0xD6,
    0xD7,  0xCD,  0xDF,  0xF5,  0x5C,  0xFA,  0x3A,  0x5F,
    0xB8,  0xF4,  0xE8,  0xF1,  0xE2,  0xF3,  0xE0,  0xEF,
    0xF0,  0xF8,  0xEE,  0xEB,  0xE4,  0xFC,  0xF2,  0xF9,
    0xE7,  0xE9,  0xEA,  0xFB,  0xE5,  0xE3,  0xEC,  0xF6,
    0xF7,  0xED,  0xFF,  0xD5,  0x7C,  0xDA,  0xA8,  0x0,
  } ;





void Widget::RegisterNewFont(uint16_t Height, uint16_t Width,
        uint16_t Style, uint16_t Dir, char name[32], uint16_t *DATA )
{
    FONT_INFO *CurrentFont = &FontsBase[iFontsNumber] ;

    CurrentFont->Height = Height ;
    CurrentFont->Width = Width ;
    CurrentFont->Style = Style ;
    CurrentFont->Dir = Dir ;
    strcpy(CurrentFont->name, name) ;
    CurrentFont->DATA = DATA ;

    iFontsNumber++ ;
}


void Widget::lcd_RegisterFonts()
{
    int i, j ;

    CurrentFont = &FontsBase[0] ;
    iFontsNumber = 0 ;

    CharSize = 1 ;


    RegisterNewFont(27, 15, 0, 0, "DejaVu Sans" , (uint16_t*)(font_Deja_Book_15_27_hx));
    RegisterNewFont(27, 15, 0, 0, "DejaVu Sans" , (uint16_t*)(font_Deja_Bold_15_27_hx));
    RegisterNewFont(27, 16, 1, 0,"DejaVu Sans", (uint16_t*)(font_Deja_Oblique_16_27_hx)) ;
    RegisterNewFont(27, 16, 1, 0,"DejaVu Sans", (uint16_t*)(font_Deja_Bold_Oblique_16_27_hx)) ;

    RegisterNewFont(22, 14, 0, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_14_22_hx)) ;
    RegisterNewFont(21, 16, 1, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_Oblique_16_21_hx)) ;
    RegisterNewFont(21, 15, 2, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_15_21_hx)) ;
    RegisterNewFont(21, 16, 3, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_Oblique_16_21_hx)) ;


    RegisterNewFont(19,10,0,0,"DejaVu Sans",(uint16_t*)(font_Deja_Book_10_19_hx)) ;
    RegisterNewFont(18,10,1,0,"DejaVu Sans",(uint16_t*)(font_Deja_Oblique_10_18_hx)) ;
    RegisterNewFont(19,11,0,0,"DejaVu Sans",(uint16_t*)(font_Deja_Bold_11_19_hx)) ;
    RegisterNewFont(19,11,1,0,"DejaVu Sans",(uint16_t*)(font_Deja_Bold_Oblique_11_19_hx)) ;

    RegisterNewFont(16,9,0,0,"DejaVu Sans",(uint16_t*)(font_Deja_Book_9_16_hx)) ;
    RegisterNewFont(16,9,1,0,"DejaVu Sans",(uint16_t*)(font_Deja_Oblique_9_16_hx)) ;
    RegisterNewFont(15,9,2,0,"DejaVu Sans",(uint16_t*)(font_Deja_Bold_9_15_hx)) ;
    RegisterNewFont(15,9,3,0,"DejaVu Sans",(uint16_t*)(font_Deja_Bold_Oblique_9_15_hx)) ;

    RegisterNewFont(14, 8, 0, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_8_14_hx)) ;
    RegisterNewFont(14, 9, 1, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_Oblique_9_14_hx)) ;
    RegisterNewFont(13, 9, 2, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_9_13_hx)) ;
    RegisterNewFont(13, 10, 3, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_Oblique_10_13_hx)) ;

    RegisterNewFont(12, 7, 0, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_7_12_hx)) ;
    RegisterNewFont(12, 8, 1, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_Oblique_8_12_hx)) ;
    RegisterNewFont(11, 8, 2, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_8_11_hx)) ;
    RegisterNewFont(11, 9, 3, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_Oblique_9_11_hx)) ;

    for(i=0;i<iFontsNumber;i++)
    {
        for(j=i+1;j<iFontsNumber;j++)
            if(FontsBase[i].Height > FontsBase[j].Height)
        {
            FontTmp = FontsBase[i] ;
            FontsBase[i] = FontsBase[j] ;
            FontsBase[j] = FontTmp ;
        }
    }

    CurrentFont = &FontsBase[0] ;


}


int Widget::lcd_getFontsNumber()
{
    return iFontsNumber ;
}


void Widget::lcd_setFont(int i)
{

    if(i<0) i=0;
    if(i>= iFontsNumber) i = iFontsNumber-1 ;
    CurrentFont = &FontsBase[i] ;
}




int Widget::lcd_setFontName(char *family,int iHeight, int istyle)
{
    int i = 0, k ;

    if(strcmp(family,"Default") == 0) family = NULL ;
    if(strcmp(family,"default") == 0) family = NULL ;
    if(strcmp(family,"DEFAULT") == 0) family = NULL ;

    //if(family == NULL)
    while((i<iFontsNumber) && ((iHeight>FontsBase[i].Height)||(istyle!=FontsBase[i].Style)))
    {

        i++ ;
    }

    k = i ;
    if(family != NULL)
    {
        while(((k<iFontsNumber) &&
               (  (iHeight>FontsBase[k].Height)
                ||(istyle!=FontsBase[k].Style)
                ||(strncmp(family,FontsBase[k].name,strlen(family)) != 0)))) k++ ;
    }

    if(k>= iFontsNumber) k = i ;
    if(k>= iFontsNumber) return 0 ;

    CurrentFont = &FontsBase[k] ;

    return 1 ;
}


FONT_INFO* Widget::lcd_getFontInfo(int i)
{
    if(i < iFontsNumber) return &FontsBase[i];
    return NULL ;
}


void Widget::lcd_SetCharSize(int Size)
{
    CharSize = Size ;
}


void Widget::lcd_DisplaySymbol(int x,int y, int c)
{
    int ix, iy;
    int ih,iw ;
    int iSymbStep ;
    int iN_points = 0, iN_points_bk = 0 ;

    unsigned int i = c & 0xff ;

    ih = CurrentFont->Height ;
    iw = CurrentFont->Width ;

  if(CharSize == 1)
  {

    if(CurrentFont->Dir==0)
    {
        iSymbStep = ih ;

        //LCD_Address_Set(x,y,x+iw-1,y+ih-1);

        //LCD_Send_Start() ;

        for(iy=0;iy<ih;iy++)
            for(ix=0;ix<iw;ix++)
        {
            //if( (CurrentFont->DATA[i*iSymbStep + iy] & (1<<(16-ix)))!=0)
            //	LCDPutPixel(ix+x,iy+y, CurrentColor) ;
            if( (CurrentFont->DATA[i*iSymbStep + iy] & (1<<(16-ix)))!=0)
            {
                qpoints[iN_points].setX(ix+x +CurrentViewport.left()) ;
                qpoints[iN_points].setY(iy+y +CurrentViewport.top()) ;

                iN_points++ ;
                //LCD_SendDataForFast( qCurrentForeColor ) ;
            }
            else
            {
                qpoints2[iN_points_bk].setX(ix+x +CurrentViewport.left()) ;
                qpoints2[iN_points_bk].setY(iy+y +CurrentViewport.top()) ;
                iN_points_bk ++ ;
                //LCD_SendDataForFast( CurrentTextBkColor) ;
            }
                //LCD_WR_DATA(CurrentTextBkColor);
        }
        painter.begin(&(pixmapScreen)) ;
        painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                            CurrentViewport.width(),CurrentViewport.height());

        //painter.set
        //painter.setPen(QPen(qCurrentForePen, Qt::SolidPattern));
        painter.setPen(qCurrentForePen);
        painter.drawPoints(qpoints, iN_points);
        painter.setPen(qCurrentBackPen);
        painter.drawPoints(qpoints2, iN_points_bk);
        painter.end() ;


     }
     else
     {
        iSymbStep = iw ;

        //LCD_Address_Set(x,y,x+iw-1,y+ih-1);

        //LCD_Send_Start() ;

        for(iy=0;iy<ih;iy++)
            for(ix=0;ix<iw;ix++)
        {
            //if( (CurrentFont->DATA[i*iSymbStep + iw-1-ix] & (1<<(16-iy)))!=0)
            //	LCDPutPixel(x+iw-1-ix,y+ih-1-iy, CurrentColor) ;
            if( (CurrentFont->DATA[i*iSymbStep + iw-1-ix] & (1<<(16-iy)))!=0)
            {
                qpoints[iN_points].setX(ix+x +CurrentViewport.left()) ;
                qpoints[iN_points].setY(iy+y +CurrentViewport.top()) ;

                iN_points++ ;
                //LCD_SendDataForFast( CurrentColor ) ;
            }
            else
            {
                qpoints2[iN_points_bk].setX(ix+x +CurrentViewport.left()) ;
                qpoints2[iN_points_bk].setY(iy+y +CurrentViewport.top()) ;
                iN_points_bk ++ ;
                //LCD_SendDataForFast( CurrentTextBkColor) ;
            }
        }

        painter.begin(&(pixmapScreen)) ;
        painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                            CurrentViewport.width(),CurrentViewport.height());
        painter.setPen(qCurrentForePen);
        painter.drawPoints(qpoints, iN_points);
        painter.setPen(qCurrentBackPen);
        painter.drawPoints(qpoints2, iN_points_bk);
        painter.end() ;
        //LCD_Send_Stop() ;
    }

    CurrentDrawX = x + iw ;
    CurrentDrawY = y ;
  }

  if(CurrentDrawX>=( DEF_QRLISP_MAXX - CurrentFont->Width*CharSize))
  {
    CurrentDrawX = 0 ;
    CurrentDrawY += CurrentFont->Height*CharSize ;
  }

  if((i == 13)||(i == 10))
  {
    CurrentDrawX = 0 ;
    CurrentDrawY += CurrentFont->Height*CharSize ;
  }

  if(CurrentDrawY>=( DEF_QRLISP_MAXY - CurrentFont->Height*CharSize))
  {

  }
}



void Widget::lcd_DisplaySymbolRus(int x,int y, int i )
{
    if(i<128) i = iDecodeRusTable[i] & 0xff ;

    lcd_DisplaySymbol( x, y, i) ;
}





void Widget::lcd_PutString(int x, int y, char *str)
{
    unsigned int i ;

    //CurrentColor = color ;

    CurrentDrawX = x ;
    CurrentDrawY = y ;

    while(*str!=0)
    {
        i = *(unsigned char *)str ;

        lcd_DisplaySymbol(CurrentDrawX,CurrentDrawY, i&0xff ) ;

        //x += CurrentFont->Width*CharSize ;

        str++ ;
    }
}

void Widget::lcd_PutChar(unsigned int  c)
{
    lcd_DisplaySymbol(CurrentDrawX, CurrentDrawY, c&0xff ) ;
}

void Widget::lcd_PutCharRus(unsigned int  c)
{
    lcd_DisplaySymbolRus(CurrentDrawX, CurrentDrawY, c&0xff ) ;
}


void Widget::lcd_PutStringRus(int x, int y, char *str)
{
    unsigned int i ;

    //CurrentColor = color ;

    CurrentDrawX = x ;
    CurrentDrawY = y ;

    while(*str!=0)
    {
        i = *(unsigned char *)str ;

        lcd_DisplaySymbolRus(CurrentDrawX,CurrentDrawY, i&0xff ) ;

        //x += CurrentFont->Width * CharSize ;

        str++ ;
    }
}





void Widget::lcd_gotoxy(int x, int y)
{
    CurrentDrawX = x ;
    CurrentDrawY = y ;
}

int Widget::lcd_getx()
{
    return CurrentDrawX ;
}

int Widget::lcd_gety()
{
    return CurrentDrawY ;
}

int Widget::lcd_getFontHeight()
{
    return CurrentFont->Height*CharSize ;
}

int Widget::lcd_getFontWidth()
{
    return CurrentFont->Width*CharSize ;
}

int Widget::lcd_getTextWidth(char *str)
{
    return strlen(str) * lcd_getFontWidth() ;
}



QImage Widget::GetImage (int x, int y, int w, int h)
{
    return pixmapScreen.copy(QRect(x+CurrentViewport.left(),y+CurrentViewport.top(),w,h)) ;
}


void Widget::PutImage (int x, int y, QImage image)
{
    painter.begin(&(pixmapScreen)) ;
    painter.setClipRect(CurrentViewport.left(),CurrentViewport.top(),
                        CurrentViewport.width(),CurrentViewport.height());

    painter.drawImage(CurrentViewport.left()+x, CurrentViewport.top()+y, image) ;
    painter.end() ;
}


void Widget::on_groupBox_Options_clicked()
{
}

void Widget::showOptions()
{
    /*if(ui->groupBox_Options->isHidden())
        ui->groupBox_Options->show();
    else
        ui->groupBox_Options->hide();*/
    if(textBrowser_tcp->isHidden() )
    {
        iUseTextWindow = 1 ;
        textBrowser_tcp->show() ;
    }
    else
    {
        iUseTextWindow = 0 ;
        textBrowser_tcp->hide() ;
    }
}
