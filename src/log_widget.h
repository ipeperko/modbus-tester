#ifndef MODBUS_TESTER_LOG_WIDGET_H
#define MODBUS_TESTER_LOG_WIDGET_H

#include "log_widget_item.h"
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

    static constexpr int max_logs = 200;

private:
    void add(QListWidgetItem *item);

public slots:
    void erase_logs();

private slots:
    void show_context_menu(const QPoint& pos);

signals:
    void item_action(log_widget_item const* item);
};


#endif //MODBUS_TESTER_LOG_WIDGET_H
