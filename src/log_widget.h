#ifndef MODBUS_TESTER_LOG_WIDGET_H
#define MODBUS_TESTER_LOG_WIDGET_H

#include <QListWidget>

class log_widget : public QListWidget
{
    Q_OBJECT
public:
    log_widget(QWidget* parent = nullptr);

    void append_item(QListWidgetItem* item, const QString &msg);
    void append_log_message(const QString& msg);
    void append_log_error(const QString& msg);
    void scroll_to_bottom();
};


#endif //MODBUS_TESTER_LOG_WIDGET_H
