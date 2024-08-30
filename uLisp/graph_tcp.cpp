#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/stat.h>

#include "arrays.h"
#include "graph_tcp.h"

#define BUFSIZE 1024





static   int sockfd, portno, n;
static   struct sockaddr_in serveraddr;
static   struct hostent *server;
static   char hostname[256] = "127.0.0.1" ;
static   char buf[BUFSIZE];
static   char command[BUFSIZE];
static   char FileBuffer[BUFSIZE] ;
static   char command_line[BUFSIZE] ;



static int iScreenMaxX, iScreenMaxY;
static int iCharHeight, iCharWidth ;

static char EscCmd[10] ;
static uint8_t uiEscCmd_len, uiEscCmd_index ;
static int iInversionSymbol = 0 ;

static int iCurrentX, iCurrentY ;
static int iCurrentColor, iCurrentBkColor, iCurrentCharSize ;
static int iCursorOn = 1, idrawcursor = 0 ;

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}


long GetFileSize(char* filename)
{
    struct stat stat_buf;
    int rc = stat(filename, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}








int read_answear ()
{
    buf[0] = 0 ;
    n = read(sockfd, buf, BUFSIZE);
    if(strncmp(buf,"OK",2)==0)
        return 1 ;

    return 0 ;
}


void tcp_setTextColor (int color)
{
    sprintf(buf, "setcolorb") ;
    *(int*)(&buf[9]) = ((color&0x3f)<<3) +((color&0x7c0)<<5)  +((color&0xf800)<<8);
    n = write(sockfd, buf, 13);
    read_answear() ;
}

void tcp_setTextBkColor (int color)
{
    sprintf(buf, "setbkcolorb") ;
    *(int*)(&buf[11]) = ((color&0x3f)<<3) +((color&0x7c0)<<5)  +((color&0xf800)<<8);
    n = write(sockfd, buf, 15);
    read_answear() ;
}


void tcp_setRegularTextColors (int color, int bkcolor)
{
    iCurrentColor = color ;
    iCurrentBkColor = bkcolor ;

    tcp_setTextBkColor(bkcolor);
    tcp_setTextColor(color);
}



void tcp_setCursor(int x, int y)
{
    //sprintf(buf,"gotoxy %d %d:", x,y) ;
    strcpy(buf,"gotoxyb") ;
    *(int*)(&buf[7]) = x ;
    *(int*)(&buf[11]) = y ;

    n = write(sockfd, buf, 15);
    read_answear() ;

    /*if(iCursorOn==1)
    {
        if(!idrawcursor)
        {
            idrawcursor = 1 ;
            int ix=tcp_getx();
            int iy=tcp_gety();
            tcp_drawsymbol('_') ;
            tcp_setCursor(ix,iy);
            idrawcursor = 0 ;
        }
    }*/
}


void tcp_drawsymbol (int c)
{
    int ix, iy ;
    int V100_EscProcessing(unsigned int c) ;

    if(V100_EscProcessing((unsigned int)c) == 1) return ;


    ix=tcp_getx();
    iy=tcp_gety();


    if(((c&0xff) == 13)||((c&0xff) == 10))
    {
        ix = 0 ;
        iy += iCharHeight;
        if(iy>=(iScreenMaxY-iCharHeight))
        {
            iy = 0 ;
        }

        tcp_setCursor(ix,iy);
        return ;
    }

    if((c&0xff) == 12)
    {

        tcp_fillScreen(iCurrentBkColor);

        tcp_setCursor(0,0);

        return ;
    }

    if(c==8)
    {

        ix -= iCharWidth ;
        if(ix<=0)
        {
            ix = (iScreenMaxX/iCharWidth) ;
            ix *= iCharWidth ;
            iy -=  iCharHeight ;
            if( iy<0 )
            {
                iy = (iScreenMaxY/iCharHeight) ;
                iy *= iCharHeight ;
            }
        }
        tcp_setCursor(ix,iy);

        return ;
    }

    if(ix>=iScreenMaxX-iCharWidth)
    {
        ix = 0 ;
        iy += iCharHeight ;
        if(iy>=iScreenMaxY-iCharHeight) iy = 0 ;
        tcp_setCursor(ix,iy);
    }

    /*if(iInversionSymbol==1)
    {
        tcp_setTextColor(iCurrentBkColor) ;
        tcp_setTextBkColor(iCurrentColor) ;
    }*/

    strcpy(buf,"putcb" ) ;
    *(int*)(&buf[5]) = c;
    n = write(sockfd, buf, 9);
    read_answear() ;

}



void tcp_drawPixel (int x, int y, int color)
{
    //sprintf(buf,"point %d %d:", x,y) ;
    //n = write(sockfd, buf, strlen(buf));

    strcpy(buf,"pointb") ;
    *(int*)(&buf[6]) = x ;
    *(int*)(&buf[10]) = y ;

    n = write(sockfd, buf, 14);
    read_answear() ;
}

void tcp_drawLine (int x1, int y1, int x2, int y2, int color)
{
//    sprintf(buf,"line %d %d %d %d %d:", x1,y1, x2,y2, color) ;
//    n = write(sockfd, buf, strlen(buf));
    strcpy(buf,"lineb") ;
    *(int*)(&buf[5]) = x1 ;
    *(int*)(&buf[9]) = y1 ;
    *(int*)(&buf[13]) = x2 ;
    *(int*)(&buf[17]) = y2 ;

    n = write(sockfd, buf, 21);
    read_answear() ;
}

void tcp_drawRect(int x1, int y1, int x2, int y2, int color)
{
    tcp_setTextColor(color) ;

//    sprintf(buf,"rectangle %d %d %d %d:", x1,y1, x2,y2) ;
//    n = write(sockfd, buf, strlen(buf));
    strcpy(buf,"rectangleb") ;
    *(int*)(&buf[10]) = x1 ;
    *(int*)(&buf[14]) = y1 ;
    *(int*)(&buf[18]) = x2 ;
    *(int*)(&buf[22]) = y2 ;

    n = write(sockfd, buf, 26);
    read_answear() ;
}

void tcp_fillRect(int x1, int y1, int x2, int y2, int color)
{
    tcp_setTextColor(color) ;

    //sprintf(buf,"bar %d %d %d %d:", x1,y1, x2,y2) ;
    //n = write(sockfd, buf, strlen(buf));
    strcpy(buf,"barb") ;
    *(int*)(&buf[4]) = x1 ;
    *(int*)(&buf[8]) = y1 ;
    *(int*)(&buf[12]) = x2 ;
    *(int*)(&buf[16]) = y2 ;

    n = write(sockfd, buf, 20);

    read_answear() ;
}

void tcp_drawCircle(int x, int y, int r, int color)
{
    tcp_setTextColor(color) ;

    //sprintf(buf,"ellipce %d %d %d %d:", x-r,y-r, r,r) ;
    //n = write(sockfd, buf, strlen(buf));
    strcpy(buf,"ellipceb") ;
    *(int*)(&buf[8]) = x-r ;
    *(int*)(&buf[12]) = y-r ;
    *(int*)(&buf[16]) = r ;
    *(int*)(&buf[20]) = r ;
    n = write(sockfd, buf, 24);

    read_answear() ;
}

void tcp_fillCircle(int x, int y, int r, int color)
{
    tcp_setTextColor(color) ;
    //iCurrentColor = color ;

    //sprintf(buf,"ellipcf %d %d %d %d:", x-r,y-r, r,r) ;
    //n = write(sockfd, buf, strlen(buf));
    strcpy(buf,"ellipcfb") ;
    *(int*)(&buf[8]) = x-r ;
    *(int*)(&buf[12]) = y-r ;
    *(int*)(&buf[16]) = r ;
    *(int*)(&buf[20]) = r ;
    n = write(sockfd, buf, 24);

    read_answear() ;
}

void tcp_drawRoundRect(int x1, int y1, int x2, int y2, int r, int color)
{
    tcp_setTextColor(color) ;
    //iCurrentColor = color ;

    //sprintf(buf,"rectround %d %d %d %d %d :", x1,y1, x2,y2, r) ;
    //n = write(sockfd, buf, strlen(buf));
    strcpy(buf,"rectroundb") ;
    *(int*)(&buf[10]) = x1 ;
    *(int*)(&buf[14]) = y1 ;
    *(int*)(&buf[18]) = x2 ;
    *(int*)(&buf[22]) = y2 ;
    *(int*)(&buf[26]) = r ;
    n = write(sockfd, buf, 30);

    read_answear() ;
}

void tcp_fillRoundRect(int x1, int y1, int x2, int y2, int r, int color)
{
    tcp_setTextColor(color) ;
    //iCurrentColor = color ;

    //sprintf(buf,"barround %d %d %d %d %d :", x1,y1, x2-x1,y2-y1, r) ;
    //n = write(sockfd, buf, strlen(buf));
    strcpy(buf,"barroundb") ;
    *(int*)(&buf[9]) = x1 ;
    *(int*)(&buf[13]) = y1 ;
    *(int*)(&buf[17]) = x2 ;
    *(int*)(&buf[21]) = y2 ;
    *(int*)(&buf[25]) = r ;
    n = write(sockfd, buf, 29);
    read_answear() ;
}

void tcp_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color)
{
}

void tcp_fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color)
{
}



void tcp_drawChar(int x, int y, int ch, int color, int bkcolor, int size)
{
    tcp_setTextColor(color) ;

    tcp_setTextBkColor(bkcolor) ;

    //iCurrentColor = color ;
    //iCurrentBkColor = bkcolor ;

    tcp_setCursor(x,y) ;

    //sprintf(buf, "putc %c:", ch) ;
    //n = write(sockfd, buf, strlen(buf));
    strcpy(buf, "putcb") ;
    *(int*)(&buf[5]) = ch;
    n = write(sockfd, buf, 9);

    read_answear() ;
}


void tcp_setTextSize(int sz)
{
}

void tcp_setTextWrap(int On)
{
}

void tcp_fillScreen(int color)
{
    tcp_setTextBkColor(color) ;
    //iCurrentBkColor = color ;

    sprintf(buf,"clrscr:") ;
    n = write(sockfd, buf, strlen(buf));
    read_answear() ;

}

void tcp_setRotation(int On)
{
}

void tcp_invertDisplay(int On)
{
}



/***********************************************************************************
 *
 *
 *
************************************************************************************/




int GetFile(char *file, char *file_dest)
{
    int n ;
    char command[512] ;

    sprintf(command, "get %s", file) ;

            /* send the message line to the server */
            n = write(sockfd, command, strlen(command));
            if (n < 0)
                error((char*)"ERROR writing to socket");

            bzero(buf, BUFSIZE);
            n = read(sockfd, buf, BUFSIZE);
            if(n>0)
            {
                if(strcmp(buf, "file not found")==0)
                {
                     printf("%s",buf) ;
                     //puts("file not found\n") ;
                     fflush(stdout) ;
                }
                else
                {

                    if(strncmp(buf, "START",5)==0)
                    {
                        int len, n ;

                        sscanf(&buf[5],"%d", &len ) ;

                        FILE *fp ;

                        fp = fopen(file_dest,"wb");
                        if(fp != NULL)
                        {

                            while(len>0)
                            {
                                strcpy(buf,"Next") ;

                                write(sockfd, buf, strlen(buf)) ;

                                n = read(sockfd, FileBuffer, BUFSIZE);

                                fwrite(FileBuffer,1,n,fp);
                                len -= n ;
                            }
                            fclose(fp) ;
                            puts("file copied") ;
                            fflush(stdout) ;
                        }
                    }
               }
            }

    return 0 ;

}


int PutFile(char *file, char *file_source)
{
    FILE *fp ;
    int len ;
    char command[512] ;

    sprintf(command, "put %s ", file) ;

            len = GetFileSize(file_source) ;

            printf("copy %s to %s\n",file_source, file) ;
            fflush(stdout) ;

            fp = fopen(file_source,"rb");
            if(fp != NULL)
            {

                int n ;

                sprintf(buf," %d",  len  ) ;
                strcpy(&command[strlen(command)-1],buf) ;

                /* send the message line to the server */
                n = write(sockfd, command, strlen(command));
                if (n < 0)
                    error((char*)"ERROR writing to socket");
                //printf(command) ;
                //fflush(stdout);


                bzero(buf, BUFSIZE);
                n = read(sockfd, buf, BUFSIZE);

                if(n>0)
                {
                    if(strcmp(buf, "cannot open file")==0)
                    {
                        printf("%s",buf) ;
                        fflush(stdout) ;
                    }
                    else
                    {

                        //int len2 ;

                        while(strncmp(buf, "ACCEPT",6)==0)
                        {

                            //sscanf(&buf[6],"%d", &len2 ) ;

                            if(len>0)
                            {
                                int n2 = BUFSIZE ;

                                if( n2>len ) n2 = len ;

                                n = fread(FileBuffer,1,n2,fp);
                                write(sockfd, FileBuffer, n ) ;
                                len -= n ;

                                bzero(buf, BUFSIZE);
                                n = read(sockfd, buf, BUFSIZE);
                            }
                        }
                        puts("file copied") ;
                        fflush(stdout) ;
                   }
                    fclose(fp) ;
               }
            }
            else
            {
                        puts("file not exists") ;
                        fflush(stdout) ;

            }
    return 0 ;
}



int tcp_getmaxx ()
{
    int i ;

    sprintf(buf,"getmaxxb:") ;
    n = write(sockfd, buf, strlen(buf));

    buf[0] = 0 ;
    n = read(sockfd, &i, sizeof(int));

    return i ;
}

int tcp_getmaxy ()
{
    int i ;

    sprintf(buf,"getmaxyb:") ;
    n = write(sockfd, buf, strlen(buf));

    buf[0] = 0 ;
    n = read(sockfd, &i, sizeof(int));

    return i ;
}


int tcp_getx ()
{
    int i ;

    sprintf(buf,"getxb:") ;
    n = write(sockfd, buf, strlen(buf));

    buf[0] = 0 ;
    n = read(sockfd, &i, sizeof(int));

    return i ;
}

int tcp_gety ()
{
    int i ;

    sprintf(buf,"getyb:") ;
    n = write(sockfd, buf, strlen(buf));

    buf[0] = 0 ;
    n = read(sockfd, &i, sizeof(int));

    return i ;
}



int tcp_getfontheight ()
{
    int i ;

    sprintf(buf,"chrheightb:") ;
    n = write(sockfd, buf, strlen(buf));

    buf[0] = 0 ;
    n = read(sockfd, &i, sizeof(int));

    return i ;
}

int tcp_getfontwidth ()
{
    int i ;

    sprintf(buf,"chrwidthb:") ;
    n = write(sockfd, buf, strlen(buf));

    buf[0] = 0 ;
    n = read(sockfd, &i, sizeof(int));

    return i ;
}



int InitTcpGraphics()
{
    portno = 1500 ;

    //hostname = "127.0.0.1";

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error((char*)"ERROR opening socket");
        return -1;
    }

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        return -1;
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
      (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, (const sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
      error((char*)"ERROR connecting");
      return -1;
    }

    iScreenMaxX = tcp_getmaxx() ;
    iScreenMaxY = tcp_getmaxy() ;

    iCharHeight = tcp_getfontheight() ;
    iCharWidth = tcp_getfontwidth() ;

    tcp_fillScreen(0);

    //tcp_setTextColor() ;
    tcp_setRegularTextColors(0xffff, 0) ;

    printf("Connected to %s\n\r", hostname);

    /*tcp_fillRect(10,10,100,100, 6000) ;*/

    tcp_setCursor(10,10) ;

    return 1;
}





typedef struct smouse_state{
    int mouseX ;
    int mouseY ;
    int LeftButtonState, MidButtonState, RightButtonState ;
} mouse_state_type ;

static mouse_state_type mouse_state ;



int MouseStateButtons ()
{
    sprintf(buf,"getmousestat:") ;
    n = write(sockfd, buf, strlen(buf));

    buf[0] = 0 ;
    n = read(sockfd, &mouse_state, sizeof(int));

return mouse_state.LeftButtonState ;
}

int MouseStateX ()
{
   return  mouse_state.mouseX ;
}

int MouseStateY ()
{
    return  mouse_state.mouseY ;
}


int tcp_kbhit ()
{
    int i ;

    sprintf(buf,"kbhitb:") ;
    n = write(sockfd, buf, strlen(buf));

    buf[0] = 0 ;
    n = read(sockfd, &i, sizeof(int));

    return i ;
}



#define tcp_and_console_keyboard


int tcp_getch ()
{
    int i ;
    char getch(void);

    char gserial_key = -1 ;//getch() ;

    while((tcp_kbhit() == 0 ) && (gserial_key == -1))
    {
#ifdef tcp_and_console_keyboard
        gserial_key = getch() ;
#endif
        usleep(2000);
    }

    if(gserial_key != -1)
        return gserial_key ;


    sprintf(buf,"getchb:") ;
    n = write(sockfd, buf, strlen(buf));

    buf[0] = 0 ;
    n = read(sockfd, &i, sizeof(int));

    return i ;
}








int V100_EscProcessing(unsigned int c)
{
    if((uiEscCmd_index == 0)&&(c == 27))
    {
        EscCmd[uiEscCmd_index] = 27 ;
        uiEscCmd_index ++ ;

        return 1 ;
    }

    if(uiEscCmd_index > 0)
    {
        EscCmd[uiEscCmd_index] = 0x0ff&c ;
        uiEscCmd_index ++ ;


        if(uiEscCmd_index == 2)
        {
            switch(EscCmd[1])
            {
            case 'D':

                uiEscCmd_index = 0 ;
                return 1 ;
            case 'M':

                uiEscCmd_index = 0 ;
                return 1 ;
            case 'E':

                uiEscCmd_index = 0 ;
                return 1 ;
            default : return 1 ;
            }
        }
        if(uiEscCmd_index == 3)
        {

            if(EscCmd[1]=='[')
              switch(EscCmd[2])
            {
            case 'f':
            case 'H':
                //V100_PutCursor( 0 ) ;
                //iV100_CursorCol = 0 ;
                //V100_PutCursor( 1 ) ;
                uiEscCmd_index = 0 ;
                return 1 ;
            case 'K': // Erase to end of line
                {

                }
                uiEscCmd_index = 0 ;
                return 1 ;

            case 'J': // Erase to end of screen

                uiEscCmd_index = 0 ;
                return 1 ;

            //case '?':     //ESC [ ? 6 h  turn on region - origin mode
            //case '?':     //ESC [ ? 6 l  turn off region - full screen mode
                return 1 ;
            default : return 1 ;
            }
        }
        if(uiEscCmd_index == 4)
        {

            if(EscCmd[1]=='[')
              switch(EscCmd[3])
            {
            case 'm':
                   if(EscCmd[2]=='7')
                   {
                       tcp_setTextColor(iCurrentBkColor) ;
                       tcp_setTextBkColor(iCurrentColor) ;

                       iInversionSymbol = 1 ;
                   }
                   if(EscCmd[2]=='0')
                   {
                       tcp_setTextColor(iCurrentColor) ;
                       tcp_setTextBkColor(iCurrentBkColor) ;
                       iInversionSymbol = 0 ;
                   }
                    uiEscCmd_index = 0 ;
                    return 1 ;
            case 'K': // ESC+***+K Erase to end of line
                if(EscCmd[2]=='0')
                {

                }
                if(EscCmd[2]=='1')
                {

                }
                if(EscCmd[2]=='2')
                {

                }

                uiEscCmd_index = 0 ;
                return 1 ;

            case 'J': // ESC+***+J Erase to end of screen
                if(EscCmd[2]=='0')
                {
                    uiEscCmd_index = 0 ;
                    return 1 ;

                }
                if(EscCmd[2]=='1')	// Erase to begin of screen
                {
                    uiEscCmd_index = 0 ;
                    return 1 ;

                }
                if(EscCmd[2]=='2')	// Erase  screen
                {
                uiEscCmd_index = 0 ;
                return 1 ;
                }

            case 'A':	 //  ESC [ pn A        cursor up pn times - stop at top
                //V100_setcursor(iV100_CursorCol, iV100_CursorRow - EscCmd[2] ) ;
               {
                    int  ix=tcp_getx();
                    int  iy=tcp_gety();

                    tcp_setCursor(ix, iy-(iCharHeight*EscCmd[2])) ;
                }

                uiEscCmd_index = 0 ;
                return 1 ;
            case 'B':	 //  ESC [ pn B        cursor down pn times - stop at bottom
                //V100_setcursor(iV100_CursorCol, iV100_CursorRow + EscCmd[2] ) ;
              {
                   int  ix=tcp_getx();
                   int  iy=tcp_gety();

                   tcp_setCursor(ix, iy+(iCharHeight*EscCmd[2])) ;
               }

                uiEscCmd_index = 0 ;
                return 1 ;
            case 'C':	 //  ESC [ pn C        cursor right pn times - stop at far right
                //V100_setcursor(iV100_CursorCol + EscCmd[2], iV100_CursorRow ) ;
              {
                   int  ix=tcp_getx();
                   int  iy=tcp_gety();

                   tcp_setCursor(ix+(iCharWidth*EscCmd[2]), iy) ;
               }

                uiEscCmd_index = 0 ;
                return 1 ;
            case 'D':	 //  ESC [ pn D        cursor left pn times - stop at far left
                //V100_setcursor(iV100_CursorCol - EscCmd[2], iV100_CursorRow ) ;
              {
                   int  ix=tcp_getx();
                   int  iy=tcp_gety();

                   tcp_setCursor(ix-(iCharWidth*EscCmd[2]), iy) ;
               }

                uiEscCmd_index = 0 ;
                return 1 ;


            default : return 1 ;
            }
        }
        if(uiEscCmd_index == 5)
        {
            //case '?':     //ESC [ ? 6 h  turn on region - origin mode
            //case '?':     //ESC [ ? 6 l  turn off region - full screen mode
            if(strncmp(&EscCmd[1],"{?6h",4)==0)
            {
                uiEscCmd_index = 0 ;
                return 1 ;
            }
            if(strncmp(&EscCmd[1],"{?6l",4)==0)
            {
                uiEscCmd_index = 0 ;
                return 1 ;
            }
            if(strncmp(&EscCmd[1],"{?1h",4)==0)
            {
                uiEscCmd_index = 0 ;
                return 1 ;
            }
            if(strncmp(&EscCmd[1],"{?1l",4)==0)
            {
                uiEscCmd_index = 0 ;
                return 1 ;
            }

            if((EscCmd[1]=='[')&&(EscCmd[2]=='?'))
            {
                if(EscCmd[3]==25)
                {
                    if(EscCmd[4]=='h') iCursorOn = 1 ;
                    if(EscCmd[4]=='l') iCursorOn = 0 ;
                }

                uiEscCmd_index = 0 ;
                return 1 ;
            }
            if(strncmp(&EscCmd[1],"[?6l",4)==0)
            {
                uiEscCmd_index = 0 ;
                return 1 ;
            }
        }

        if(uiEscCmd_index == 6)
        {
            if(EscCmd[3]==';')
            {
                //if(EscCmd[5]=='r') ; // Set scroll region
                if((EscCmd[5]=='H')||(EscCmd[5]=='f'))
                {
                    //ESC [ pl ; pc f   set cursor position - pl Line, pc Column
                    //V100_setcursor(EscCmd[2], EscCmd[4] ) ;
                    tcp_setCursor((iCharWidth*EscCmd[4]), (iCharHeight*EscCmd[2])) ;

                }
            }
            if(EscCmd[1]=='[')
            {
                //if(EscCmd[5]=='r') ; // Set scroll region
                int  ix=tcp_getx();
                int  iy=tcp_gety();

                int ishift = 100*(EscCmd[2]-'0')+10*(EscCmd[3]-'0')+(EscCmd[4]-'0');

                if(EscCmd[5]=='D')
                    tcp_setCursor(ix-ishift*iCharWidth, iy) ;

                if(EscCmd[5]=='C')
                    tcp_setCursor(ix+ishift*iCharWidth, iy) ;


                if(EscCmd[5]=='A')
                    tcp_setCursor(ix, iy+ishift*iCharHeight) ;

                if(EscCmd[5]=='B')
                    tcp_setCursor(ix, iy-ishift*iCharHeight) ;

            }

            uiEscCmd_index = 0 ;
            return 1 ;
        }
        return 1 ;
    }

    return 0 ;
}



char *tcp_getimage(int x,int y,int w,int h )
{
    void filldimensionsteps(array_desc_t *descriptor) ;

    strcpy(buf,"imggetb") ;
    *(int*)(&buf[7]) = x ;
    *(int*)(&buf[11]) = y ;
    *(int*)(&buf[15]) = w ;
    *(int*)(&buf[19]) = h ;

    n = write(sockfd, buf, 23);

    n = read(sockfd, buf, 4);
    int32_t sz = *(int32_t*)(buf) ;

    array_desc_t desctriptor ;

    desctriptor.size = sz ;
    desctriptor.dim[0] = h ;
    desctriptor.dim[1] = w ;
    desctriptor.dim[2] = sz / (w*h) ;
    desctriptor.ndim = 3 ;
    desctriptor.type = CHAR ;
    desctriptor.element_size = 1 ;

    filldimensionsteps(&desctriptor) ;

    char *cPtr = (char *)malloc(desctriptor.size+sizeof(array_desc_t)) ;
    *(array_desc_t*)cPtr = desctriptor ;

    char *cPtr2 = cPtr + sizeof(array_desc_t) ;

    int iCount = 1000 ;

    while((sz>0)&&(iCount>0))
    {
        n = read(sockfd, cPtr2, sz);
        sz -= n ;
        cPtr2 += n ;
        if(n==0) iCount-- ;
    }


    return cPtr ;
}


void delay (int ms);


int tcp_putimage(int x, int y, char  *array )
{
    array_desc_t *desctriptor = (array_desc_t*)array ;
    int32_t sz = desctriptor->size ;
    int h = desctriptor->dim[0] ;
    int w = desctriptor->dim[1] ;

    array += sizeof(array_desc_t) ;

    strcpy(buf,"imgputb") ;
    *(int*)(&buf[7]) = x ;
    *(int*)(&buf[11]) = y ;
    *(int*)(&buf[15]) = w ;
    *(int*)(&buf[19]) = h ;
    *(int*)(&buf[23]) = sz ;

    n = write(sockfd, buf, 27);

    //delay(10);
    //buf[0] = 0 ;
    n = read(sockfd, buf, 5);

    if(strncmp(buf,"WA",2)==0)
    {
        n = write(sockfd, array, sz);
        array += n ;
    }

    return read_answear() ;
}
