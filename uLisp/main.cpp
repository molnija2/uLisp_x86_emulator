#include <QCoreApplication>


void loop();
void setup();
void DeleteWorkspace();

char *file_name ;
char file_name_org[] = "test_program.l" ;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);



    if(argc > 1) file_name = argv[1] ;
    else file_name = file_name_org ;

        setup() ;

    loop();
    DeleteWorkspace();

    return a.exec();
    //return 0;
}
