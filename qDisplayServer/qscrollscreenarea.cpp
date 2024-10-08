#include <stdio.h>
#include <unistd.h>

#include <QPainter>
#include <QPaintEngine>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>

#include "qscrollscreenarea.h"
#include "widget.h"

extern QMutex pmpaintMutex;
extern QMutex  keyboardMutex;
extern QApplication *app;

static  Widget *qrlisp ;



#define ALT_mask 0x0001
#define CTRL_mask 0x0002
#define SHIFT_mask 0x0004


QScrollScreenArea::QScrollScreenArea(QWidget *parent) :
    QAbstractScrollArea(parent)
    //ui(new Ui::QScrollScreenArea)
{
    //ui->setupUi(this);

    qrlisp = (Widget*)(parent) ;

    codec = qrlisp->codec;

    //codec= QTextCodec::codecForName("IBM866" /*"Windows-1251"*/ ) ;
    //QTextCodec::setCodecForLocale(codec);

    clipboard = QApplication::clipboard();
    mimeData = clipboard->mimeData();

    LeftButtonState=0;
    MidButtonState=0;
    RightButtonState=0;
    mouseX=0;
    mouseY=0;

    keypressed=0;
    iModifierPressed=0;
    ikeyread=0;


    this->horizontalScrollBar()->setPageStep(width());
    this->horizontalScrollBar()->setRange(0,DEF_QRLISP_MAXX - width()) ;

    this->verticalScrollBar()->setPageStep(height());
    this->verticalScrollBar()->setRange(0,DEF_QRLISP_MAXY-height()) ;

    this->setBackgroundRole(QPalette::NoRole);

    setAutoFillBackground(false) ;

    setUpdatesEnabled(true);

    this->startTimer(100) ;




}

QScrollScreenArea::~QScrollScreenArea()
{
    //delete ui;
}


void QScrollScreenArea::SetPixmap( QImage *pixmap )
{
    Pixmap = pixmap ;
}





void QScrollScreenArea::mousePressEvent( QMouseEvent * e )
{
    if(e->button()==Qt::LeftButton) LeftButtonState=1;
     else
      if(e->button()==Qt::MidButton) MidButtonState=1;
       else
        if(e->button()==Qt::RightButton) RightButtonState=1;


    mouseX=e->pos().x() ;
    mouseY=e->pos().y() ;

    /*QPoint pos = this->pos() ;
    qrlisp->mouseX+= pos.x() ;
    qrlisp->mouseY+= pos.y() ;*/

    mouseX+= this->horizontalScrollBar()->value() ;
    mouseY+= this->verticalScrollBar()->value() ;


    sprintf(cBuffer,"mouse:   Left=%d  Mid=%d Rihgt=%d  Mouse: x=%d   y=%d ",
            LeftButtonState,
            MidButtonState,
            RightButtonState,
            mouseX,mouseY );
    if(qrlisp->iUseTextWindow==1)
        qrlisp->textBrowser_tcp->append(cBuffer);
}


void QScrollScreenArea::mouseReleaseEvent( QMouseEvent * e )
{
    if(e->button()==Qt::LeftButton) LeftButtonState=0;
     else
      if(e->button()==Qt::MidButton) MidButtonState=0;
       else
        if(e->button()==Qt::RightButton) RightButtonState=0;


       mouseX=e->pos().x() ;
       mouseY=e->pos().y() ;

       /*QPoint pos = this->pos() ;
       qrlisp->mouseX+= pos.x() ;
       qrlisp->mouseY+= pos.y() ;*/

       mouseX+= this->horizontalScrollBar()->value() ;
       mouseY+= this->verticalScrollBar()->value() ;

    sprintf(cBuffer,"mouse:   Left=%d  Mid=%d Rihgt=%d  Mouse: x=%d   y=%d ",
            LeftButtonState,
            MidButtonState,
            RightButtonState,
            mouseX,mouseY );
     if(qrlisp->iUseTextWindow==1) qrlisp->textBrowser_tcp->append(cBuffer);
}


void QScrollScreenArea::mouseMoveEvent( QMouseEvent * e )
{
//  QPoint pos = e.pos();

    mouseX=e->pos().x();
    mouseY=e->pos().y();

    /*QPoint pos = this->pos() ;
    qrlisp->mouseX+= pos.x() ;
    qrlisp->mouseY+= pos.y() ;*/

    mouseX+= this->horizontalScrollBar()->value() ;
    mouseY+= this->verticalScrollBar()->value() ;

    sprintf(cBuffer,"mouse:    x=%d    y=%d",mouseX,mouseY);
    if(qrlisp->iUseTextWindow==1) qrlisp->textBrowser_tcp->append(cBuffer);
}


void QScrollScreenArea::paintEvent ( QPaintEvent * event )
{
    if(!pmpaintMutex.tryLock(100))
    {
        //qrlisp->AddPointsCount = 1 ;
        return ;
    }
    //pmpaintMutex.lock();

    //if(qrlisp->AddPointsCount==0) return ;

    //setViewportMargins(50, 50, 50, 50);

    QPainter painter(this->viewport()) ;




    /*painter.drawPixmap( qrlisp->ClipRect.x(),qrlisp->ClipRect.y(),
                        *Pixmap,
                        qrlisp->ClipRect.x()+this->horizontalScrollBar()->value(),
                        qrlisp->ClipRect.y()+this->verticalScrollBar()->value(),
                        qrlisp->ClipRect.width(), qrlisp->ClipRect->height()) ;*/

    painter.drawImage( 0,0,
                        *Pixmap,
                        0+this->horizontalScrollBar()->value(),
                        0+this->verticalScrollBar()->value(),
                        this->width(), this->height()) ;
    painter.end() ;

    pmpaintMutex.unlock();

    AddPointsCount = 0 ;
}


void QScrollScreenArea::resizeEvent( QResizeEvent * event )
{
    this->horizontalScrollBar()->setPageStep(width());
    this->horizontalScrollBar()->setRange(0,DEF_QRLISP_MAXX - width()) ;

    this->verticalScrollBar()->setPageStep(height());
    this->verticalScrollBar()->setRange(0,DEF_QRLISP_MAXY-height()) ;

}


void QScrollScreenArea::timerEvent( QTimerEvent * event )
{
    //if(qrlisp->AddPointsCount==0) return ;

    viewport()->repaint();//repaint(0,0, width(), height()) ;

}








extern char ExtCharsTable[100] ;



void QScrollScreenArea::keyPressEvent( QKeyEvent *e )
{
     char key, extkey=0;

     if(e->key() == Qt::Key_CapsLock) return ;
     if(e->key() == Qt::Key_NumLock) return ;
     if(e->key() == Qt::Key_ScrollLock) return ;

     if(e->key() == Qt::Key_Alt)
     {
         iModifierPressed|=ALT_mask;
         return ;
     }
     if(e->key() == Qt::Key_Shift)
     {
         iModifierPressed|=SHIFT_mask;
         return ;
     }
     if(e->key() == Qt::Key_Control)
     {
         iModifierPressed|=CTRL_mask;
         return ;
     }

     if(e->key() == 0)
     {
         return ;
     }

    keyboardMutex.lock();

    int scankey = e->nativeScanCode() ;

    quint32 iModifier = e->nativeModifiers() ;
    extkey=e->key() ;

    int CodeChar = *codec->fromUnicode((QString)(e->text()));

    if(0 != (iModifierPressed&ALT_mask))
    {
        switch(CodeChar)
        {
        case 'V':
        case 'v':  CodeChar = 7 ; break ;
        default: ;
        }

        switch(e->nativeScanCode())
        {
        case 96: qrlisp->showOptions();  // ALT-F12
            keyboardMutex.unlock();  return ;
        default: ;
        }
    }



    if(0 != (iModifierPressed&SHIFT_mask))
    {
        switch(e->key())
        {
        case Qt::Key_Insert :
            {
            mimeData = clipboard->mimeData();

                /*if (mimeData->hasImage()) { }
                else if (mimeData->hasHtml()) { }
                else */
                if (mimeData->hasText())
                {
                    //setText(mimeData->text());
                    //setTextFormat(Qt::PlainText);
                    int len = mimeData->text().length() ;
                    strncpy(&Symbol[keypressed],
                            mimeData->text().toLocal8Bit().data(),len) ;
                    keypressed += len ;

                    keyboardMutex.unlock();
                    return ;
                }
            }
            break ;
        default: ;
        }
    }

    Symbol[keypressed]=CodeChar & 0xff ;
    keypressed++;

    if(CodeChar==0)
    {
        Symbol[keypressed]=extkey ;
        keypressed++;
    }


   if(qrlisp->iUseTextWindow==1)
   {
       sprintf(cBuffer,"ascii=%d,  key=%d  keytab=0x%X = %d     n=%d",
               e->nativeScanCode(),extkey, CodeChar&0xff, CodeChar&0xff,keypressed-ikeyread);
       qrlisp->textBrowser_tcp->append(cBuffer) ;
         //statusLabel.setText(Abuff);
     }

     keyboardMutex.unlock();
}




void QScrollScreenArea::keyReleaseEvent( QKeyEvent * e )
{
     char key, extkey=0;

     if(e->key() == Qt::Key_CapsLock) return ;
     if(e->key() == Qt::Key_NumLock) return ;
     if(e->key() == Qt::Key_ScrollLock) return ;

     if(e->key() == Qt::Key_Alt)
     {
         iModifierPressed&=~ALT_mask;
         return ;
     }
     if(e->key() == Qt::Key_Shift)
     {
         iModifierPressed&=~SHIFT_mask;
         return ;
     }
     if(e->key() == Qt::Key_Control)
     {
         iModifierPressed&=~CTRL_mask;
         return ;
     }

}


char QScrollScreenArea::GETCH()
{
     int inkeys;
     char key;

 //    redraw();
 // keyboardMutex.lock();

     //if(keypressed<0) keypressed=0;

     //inkeys=keypressed-ikeyread;

     /*while(keypressed==0)
     {
         usleep(200);
         //inkeys=keypressed-ikeyread;
     }
     usleep(200);*/
 //    printf("\nLSP::GETCH   %c    %d                %d", Symbol[ikeyread],Symbol[ikeyread], inkeys);
     keyboardMutex.lock();

     if(keypressed == 0)
     {
         keyboardMutex.unlock();
         return 0 ;
     }

     key=Symbol[ikeyread];


     ikeyread++;

     if(keypressed==ikeyread)
     {
          keypressed=0;
          ikeyread=0;
     }

     keyboardMutex.unlock();



  return key ;
};



int QScrollScreenArea::KBHIT()
{
    int n ;

     keyboardMutex.lock();
     n = keypressed - ikeyread ;
     keyboardMutex.unlock();

    return n ;
};



