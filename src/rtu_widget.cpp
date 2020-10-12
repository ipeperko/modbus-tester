#include "rtu_widget.h"
#include "ui_rtu_widget.h"
#include <QSettings>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <modbus/modbus.h>

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

    ui->comboBox_RTS->addItem("None", MODBUS_RTU_RTS_NONE);
    ui->comboBox_RTS->addItem("Up", MODBUS_RTU_RTS_UP);
    ui->comboBox_RTS->addItem("Down", MODBUS_RTU_RTS_DOWN);

    connect(ui->radioButton_RS232, static_cast<void (QRadioButton::*)(bool)>(&QRadioButton::clicked), this, &rtu_widget::mode_changed);
    connect(ui->radioButton_RS485, static_cast<void (QRadioButton::*)(bool)>(&QRadioButton::clicked), this, &rtu_widget::mode_changed);

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

    mode_changed();
}

void rtu_widget::set_baud_rate(int val)
{
    int idx = ui->comboBox_BaudRate->findData(val);

    if (idx > -1) {
        ui->comboBox_BaudRate->setCurrentIndex(idx);
    }
}

void rtu_widget::set_parity(char val)
{
    int idx = ui->comboBox_Parity->findData(QChar(val));

    if (idx > -1) {
        ui->comboBox_Parity->setCurrentIndex(idx);
    }
}

void rtu_widget::set_data_bits(int val)
{
    int idx = ui->comboBox_DataBits->findData(val);

    if (idx > -1) {
        ui->comboBox_DataBits->setCurrentIndex(idx);
    }
}

void rtu_widget::set_stop_bits(int val)
{
    int idx = ui->comboBox_StopBits->findData(val);

    if (idx > -1) {
        ui->comboBox_StopBits->setCurrentIndex(idx);
    }
}

void rtu_widget::set_rts(int val)
{
    int idx = ui->comboBox_RTS->findData(val);

    if (idx > -1) {
        ui->comboBox_RTS->setCurrentIndex(idx);
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

int rtu_widget::rts() const
{
    return ui->comboBox_RTS->currentData().toInt();
}

void rtu_widget::load_settings(const QString& topic)
{
    QSettings set;
    set_rtu_type(static_cast<mb_rtu_type>(set.value(topic + "/rtu_mode", static_cast<int>(mb_rtu_type::RS485)).toInt()));
    set_baud_rate(set.value(topic + "/rtu_baud_rate", QSerialPort::Baud9600).toInt());
    set_parity(set.value(topic + "/rtu_parity", QChar('N')).toChar().toLatin1());
    set_data_bits(set.value(topic + "/rtu_data_bits", QSerialPort::Data8).toInt());
    set_stop_bits(set.value(topic + "/rtu_stop_bits", QSerialPort::OneStop).toInt());
    set_rts(set.value(topic + "/rtu_rts", MODBUS_RTU_RTS_NONE).toInt());
}

void rtu_widget::save_settings(const QString& topic)
{
    QSettings set;
    set.setValue(topic + "/rtu_mode", static_cast<int>(rtu_type()));
    set.setValue(topic + "/rtu_baud_rate", baud_rate());
    set.setValue(topic + "/rtu_parity", QChar(parity()));
    set.setValue(topic + "/rtu_data_bits", data_bits());
    set.setValue(topic + "/rtu_stop_bits", stop_bits());
    set.setValue(topic + "/rtu_rts", rts());
}

void rtu_widget::mode_changed()
{
    auto is_485 = rtu_type() == mb_rtu_type::RS485;
    ui->comboBox_RTS->setEnabled(is_485);
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

