#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCharts/QValueAxis>
#include <QDateTime>
#include <QSerialPortInfo>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))
{
    ui->setupUi(this);

    initSerialSettings();  // 初始化串口设置
    initCharts();          // 初始化图表

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::on_readData);
    connect(ui->clearDataButton, &QPushButton::clicked, this, &MainWindow::on_clearDataButton_clicked);
//    connect(ui->displayFormatComboBox, &QComboBox::currentTextChanged, this, &MainWindow::on_displayFormatChanged);
connect(ui->saveToCsvButton, &QPushButton::clicked, this, &MainWindow::on_saveToCsvButton_clicked);  // 连接保存按钮

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSerialSettings()
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->portComboBox->addItem(info.portName());
    }

    ui->baudRateComboBox->addItem("9600");
    ui->baudRateComboBox->addItem("115200");
    ui->baudRateComboBox->setCurrentIndex(0);
}

void MainWindow::initCharts()
{
    tempSeries = new QtCharts::QLineSeries();
    humiSeries = new QtCharts::QLineSeries();

    tempSeries->setName("温度");
    humiSeries->setName("湿度");

    chart = new QtCharts::QChart();
    chart->addSeries(tempSeries);
    chart->addSeries(humiSeries);
    chart->createDefaultAxes();
    chart->axisX()->setTitleText("时间");
    chart->axisY()->setTitleText("值");
    chart->legend()->setVisible(true);

    chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(chartView);
}

void MainWindow::on_openSerialButton_clicked()
{
    serial->setPortName(ui->portComboBox->currentText());
    serial->setBaudRate(ui->baudRateComboBox->currentText().toInt());
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadOnly)) {
        ui->statusBar->showMessage("串口已成功打开。");
        startTimer();
    } else {
        ui->statusBar->showMessage("串口打开失败。");
    }
}

void MainWindow::on_closeSerialButton_clicked()
{
    if (serial->isOpen()) {
        serial->close();
        ui->statusBar->showMessage("串口已关闭。");
        timer->stop();
    }
}

void MainWindow::startTimer()
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_readData);
    timer->start(1000);
}
void MainWindow::on_readData()
{
    static QByteArray buffer;  // 用于存储读取到的临时数据
    buffer += serial->readAll();  // 从串口读取数据并追加到缓冲区

    const char startMarker = '*';  // 数据包的起始标记
    const char endMarker = '#';    // 数据包的结束标记
    const int minPacketLength = 4; // 数据包的最小长度（包括标记和数据）

    while (buffer.size() >= minPacketLength) {
        // 确保缓冲区的第一个和最后一个字符是起始和结束标记
        if (buffer.startsWith(startMarker) && buffer.endsWith(endMarker)) {
            // 提取中间的数据部分
            QByteArray data = buffer.mid(1, buffer.size() - 2);
            buffer.clear();  // 清空缓冲区

            // 确保数据长度为2字节
            if (data.size() == 2) {
                uchar tempHex = static_cast<uchar>(data[0]);  // 温度字节
                uchar humiHex = static_cast<uchar>(data[1]);  // 湿度字节

                // 打印原始数据
                qDebug() << "Raw Temperature byte:" << QString::number(tempHex, 16).toUpper();
                qDebug() << "Raw Humidity byte:" << QString::number(humiHex, 16).toUpper();

                // 将字节转换为十进制数值
                double temperature = static_cast<double>(tempHex);
                double humidity = static_cast<double>(humiHex);

                qDebug() << "Temperature:" << temperature;
                qDebug() << "Humidity:" << humidity;

                QDateTime currentTime = QDateTime::currentDateTime();
                qint64 timeInSeconds = currentTime.toSecsSinceEpoch();

                // 更新温度和湿度曲线
                tempSeries->append(timeInSeconds, temperature);
                humiSeries->append(timeInSeconds, humidity);

                chart->axisX()->setRange(timeInSeconds - 60, timeInSeconds); // 显示最近1分钟的数据
                chart->axisY()->setRange(0, 100);  // 假设温度和湿度的范围是0到100
                dataRecords.append(qMakePair(timeInSeconds, qMakePair(temperature, humidity)));
            }
        } else {
            // 移除缓冲区的第一个字节并继续处理
            buffer.remove(0, 1);
        }
    }
}




void MainWindow::on_clearDataButton_clicked()
{
    tempSeries->clear();
    humiSeries->clear();
    ui->statusBar->showMessage("数据已清空。");
}

//void MainWindow::on_displayFormatChanged()
//{
//    displayFormat = ui->displayFormatComboBox->currentText();
//}

//void MainWindow::displayData(const QByteArray &data)
//{
//    if (displayFormat == "HEX") {
//        // 以十六进制格式显示数据
//        QString hexString = data.toHex(' ').toUpper();
//        ui->dataDisplayTextEdit->append(hexString);
//    } else {
//        // 以ASCII格式显示数据
//        QString asciiString = QString::fromLatin1(data);
//        ui->dataDisplayTextEdit->append(asciiString);
//    }
//}


void MainWindow::on_saveToCsvButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存到CSV文件", "", "CSV Files (*.csv)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件进行写入";
        return;
    }

    QTextStream out(&file);
    out << "Timestamp,Temperature,Humidity\n";  // 写入CSV标题行

    for (const auto &record : dataRecords) {
        qint64 timestamp = record.first;
        double temperature = record.second.first;
        double humidity = record.second.second;
        out << timestamp << "," << temperature << "," << humidity << "\n";
    }

    file.close();
    qDebug() << "数据已保存到" << fileName;
}

