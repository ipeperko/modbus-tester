#ifndef MODBUS_TESTER_RTU_WIDGET_H
#define MODBUS_TESTER_RTU_WIDGET_H

#include "mb_tester_common.h"
#include <QWidget>
#include <QFileSystemWatcher>

namespace Ui {
class ClientForm;
class RTUWidget;
}

class rtu_widget : public QWidget
{
    Q_OBJECT
public:
    rtu_widget(QWidget* parent = nullptr);
    ~rtu_widget();

    void set_rtu_type(mb_rtu_type type);
    void set_baud_rate(int val);
    void set_parity(char val);
    void set_data_bits(int val);
    void set_stop_bits(int val);
    void set_rts(int val);

    mb_rtu_type rtu_type() const;
    QString com_port() const;
    int baud_rate() const;
    char parity() const;
    int data_bits() const;
    int stop_bits() const;
    int rts() const;

    void load_settings(const QString& topic);
    void save_settings(const QString& topic);

private:
    Ui::RTUWidget* ui{nullptr};
    QFileSystemWatcher file_watcher;

private slots:
    void mode_changed();
    void port_directory_changed(const QString &path);
};

#endif //MODBUS_TESTER_RTU_WIDGET_H
