#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket=new QTcpSocket();
    socket->connectToHost("127.0.0.1", 8080);
    setWindowTitle("TCP calculate");
    if (socket->isOpen())
        qDebug()<<"Sokcket is connected";
    QObject::connect(socket, SIGNAL(readyRead()), SLOT(readFromServer()));
     connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), SLOT(onGroupButtonClicked(int)));

}

MainWindow::~MainWindow()
{
    delete socket;
    delete ui;
}


void MainWindow::onGroupButtonClicked(int id)
{
    QButtonGroup *group=(QButtonGroup *)sender();
    QAbstractButton * button=group->button(id);
    QString str=button->text();
    if (button->text()[0]==10060){
        ui->lineEdit->clear();
        ui->label->setText("");
    }
    else if(button->text()=="П")
         ui->lineEdit->setText(ui->lineEdit->text()+"3.14");
    else if (button->text()=="a^b")
         ui->lineEdit->setText(ui->lineEdit->text()+"^");
    else
    ui->lineEdit->setText(ui->lineEdit->text()+button->text());

}

void MainWindow::on_pushButton_21_clicked()
{

    QByteArray arr;
    QString str=ui->lineEdit->text();
    arr.append(str);
   socket->write(arr);
    ui->lineEdit->clear();
}
void MainWindow::readFromServer()
{

     ui->label->setText("✓");
    QByteArray arr=socket->readAll();
    QString str=QString(arr);
      ui->lineEdit->setText(str);

}
