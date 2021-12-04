#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMenu>

#include "winsock2.h"
#include "windows.h"
#include "winsock.h"
#include "iphlpapi.h"
#include "mysetting.h"

#define PROGRAM_VERSION "v0.1.0"
#define SPEED_UNIT_B    0x00
#define SPEED_UNIT_KB   0x01
#define SPEED_UNIT_MB   0x02
#define SPEED_UNIT_GB   0x04

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private slots:
    void OnUpdateSpeed();
    void OnSetting();
    void OnExit();
    void OnFullScreenDetect();

private:
    Ui::MainWindow *ui;

    QPalette pal;
    QPoint m_point;

    QMenu *m_menuContextMenu;
    QAction* m_actSetting;
    QAction* m_actExit;
    MySetting* m_mysetting;

    void paintEvent(QPaintEvent *event);

    QTimer* m_timer;
    QTimer* m_timer_fullScreenDetect;

    DWORD   dwLastIn = 0;           //上一秒钟的接收字节数
    DWORD   dwLastOut = 0;          //上一秒钟的发送字节数
    PMIB_IFTABLE    m_pTable = NULL;
    DWORD    m_dwAdapters = 0;
    ULONG    uRetCode;
    int firstRun;
    bool m_bIsFullScreen;
};

#endif // MAINWINDOW_H
