#include "log_widget.h"
#include <QDateTime>

namespace {
QString timeString(std::chrono::system_clock::time_point tp, bool millis=true)
{
    double tp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
    std::time_t tp_c = std::chrono::system_clock::to_time_t(tp);
    QDateTime dt = QDateTime::fromSecsSinceEpoch(tp_c);
    tp_ms -= std::milli::den * tp_c;
    QString s = dt.toString("hh:mm:ss");
    QString s_ms;
    if (millis) {
        s_ms = QString::number(tp_ms);
        while (s_ms.size() < 3) {
            s_ms.prepend("0");
        }
        s += "." + s_ms;
    }
    return s;
}
}

log_widget::log_widget(QWidget *parent)
    : QListWidget(parent)
{
}

void log_widget::append_item(QListWidgetItem* item, const QString &msg)
{
    item->setText(timeString(std::chrono::system_clock::now()) + " " + msg);
    addItem(item);
    scroll_to_bottom();
}

void log_widget::append_log_message(const QString &msg)
{
    auto* item = new QListWidgetItem(timeString(std::chrono::system_clock::now()) + " " + msg);
    addItem(item);
    scroll_to_bottom();
}

void log_widget::append_log_error(const QString &msg)
{
    auto* item = new QListWidgetItem(timeString(std::chrono::system_clock::now()) + " " + msg);
    item->setBackground(QColor(255, 0, 0, 120));
    addItem(item);
    scroll_to_bottom();
}

void log_widget::scroll_to_bottom()
{
    int c = count();
    if (c > 0) {
        auto it = item(c - 1);
        scrollToItem(it);
    }
}
