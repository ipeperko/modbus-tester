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

public slots:
    void erase_logs();

private slots:
    void show_context_menu(const QPoint& pos);

signals:
    void item_action(QListWidgetItem* item);
};


#endif //MODBUS_TESTER_LOG_WIDGET_H
