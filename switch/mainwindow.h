#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QPushButton>

enum canComMode{
    CANCOM_ANNOUNCE_DEVID = 0,//通告设备ID

    CANCOM_MOTOR_CTRL,       //MOTOR-电机控制
    CANCOM_MOTOR_FEEDBACK,   //MOTOR-电机反馈
    CANCOM_MOTOR_IN,         //MOTOR-进入电机模式
    CANCOM_MOTOR_RESET,      //MOTOR-复位模式
    CANCOM_MOTOR_CALI,       //MOTOR-高速编码器标定
    CANCOM_MOTOR_ZERO,       //MOTOR-设置机械零位
    CANCOM_MOTOR_ID,         //MOTOR-设置ID
    CANCOM_PARA_WRITE,       //参数-写入
    CANCOM_PARA_READ,        //参数-读取
    CANCOM_PARA_UPDATE,      //参数-更新上传
    CANCOM_OTA_START,        //OTA-启动
    CANCOM_OTA_INFO,         //OTA-升级文件描述
    CANCOM_OTA_ING,          //OTA-升级中
    CANCOM_OTA_END,          //OTA-升级完成
    CANCOM_CALI_ING,         //编码器标定中
    CANCOM_CALI_RST,         //编码器标定结果
    CANCOM_SDO_READ,         //sdo
    CANCOM_SDO_WRITE,        //sdo
    CANCOM_PARA_STR_INFO,    //参数-字符串信息
    CANCOM_MOTOR_BRAKE,      //MOTOR-进入刹车模式
    CANCOM_FAULT_WARN,       //故障和警告信息

    CANCOM_MODE_TOTAL,
};
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
