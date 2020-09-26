#ifndef MODBUS_TESTER_LOG_WIDGET_ITEM_H
#define MODBUS_TESTER_LOG_WIDGET_ITEM_H

#include "mb_tester_common.h"
#include <QListWidgetItem>

class log_widget_item : public QListWidgetItem
{
public:
    log_widget_item(QListWidget *parent, mb_dropdown_data_index_t role, mb_direction_t dir, int addr, mb_reg_vector const& regs)
        : QListWidgetItem(parent)
        , role(role)
        , direction(dir)
        , addr(addr)
        , size(regs.size())
        , data_regs(regs)
    {}

    log_widget_item(QListWidget *parent, mb_dropdown_data_index_t role, mb_direction_t dir, int addr, mb_bit_vector const& bits)
        : QListWidgetItem(parent)
        , role(role)
        , direction(dir)
        , addr(addr)
        , size(bits.size())
        , data_bits(bits)
    {}

//    void set_data(mb_reg_vector const& regs);
//    void set_data(mb_bit_vector const& bits);

//private:
    mb_dropdown_data_index_t role;
    mb_direction_t direction;
    int addr;
    int size;
    mb_reg_vector data_regs;
    mb_bit_vector data_bits;
//    std::vector<uint8_t> data;
};
#endif //MODBUS_TESTER_LOG_WIDGET_ITEM_H
