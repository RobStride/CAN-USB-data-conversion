#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QPushButton>

struct exCanIdInfo{
        quint32 id:8;
        quint32 data:16;
        quint32 mode:5;
// enum canComMode mode:5;
        quint32 res:3;
};
struct canPack{
    struct exCanIdInfo exId;
    quint8 len;
    quint8 data[8];
};

struct eCanIdInfo
{
    quint32 id:8;
    quint32 data:16;
    quint32 mode:5;
    quint32 res:3;
};
struct dataPack{
    struct eCanIdInfo exId;
    quint32 res;
    quint8 len;
    quint8 cdata[8];
};

union byte_4
{
   int data;
   quint32 udata;
   float fdata;
   quint8 byte[sizeof(int)];//以一字节为单元
};
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void txdPack(struct dataPack *pack);
    void analysisRxdDatas(QByteArray);
    struct canPack rxFrame;

signals:

    void ComToCanData(QByteArray);
    void CanToComData(QByteArray);

private slots:

    void SerialToCan(QByteArray);
    void CanToSerial(QByteArray);

    void on_btn_clear1_clicked();

    void on_btn_clear2_clicked();

    void on_comTocan_bt_clicked();

    void on_canTocom_bt_clicked();

private:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    Ui::MainWindow *ui;

    bool readParaAllFlag;
};

#endif // MAINWINDOW_H
