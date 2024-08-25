#ifndef QSCROLLSCREENAREA_H
#define QSCROLLSCREENAREA_H

#include <QAbstractScrollArea>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QScrollBar>
#include <QPixmap>
#include <QImage>
#include <QRect>
#include <QTimer>
#include <QEvent>

//#include "qrlisp.h"

/*namespace Ui {
    class QScrollScreenArea;
}*/

class QScrollScreenArea : public QAbstractScrollArea
{
    Q_OBJECT

public:
    explicit QScrollScreenArea(QWidget *parent = 0);
    ~QScrollScreenArea();


    //char Abuff[100];
    char cBuffer[256] ;

    void SetPixmap( QImage *pixmap ) ;

    QImage *Pixmap ;

    QTextCodec *codec;

    int volatile keypressed ;
    int volatile iModifierPressed ;
    int volatile ikeyread ;
    int AddPointsCount ;
    char Symbol[512];

    int LeftButtonState, MidButtonState, RightButtonState ;
    int mouseY ;
    int mouseX ;

public:
    char GETCH();
    int KBHIT();
protected:

    virtual void mousePressEvent( QMouseEvent * e );
    virtual void mouseReleaseEvent( QMouseEvent * e );
    virtual void mouseMoveEvent( QMouseEvent * e );
    virtual void paintEvent( QPaintEvent * event ) ;
    virtual void resizeEvent( QResizeEvent * event ) ;
    virtual void timerEvent( QTimerEvent * event ) ;
    virtual void keyPressEvent( QKeyEvent * event ) ;
    virtual void keyReleaseEvent( QKeyEvent * event ) ;

private:
    //Ui::QScrollScreenArea *ui;
private slots:

public slots:

};

#endif // QSCROLLSCREENAREA_H
