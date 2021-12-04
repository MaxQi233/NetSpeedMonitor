#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pubfunc.h"


#include <QPainter>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_bIsFullScreen = false;

    //外观部分
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.9);

    resize(109,50);

    //功能部分
    m_menuContextMenu = new QMenu(this);    //右键菜单

    m_actSetting = new QAction(QString("设置"),this);
    m_menuContextMenu->addAction(m_actSetting);
    m_mysetting = new MySetting(this);
    connect(m_actSetting,SIGNAL(triggered()),this,SLOT(OnSetting()));

    m_actExit = new QAction(QString("退出"),this);
    m_menuContextMenu->addAction(m_actExit);
    connect(m_actExit,SIGNAL(triggered()),this,SLOT(OnExit()));

    firstRun = 0;
    uRetCode = GetIfTable(m_pTable, &m_dwAdapters, TRUE);
    if (uRetCode == ERROR_NOT_SUPPORTED)
    {
        QMessageBox::critical(this,QString("错误"),QString("未找到网卡！"));
        exit(0);
    }

    if (uRetCode == ERROR_INSUFFICIENT_BUFFER)
    {
        m_pTable = (PMIB_IFTABLE)new BYTE[65535];   //统计65535个端口
    }

    dwLastIn = 0;           //上一秒钟的接收字节数
    dwLastOut = 0;          //上一秒钟的发送字节数

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);//一秒刷新一次
    connect(m_timer,SIGNAL(timeout()),this,SLOT(OnUpdateSpeed()));

    m_timer_fullScreenDetect = new QTimer(this);
    m_timer_fullScreenDetect->setInterval(200);//全屏程序检测，0.2秒检测一次 ps:会不会太频繁？
    connect(m_timer_fullScreenDetect,SIGNAL(timeout()),this,SLOT(OnFullScreenDetect()));

    m_timer->start();
    m_timer_fullScreenDetect->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//添加圆角
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::black));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 5, 5);

    QWidget::paintEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_point = e->globalPosition().toPoint() - pos();
        e->accept();
    }

    if(e->button() == Qt::RightButton)
    {
        m_menuContextMenu->exec(cursor().pos());
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        move(e->globalPosition().toPoint() - m_point);
        e->accept();
    }
}

void MainWindow::OnUpdateSpeed()
{
    QString NetSpeedInfo;
    DWORD   dwBandIn = 0;           //下载速度
    DWORD   dwBandOut = 0;          //上传速度

    NetSpeedInfo.clear();
    GetIfTable(m_pTable, &m_dwAdapters, TRUE);
    DWORD   dwInOctets = 0;
    DWORD   dwOutOctets = 0;

    //将所有端口的流量进行统计
    for (UINT i = 0; i < m_pTable->dwNumEntries; i++)
    {
        MIB_IFROW   Row = m_pTable->table[i];
        dwInOctets += Row.dwInOctets;
        dwOutOctets += Row.dwOutOctets;
    }

    //如果ulong不够用了，这次数据就舍弃
    if((dwInOctets < dwLastIn) || (dwOutOctets < dwLastOut))
    {
        dwLastIn = dwInOctets;      //更新最后一次传输的流量
        dwLastOut = dwOutOctets;    //更新最后一次传输的流量
        return;
    }

    dwBandIn = dwInOctets - dwLastIn;       //下载速度
    dwBandOut = dwOutOctets - dwLastOut;    //上传速度

    dwLastIn = dwInOctets;      //更新最后一次传输的流量
    dwLastOut = dwOutOctets;    //更新最后一次传输的流量

    if(m_bIsFullScreen)
    {
        return;
    }
    else if(this->isHidden())
    {
        this->show();
        this->repaint();
    }

    if(firstRun == 0)
    {
        firstRun++;
        return;
    }

    double fDownSpeed, fUpSpeed;

    //计算下载网速
    dwBandIn /= 8;//bit转byte
    if(dwBandIn > 1024 * 1024 * 1024)//GB
    {
        ui->lb_dSpeed_2->setText(QString("GB/s"));
        fDownSpeed = ((double)dwBandIn) / 1024 / 1024 / 1024;
    }
    else if(dwBandIn > 1024 * 1024)//MB
    {
        ui->lb_dSpeed_2->setText(QString("MB/s"));
        fDownSpeed = ((double)dwBandIn) / 1024 / 1024;
    }
    else//KB和B
    {
        ui->lb_dSpeed_2->setText(QString("KB/s"));
        fDownSpeed = ((double)dwBandIn) / 1024;
    }

    //计算上传网速
    dwBandOut /= 8;//bit转byte
    if(dwBandOut > 1024 * 1024 * 1024)//GB
    {
        ui->lb_uSpeed_2->setText(QString("GB/s"));
        fUpSpeed = ((double)dwBandOut) / 1024 / 1024 / 1024;
    }
    else if(dwBandOut > 1024 * 1024)//MB
    {
        ui->lb_uSpeed_2->setText(QString("MB/s"));
        fUpSpeed = ((double)dwBandOut) / 1024 / 1024;
    }
    else//KB和B
    {
        ui->lb_uSpeed_2->setText(QString("KB/s"));
        fUpSpeed = ((double)dwBandOut) / 1024;
    }

    ui->lb_dSpeed->setText(QString::number(fDownSpeed,'f',2));
    ui->lb_dSpeed->repaint();
    ui->lb_uSpeed->setText(QString::number(fUpSpeed,'f',2));
    ui->lb_uSpeed->repaint();
}

void MainWindow::OnSetting()
{
    m_mysetting->exec();
}

void MainWindow::OnExit()
{
    exit(0);
}

void MainWindow::OnFullScreenDetect()
{
    m_bIsFullScreen = CheckFullscreen();
    if(m_bIsFullScreen)
    {
        this->hide();
    }
    else if(this->isHidden())
    {
        this->show();
    }
}


