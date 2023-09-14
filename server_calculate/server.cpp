#include "server.h"
Server::Server()
{
    if (this->listen(QHostAddress::Any, 8080))
    {

        qDebug()<<"Server start!";
    }
    else
        qDebug()<<"Server is not start!";
}
Server::~Server()
{
    delete socket;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    if(!sockets_map.keys().contains(socketDescriptor)){
    socket=new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    QObject::connect(socket, &QTcpSocket::readyRead,this, &Server::sendToClient);
    connect(socket, &QTcpSocket::disconnected, this, &Server::deleteLater);
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(er(QAbstractSocket::SocketError)));
    sockets_map.insert(socketDescriptor, socket);
//    socket->write("Hi, Client! I'm Server!");
   qDebug()<<"Client connection. Descriptor: "<<socketDescriptor;
    }
}
QString probel(QString s)
{
  while (s[0]==' ')
      s.remove(0,1);
  return s;
}
unsigned long long fact(unsigned int digit)
{
    return (digit==2 || digit== 1)?digit:digit*fact(digit-1);
}
QString Server::id(QString st,QSet <char> operators)
{
    st=probel(st);
    bool key = true;
    QRegExp regular("^√?s?c?l?!?\\d*\\.?\\d+(e\\d+)?$");
    QString id="";
    int i=0;
    while (i < st.length())
    {
        if (operators.contains((st[i]).toLatin1()) || st[i]==' ')
        {
            if (id.contains(regular)){
                return id;
            }
            else{
                QString str="Error digit " + id;
                qDebug()<<"Error digit " + id;
                socket->write(str.toStdString().c_str());
                return "E";
            }
        }
        else {
            if (!(st[i]=='(')){
            id+=st[i];
        }
    }
        i++;
    }
    if (id.contains(regular)){
        return id;
    }
    else
        return "Error digit " + id;
}

QVector <QString> Server::scaner(QString stroka)
{
    stroka.push_back(')');
   stroka=probel(stroka);
   stroka=stroka.replace("lg", "l");
   stroka=stroka.replace("sin", "s");
   stroka=stroka.replace("cos","c");
  QString word="";
  QVector<QString> mas;
  mas.append("(");
  QSet <char> conteiner={'+', '-', '*', '/', ')', '^'};
  while (!stroka.isEmpty())
  {
      if (((!conteiner.contains(stroka[0].toLatin1()) && stroka[0]!='(')||(stroka[0]=='-' && mas.constLast()=="(")) &&!(stroka[0]=='!' && stroka[1]=='(') && !(stroka[0]=='l' && stroka[1]=='(') && !(stroka[0]=='s' && stroka[1]=='(') && !(stroka[0]=='c' && stroka[1]=='(') && !(stroka[0]==8730 && stroka[1]=='('))
      {
        bool key=false;
        if (mas.constLast()=="(")
        {
             stroka=probel(stroka);
            if(stroka[0]=='-')
            {
            stroka.remove(0,1);
            key=true;
            }
        }
        word=id(stroka, {'+', '-', '*', '/',' ', ')', '^'});
        int length=word.length();
        if (word[0]=='E')
            return {};
        if (word[0]=='!'){
            word.remove(0,1);
            if (word[0]=='-')
            {
                QString str="Error digit "+ word;
                socket->write(str.toStdString().c_str());
                return {};
            }
            if (round(word.toDouble())!=word.toDouble())
            {
                QString str="Fact can't have double value:"+ word;
                socket->write(str.toStdString().c_str());
                return {};
            }
            word=QString::number(fact(word.toInt()));
        }
        if (word[0]=='l'){
            word.remove(0,1);
            if (word[0]=='-')
            {
                QString str="Error digit lg"+ word;
                socket->write(str.toStdString().c_str());
                return {};
            }
            word=QString::number(log10(word.toDouble()));
        }
        if (word[0]==8730){
            word.remove(0,1);
            if (word[0]=='-')
            {
                QString str="Error digit √"+ word;
                socket->write(str.toStdString().c_str());
                return {};
            }
            word=QString::number(sqrt(word.toDouble()));
        }
        if (word[0]=='s'){
            word.remove(0,1);
            word=QString::number(qSin(word.toDouble()));
        }
        if (word[0]=='c'){
            word.remove(0,1);
            word=QString::number(qCos(word.toDouble()));
        }
        if (key){
        word.push_front('-');
        if(stroka[length-1]==')')
        {
            mas.removeLast();
           stroka.remove(0, length+1);
        }
        else
            stroka.remove(0,length);
        }
        else
        stroka.remove(0, length);
        mas.append(word);
        probel(stroka);
      }
      else
      {
          mas.append((QString)stroka[0]);
          stroka.remove(0, 1);
      }
  }
  qDebug()<<"True";
  qDebug()<<stroka;
  return mas;
}
QString Server::parser(QString str)
{
    if (str.contains(QRegExp("^-?\\d*\\.?\\d+(e\\d+)?$")))
        return str;
    QVector<QString> tokens;
    str=probel(str);
    tokens=scaner(str);
    if (tokens.isEmpty())
        return "";
    QMap <int, int > index_map;//first index-index of str, second index-number of str[i]
    QSet <char> cont={'+', '-', '*', '/', '^'};
    str.push_back(')');
    str.push_front('(');
    QVector<QString> digits;
    for (auto item: tokens)
    {
        if (item.contains(QRegExp("^-?\\d*\\.?\\d+(e\\d+)?$")))
            digits.push_back(item);
    }
     QRegExp reg("^(-?\\d*.?\\d+|\\d*.?\\d+e\\d+|\\d*.?\\d+e[+-]0?\\d+)$");
    if (tokens.length()>=1)
    while (!tokens.isEmpty()){
    int number=-1;
    index_map.clear();
    for (int i=0;i<tokens.length();i++)
    {
        if (tokens[i].contains(reg)||tokens[i]=="(")
            number++;
        if (tokens[i]==")")
            number--;
        if (tokens[i].length()==1)
        if (cont.contains(tokens[i].toStdString().c_str()[0]))
        {
            number--;
            index_map[i]=number;
        }
    }
    if (index_map.size()>=digits.size())
        return "not enough digits";
    if (index_map.size()<digits.size()-1)
        return "not enough operators";
    if (number<0)
        return "To much )";
    if (number>0)
        return "To much (";
    QList <int> list=index_map.values();
    std::sort(list.rbegin(), list.rend());
    if (!list.isEmpty()){
    int max=list[0];
    QList <int> mas=index_map.keys(max);
    bool key=false;
    int first_operator_index=mas[0];
    for (auto index: mas)
    {
        if (tokens[index]=="^")
        {
            key = true;
            if (tokens[index-1].contains(reg) && tokens[index+1].contains(reg))
            {
            tokens[index]=QString::number(qPow(tokens[index-1].toDouble(),tokens[index+1].toDouble()));
            tokens.remove(--index);
            tokens.remove(index+1);
            first_operator_index=index;
            digits.remove(0,1);
            break;
            }
        }

    }
    for (auto index: mas)
    {
        if(tokens[index]=="*")
        {
            key = true;
            if (tokens[index-1].contains(reg) && tokens[index+1].contains(reg))
            {
            tokens[index]=QString::number(tokens[index-1].toDouble()*tokens[index+1].toDouble());
            tokens.remove(--index);
            tokens.remove(index+1);
            first_operator_index=index;
            digits.remove(0,1);
            break;
            }
        }
        if(tokens[index]=="/")
        {
            key=true;
            if (tokens[index-1].contains(reg) && tokens[index+1].contains(reg) )
            {
            tokens[index]=QString::number(tokens[index-1].toDouble()/tokens[index+1].toDouble());
            tokens.remove(--index);
            tokens.remove(index+1);
            first_operator_index=index;
            digits.remove(0,1);
            break;
            }
        }
    }
    if (!key)
    {
        if (tokens[first_operator_index-1].contains(reg) && tokens[first_operator_index+1].contains(reg)){
        if (tokens[first_operator_index]=="+"){
        tokens[first_operator_index]=QString::number(tokens[first_operator_index-1].toDouble()+tokens[first_operator_index+1].toDouble());
        tokens.remove(--first_operator_index);
        tokens.remove(first_operator_index+1);
        digits.remove(0,1);
        }
        if (tokens[first_operator_index]=="-"){
            tokens[first_operator_index]=QString::number(tokens[first_operator_index-1].toDouble()-tokens[first_operator_index+1].toDouble());
            tokens.remove(--first_operator_index);
            tokens.remove(first_operator_index+1);
            digits.remove(0,1);
        }
        }
    }

    if (tokens[first_operator_index+1]==")" && tokens[first_operator_index-1]=="(")
    {
        tokens.remove(first_operator_index+1);
        tokens.remove(--first_operator_index);
        if (tokens.length()==1)
            return tokens[0];
    }
    int i=0;
    }   

        int i=0;

        while (i<tokens.length()-1)
        {
           if(tokens[i].contains(QRegExp(reg)))
           {
                if (tokens[i-1]=="!")
                {
                    tokens[i]=QString::number(fact(tokens[i].toInt()));
                    tokens.remove(i-1);
                    i--;
                }
                if (tokens[i-1]=="l")
                {
                    tokens[i]=QString::number(log10(tokens[i].toDouble()));
                    tokens.remove(i-1);
                    i--;
                }
                if (tokens[i-1]=="s")
                {
                    tokens[i]=QString::number(qSin(tokens[i].toFloat()));
                    tokens.remove(i-1);
                    i--;
                }
                if (tokens[i-1]=="c")
                {
                    tokens[i]=QString::number(qCos(tokens[i].toDouble()));
                    tokens.remove(i-1);
                    i--;
                }
                if (tokens[i-1]=="√")
                {
                    tokens[i]=QString::number(sqrt(tokens[i].toDouble()));
                    tokens.remove(i-1);
                    i--;
                }
                if (tokens[i-1]=="(" && tokens[i+1]==")")
                {
                    tokens.remove(i+1);
                    tokens.remove(--i);
                    i--;
                    if(tokens.length()==1)
                        return tokens[0];
                }
           }
           i++;
        }
}
    return "Uncorrect str";
}
void Server::sendToClient()
{

    QTcpSocket *s=(QTcpSocket*)sender();
    bool key=false;
    for (auto it=sockets_map.begin();it!=sockets_map.end();it++)
    {
        if (it.value()==s){
            key=true;
        }
    }
    if(!key)
        return;
   QByteArray data=s->readAll();
      QString str=QString(data);
      qDebug()<<"Read...\n";

      s->write(parser(str).toStdString().c_str());
}
void Server::deleteLater()
{
    QTcpSocket *delete_socket=qobject_cast<QTcpSocket *>(sender());
    for (auto it=sockets_map.begin();it!=sockets_map.end();it++)
    {
        if (it.value()==delete_socket){
            sockets_map.remove(it.key());
        }
    }
    qDebug()<<"Delete";
    delete_socket->deleteLater();
}

