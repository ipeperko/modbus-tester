#ifndef MODBUS_TESTER_LOG_WIDGET_ITEM_H
#define MODBUS_TESTER_LOG_WIDGET_ITEM_H

#include "mb_tester_common.h"
#include <QListWidgetItem>

class log_widget_item : public QListWidgetItem
{
public:
    typedef std::variant<mb_reg_vector, mb_bit_vector> data_t;

    log_widget_item(QListWidget *parent, mb_dropdown_data_index_t role, mb_direction_t dir, int addr, mb_reg_vector const& regs)
        : QListWidgetItem(parent)
        , role_(role)
        , dir_(dir)
        , addr_(addr)
        , data_(regs)
    {}

    log_widget_item(QListWidget *parent, mb_dropdown_data_index_t role, mb_direction_t dir, int addr, mb_bit_vector const& bits)
        : QListWidgetItem(parent)
        , role_(role)
        , dir_(dir)
        , addr_(addr)
        , data_(bits)
    {}

    mb_dropdown_data_index_t role() const
    {
        return role_;
    }

    mb_direction_t direction() const
    {
        return dir_;
    }

    int address() const
    {
        return addr_;
    }

    void set_data(mb_reg_vector&& d)
    {
        data_ = std::move(d);
    }

    void set_data(mb_bit_vector&& d)
    {
        data_ = std::move(d);
    }

    size_t data_size() const
    {
        return std::visit([](auto& arg) {
            return arg.size();
        }, data_);
    }

    auto& get_regs()
    {
        return std::get<mb_reg_vector>(data_);
    }

    auto const& get_regs() const
    {
        return std::get<mb_reg_vector>(data_);
    }

    auto& get_bits()
    {
        return std::get<mb_bit_vector>(data_);
    }

    auto const& get_bits() const
    {
        return std::get<mb_bit_vector>(data_);
    }

private:
    mb_dropdown_data_index_t role_;
    mb_direction_t dir_;
    int addr_;
    data_t data_;
};
#endif //MODBUS_TESTER_LOG_WIDGET_ITEM_H
