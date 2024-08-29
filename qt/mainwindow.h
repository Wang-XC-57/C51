#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QTimer>
#include <QTextEdit>       // 添加这个来包含 QTextEdit 类型
#include <QComboBox>       // 添加这个来包含 QComboBox 类型

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openSerialButton_clicked();
    void on_closeSerialButton_clicked();
    void on_readData();
    void on_clearDataButton_clicked();
    void on_displayFormatChanged();
    void on_saveToCsvButton_clicked();
private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QtCharts::QLineSeries *tempSeries;
    QtCharts::QLineSeries *humiSeries;
    QtCharts::QChart *chart;
    QtCharts::QChartView *chartView;
    QTimer *timer;

    QTextEdit *dataDisplayTextEdit;
    QComboBox *displayFormatComboBox;
    QString displayFormat = "HEX";  // 默认显示格式为 ASCII
    QList<QPair<qint64, QPair<double, double>>> dataRecords;  // 存储时间戳、温度和湿度的记录
    void initSerialSettings();
    void initCharts();
    void startTimer();
    void displayData(const QByteArray &data);
};
#endif // MAINWINDOW_H
