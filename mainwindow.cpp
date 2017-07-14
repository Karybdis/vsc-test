#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial=new QSerialPort;
    portscan();
    connect(ui->Button_portscan,SIGNAL(clicked(bool)),this,SLOT(portscan()));  //扫描串口
    connect(ui->Button_portopen,SIGNAL(clicked(bool)),this,SLOT(connectport()));  //连接串口
    connect(serial,SIGNAL(readyRead()),this,SLOT(receive()));  //接收数据
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::portscan()
{
    ui->comboBox_portnum->clear();
    QList<QSerialPortInfo> coms=QSerialPortInfo::availablePorts();
    for (int i=0;i<coms.length();i++)
        ui->comboBox_portnum->addItem(coms.at(i).portName());
}

void MainWindow::connectport()  //连接串口
{
    int baudrate=ui->comboBox_baudrate->currentIndex();
    int databit=ui->comboBox_databit->currentIndex();
    int stopbit=ui->comboBox_stopbit->currentIndex();
    int cleckbit=ui->comboBox_checkbit->currentIndex();
    if(!serial->isOpen())
    {
        serial->setPortName(ui->comboBox_portnum->currentText());
        switch(baudrate)
        {
            case 0:serial->setBaudRate(QSerialPort::Baud1200); break;
            case 1:serial->setBaudRate(QSerialPort::Baud2400); break;
            case 2:serial->setBaudRate(QSerialPort::Baud4800); break;
            case 3:serial->setBaudRate(QSerialPort::Baud9600); break;
            case 4:serial->setBaudRate(QSerialPort::Baud19200); break;
            case 5:serial->setBaudRate(QSerialPort::Baud57600); break;
            case 6:serial->setBaudRate(QSerialPort::Baud115200); break;
            default:break;
        }
        switch(databit)
        {
            case 0:serial->setDataBits(QSerialPort::Data5); break;
            case 1:serial->setDataBits(QSerialPort::Data6); break;
            case 2:serial->setDataBits(QSerialPort::Data7); break;
            case 3:serial->setDataBits(QSerialPort::Data8); break;
            default:break;
        }
        switch(stopbit)
        {
            case 0:serial->setStopBits(QSerialPort::OneStop); break;
            case 1:serial->setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2:serial->setStopBits(QSerialPort::TwoStop); break;
            default:break;
        }
        switch(cleckbit)
        {
            case 0:serial->setParity(QSerialPort::NoParity); break;
            case 1:serial->setParity(QSerialPort::OddParity); break;
            case 2:serial->setParity(QSerialPort::EvenParity); break;
            default:break;
        }
        if(!serial->open(QIODevice::ReadWrite))
            QMessageBox::information(this,"Error",serial->errorString());
        else
        {
            ui->comboBox_portnum->setEnabled(false);
            ui->comboBox_baudrate->setEnabled(false);
            ui->comboBox_databit->setEnabled(false);
            ui->comboBox_stopbit->setEnabled(false);
            ui->comboBox_checkbit->setEnabled(false);
            ui->Button_portopen->setText("断开连接");
        }
    }
    else
    {
        serial->close();
        ui->comboBox_portnum->setEnabled(true);
        ui->comboBox_baudrate->setEnabled(true);
        ui->comboBox_databit->setEnabled(true);
        ui->comboBox_stopbit->setEnabled(true);
        ui->comboBox_checkbit->setEnabled(true);
        ui->Button_portopen->setText("打开串口");
    }
}


void MainWindow::receive()
{
    QByteArray message=serial->readAll();
    QByteArray mes;
    QString str;
    int i;
    //以下简单的通信协议
    if (message[0]=='1')  //判定是否该下位机地址位
    {
        if (message[1]=='0')  //命令是0的情况,顺着传回来
        {
            for (i=0;i<message[2]-48;i++)
                mes[i]=message[i+3];
            str.append(QString(mes));
            serial->write(str.toUtf8());
        }
        else if (message[1]=='1') //命令是1的情况，倒着传回来
        {
            for (i=message[2]-48;i>0;i--)
                mes[message[2]-48-i]=message[i+2];
            str.append(QString(mes));
            serial->write(str.toUtf8());
        }
    }
}

