#include "mainwindow.h"
#include <QMessageBox>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->canTocom_bt, SIGNAL(CanToComData(QByteArray)), this, SLOT(CanToSerial(QByteArray)));
    connect(ui->comTocan_bt, SIGNAL(ComToCanData(QByteArray)), this, SLOT(SerialToCan(QByteArray)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::SerialToCan(QByteArray Pack)
{
    qDebug()<<"TxHex:"<<Pack.toHex();
    ui->lineEdit_comtocan->setText(Pack.toHex());
}
void MainWindow::CanToSerial(QByteArray Pack)
{
    qDebug()<<"TxHex:"<<Pack.toHex();
    ui->lineEdit_cantocom->setText(Pack.toHex());
}

void MainWindow::txdPack(struct dataPack *pack)
{
    QByteArray Pack("\0");

    if(pack->len < 1)
    {
        pack->len = 1;
        pack->cdata[0] = 0;
    }
    else if(pack->len > 8)
    {
        pack->len = 8;
    }

    pack->res = 0;

    Pack.resize(9+pack->len);

    Pack[0] = 'A';
    Pack[1] = 'T';

//    quint32 addr;

//    memcpy(&addr,&(pack->exId),4);

//    addr = (addr<<3)|(0X00000004);  //需按维特usb-can适配器的can——id报文要求发送134282497
    quint32 addr = (static_cast<quint32>(pack->exId.id) << 24) |    // ID 占高8位
                   (static_cast<quint32>(pack->exId.data) << 8) |   // Data 占中间16位
                   (static_cast<quint32>(pack->exId.mode) << 3) |   // Mode 占低8位中的高5位
                   (static_cast<quint32>(pack->exId.res));          // Res 占最低的3位
    quint8 byte1 = (addr >> 24) & 0xFF;  // 最高字节
    quint8 byte2 = (addr >> 16) & 0xFF;  // 第二高字节
    quint8 byte3 = (addr >> 8)  & 0xFF;  // 第三高字节
    quint8 byte4 = addr & 0xFF;          // 最低字节

    qDebug() << "Byte 1:" << QString::number(byte1, 16).toUpper();  // 以16进制输出
    qDebug() << "Byte 2:" << QString::number(byte2, 16).toUpper();
    qDebug() << "Byte 3:" << QString::number(byte3, 16).toUpper();
    qDebug() << "Byte 4:" << QString::number(byte4, 16).toUpper();

    addr = (addr << 3) | 0X00000004;


    qDebug() << "Copied address:" << addr;
    Pack[2] = (quint8)((addr&0XFF000000)>>24);
    Pack[3] = (quint8)((addr&0X00FF0000)>>16);
    Pack[4] = (quint8)((addr&0X0000FF00)>>8);
    Pack[5] = (quint8)((addr&0X000000FF)>>0);

    Pack[6] = pack->len;

    for(quint8 i =0;i<pack->len;i++)
        Pack[7+i] = pack->cdata[i];

    Pack[7+pack->len] = '\r';
    Pack[8+pack->len] = '\n';
    emit CanToSerial(Pack);
}

void MainWindow::analysisRxdDatas(QByteArray pack)
{
    QByteArray rxdata;
    // 输出字节数组内容，每个字节以十六进制和十进制表示
    qDebug() << "Byte array (hex):" << pack.toHex();
    for (int i = 0; i < pack.size(); ++i) {
        quint8 byteValue = static_cast<quint8>(pack[i]);  // 确保每个字节被解读为无符号数
        qDebug() << "Byte" << i << ":" << QString("0x%1").arg(byteValue, 2, 16, QLatin1Char('0')).toUpper() << "(" << byteValue << ")";
    }
    if(static_cast<quint8>(pack[0]) == 0x41 && static_cast<quint8>(pack[1]) == 0x54)  //pack包共15个&&(pack.size()==((quint8)pack[6]+11))
    {
        uint32_t addr = pack[5]  & 0x000000FF;
        addr |= ((pack[4] << 8)  & 0x0000FF00);
        addr |= ((pack[3] << 16) & 0x00FF0000);
        addr |= ((pack[2] << 24) & 0xFF000000);

//        qDebug()<<"addr:"<<addr;
        addr = (addr & ~0x00000004) >> 3;
        qDebug()<<"addr:"<<addr;

        rxFrame.exId.res = addr & 0x07;
        rxFrame.exId.mode = (addr >> 3) & 0x1F;
        rxFrame.exId.data = (addr >> 8) & 0xFFFF;
        rxFrame.exId.id = (addr >> 24) & 0xFF;
        qDebug() << "ID:" << rxFrame.exId.id;
        qDebug() << "Data:" << rxFrame.exId.data;
        qDebug() << "Mode:" << rxFrame.exId.mode;
        qDebug() << "Res:" << rxFrame.exId.res;

        rxFrame.len = pack[6];

        for(quint8 i = 0;i<rxFrame.len;i++)
            rxFrame.data[i] = pack[7+i];

        rxdata[0] = rxFrame.exId.id;
        rxdata[1] = (rxFrame.exId.data >> 8) & 0xFF;
        rxdata[2] = rxFrame.exId.data & 0xFF;
        rxdata[3] = ((rxFrame.exId.mode & 0x1F) << 3) | (rxFrame.exId.res & 0x07);;                 // 1 byte
        rxdata[4] = rxFrame.len;
        for(quint8 j=0;j<rxFrame.len;j++)
            rxdata[5+j] = rxFrame.data[j];

    }else
    {
        QMessageBox::warning(nullptr, "Invalid Input", "Invalid input data.");
        ui->lineEdit_comtocan->clear();
        ui->lineEdit->clear();
        return;
    }
    emit SerialToCan(rxdata);

}

void MainWindow::on_btn_clear1_clicked()
{
    ui->lineEdit_comtocan->clear();
    ui->lineEdit->clear();
}

void MainWindow::on_btn_clear2_clicked()
{
    ui->lineEdit_cantocom->clear();
    ui->lineEdit_3->clear();
}

void MainWindow::on_canTocom_bt_clicked()
{
    struct dataPack pack;
    memset(&pack, 0, sizeof(struct dataPack));
    QString hexString = ui->lineEdit_3->text().trimmed();
    if (hexString.isEmpty()) {
        qDebug() << "Error: Input is empty!";
        QMessageBox::warning(nullptr, "Invalid Input", "Please enter a valid hexadecimal string.");
        ui->lineEdit_cantocom->clear();
        ui->lineEdit_3->clear();
        return;
    }

    // 移除无效字符
    hexString.remove(QRegExp("[^0-9A-Fa-f]")); // 只保留有效的十六进制字符

    // 检查长度是否为偶数
    if (hexString.length() % 2 != 0) {
        qDebug() << "Warning: Hex string length is odd, auto-padding with 0.";
        hexString.prepend('0'); // 在前面补零
    }
    QByteArray byteArray = QByteArray::fromHex(hexString.toUtf8());

    if (byteArray.size() < 5) {
        qDebug() << "Error: Byte array too short!";
        QMessageBox::warning(nullptr, "Invalid Input", "The byte array must contain at least 5 bytes.");
        ui->lineEdit_cantocom->clear();
        ui->lineEdit_3->clear();
        return;
    }
    // 解析数据
    pack.exId.id = byteArray[0];  // ID 是第一个字节

    // Data 由两个字节组成，需要合并 byteArray[1] 和 byteArray[2]
    pack.exId.data = (static_cast<quint16>(static_cast<quint8>(byteArray[1]) << 8)) | static_cast<quint8>(byteArray[2]);

    // Mode 是 byteArray[3] 的低5位
    pack.exId.mode = (byteArray[3] >> 3) & 0x1F;

    // Res 是 byteArray[3] 的高3位
    pack.exId.res = byteArray[3] & 0x07;

    // 第 5 个字节为 Length
    pack.len = byteArray[4];

    // 输出字节数组及结构体成员
    qDebug() << "Byte array (hex):" << byteArray.toHex(); // 输出转换后的字节数组
    for (int i = 0; i < byteArray.size(); ++i) {
        qDebug() << "Byte" << i << ":" << static_cast<quint16>(byteArray[i]);
    }

    // 输出结构体的成员
    qDebug() << "ID:" << pack.exId.id;
    qDebug() << "Data:" << pack.exId.data; // 确认这里的值
    qDebug() << "Mode:" << pack.exId.mode;
    qDebug() << "Res:" << pack.exId.res;
    qDebug() << "Length:" << pack.len;

    // 输出数据数组
    QString dataStr;
    for (int i = 0; i < pack.len; ++i) {
        if (5 + i < byteArray.size()) { // 确保不会超出数组边界
            pack.cdata[i] = byteArray[5 + i]; // 从字节数组中提取数据
            dataStr.append(QString("%1 ").arg(pack.cdata[i], 2, 16, QLatin1Char('0')).toUpper());
        }
    }
    qDebug() << "Data Array:" << dataStr.trimmed();

    txdPack(&pack);
}

void MainWindow::on_comTocan_bt_clicked()
{
    QString hexString = ui->lineEdit->text().trimmed();
    if (hexString.isEmpty()) {
        qDebug() << "Error: Input is empty!";
        QMessageBox::warning(nullptr, "Invalid Input", "Please enter a valid hexadecimal string.");
        ui->lineEdit_comtocan->clear();
        ui->lineEdit->clear();
        return;
    }

    // 移除无效字符，只保留有效的十六进制字符
    hexString.remove(QRegExp("[^0-9A-Fa-f]"));

    // 检查长度是否为偶数，如果不为偶数，前面补零
    if (hexString.length() % 2 != 0) {
        qDebug() << "Warning: Hex string length is odd, auto-padding with 0.";
        hexString.prepend('0');
    }

    // 将十六进制字符串转换为字节数组
    QByteArray byteArray = QByteArray::fromHex(hexString.toUtf8());

    if (byteArray.size() < 5) {
        qDebug() << "Error: Byte array too short!";
        QMessageBox::warning(nullptr, "Invalid Input", "The byte array must contain at least 5 bytes.");
        ui->lineEdit_comtocan->clear();
        ui->lineEdit->clear();
        return;
    }

    // 输出字节数组内容，每个字节以十六进制和十进制表示
    qDebug() << "Byte array (hex):" << byteArray.toHex();
    for (int i = 0; i < byteArray.size(); ++i) {
        quint8 byteValue = static_cast<quint8>(byteArray[i]);  // 确保每个字节被解读为无符号数
        qDebug() << "Byte" << i << ":" << QString("0x%1").arg(byteValue, 2, 16, QLatin1Char('0')).toUpper() << "(" << byteValue << ")";
    }

    analysisRxdDatas(byteArray);
}
