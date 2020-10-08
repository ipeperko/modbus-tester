#include "rtu_widget.h"
#include "ui_rtu_widget.h"
#include <QSettings>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

#define DEV_PATH "/dev"

rtu_widget::rtu_widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::RTUWidget)
{
    ui->setupUi(this);

    ui->radioButton_RS485->setChecked(true);

    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud9600), QSerialPort::Baud9600);
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud19200), QSerialPort::Baud19200);
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud38400), QSerialPort::Baud38400);
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud57600), QSerialPort::Baud57600);
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud115200), QSerialPort::Baud115200);

    for (auto[f, s] : std::initializer_list<std::pair<char, const char*>>{ { 'N', "None" }, { 'E', "Even" }, { 'O', "Odd" } }) {
        ui->comboBox_Parity->addItem(s, QChar(f));
    }

    ui->comboBox_DataBits->addItem(QString::number(QSerialPort::Data5), QSerialPort::Data5);
    ui->comboBox_DataBits->addItem(QString::number(QSerialPort::Data6), QSerialPort::Data6);
    ui->comboBox_DataBits->addItem(QString::number(QSerialPort::Data7), QSerialPort::Data7);
    ui->comboBox_DataBits->addItem(QString::number(QSerialPort::Data8), QSerialPort::Data8);
    ui->comboBox_DataBits->setCurrentIndex(ui->comboBox_DataBits->findData(QSerialPort::Data8));

    ui->comboBox_StopBits->addItem(QString::number(QSerialPort::OneStop), QSerialPort::OneStop);
    ui->comboBox_StopBits->addItem(QString::number(QSerialPort::TwoStop), QSerialPort::TwoStop);

    file_watcher.addPath(DEV_PATH);
    connect(&file_watcher, &QFileSystemWatcher::directoryChanged, this, &rtu_widget::port_directory_changed);

    port_directory_changed(DEV_PATH);
}

rtu_widget::~rtu_widget()
{
    delete ui;
}

void rtu_widget::set_rtu_type(mb_rtu_type type)
{
    if (type == mb_rtu_type::RS485) {
        ui->radioButton_RS485->setChecked(true);
    }
    else {
        ui->radioButton_RS232->setChecked(true);
    }
}

mb_rtu_type rtu_widget::rtu_type() const
{
    return ui->radioButton_RS485->isChecked() ? mb_rtu_type::RS485 : mb_rtu_type::RS232;
}

QString rtu_widget::com_port() const
{
    if (ui->comboBox_SerialDevice->currentIndex() < 0) {
        throw std::runtime_error("No comm port selected");
    }

    return ui->comboBox_SerialDevice->currentText();
}

int rtu_widget::baud_rate() const
{
    return ui->comboBox_BaudRate->currentData().toInt();
}

char rtu_widget::parity() const
{
    return ui->comboBox_Parity->currentData().toChar().toLatin1();
}

int rtu_widget::data_bits() const
{
    return ui->comboBox_DataBits->currentData().toInt();
}

int rtu_widget::stop_bits() const
{
    return ui->comboBox_StopBits->currentData().toInt();
}

void rtu_widget::load_settings(const QString& topic)
{
    QSettings set;
}

void rtu_widget::save_settings(const QString& topic)
{
    QSettings set;
}

void rtu_widget::port_directory_changed(const QString& path)
{
    //qDebug() << "Directory changed :" << path;

    QString selected = ui->comboBox_SerialDevice->currentText();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    ui->comboBox_SerialDevice->clear();

    for (auto const& p : ports) {
        QString name = p.systemLocation();
        if (name.contains("ttyUSB")) {
            ui->comboBox_SerialDevice->addItem(name);
        }
    }

    int idx = ui->comboBox_SerialDevice->findText(selected);
    if (idx < 0)
        idx = 0;

    ui->comboBox_SerialDevice->setCurrentIndex(idx);
}

