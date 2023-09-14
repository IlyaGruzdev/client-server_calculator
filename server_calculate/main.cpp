#include <QCoreApplication>
#include <server.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString str="sin0*(sin0+cos0)*cos8";
    Server *s=new Server;
//   qDebug()<<s->parser(str);
    return a.exec();
}
