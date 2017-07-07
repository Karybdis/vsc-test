#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial=new QSerialPort;
    portscan();
    connect(ui->Button_portopen,SIGNAL(clicked(bool)),this,SLOT(connectport()));
    connect(serial,SIGNAL(readyRead()),this,SLOT(receive()));
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

void MainWindow::connectport()
{
    if(!serial->isOpen())
    {
        serial->setPortName(ui->comboBox_portnum->currentText());
        if(!serial->open(QIODevice::ReadWrite))
            QMessageBox::information(this,"Error",serial->errorString());
        else
        {
            ui->comboBox_portnum->setEnabled(false);
            ui->Button_portopen->setText("断开连接");
        }

    }
    else
    {
        serial->close();
        ui->comboBox_portnum->setEnabled(true);
        ui->Button_portopen->setText("打开串口");
    }
}


void MainWindow::receive()
{
    QByteArray message=serial->readAll();
    serial->write(message.toHex(),message.length());
}

