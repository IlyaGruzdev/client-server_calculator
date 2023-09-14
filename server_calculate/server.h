#ifndef SERVER_H
#define SERVER_H
#include <QString>
#include <QSet>
#include <QDebug>
#include <QList>
#include <string>
#include <QMap>
#include <algorithm>
#include <qalgorithms.h>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QByteArray>
#include <QtMath>
#include <cmath>
class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server();
   ~Server();
public:
    QString parser(QString str);
     QVector <QString> scaner(QString str);
private:
     QTcpSocket *socket=nullptr;
     QMap <int, QTcpSocket *> sockets_map;
private slots:
     QString id(QString st,QSet <char> operators);
     void sendToClient();
    void incomingConnection(qintptr socketDescriptor);
    void deleteLater();
};

#endif // SERVER_H
