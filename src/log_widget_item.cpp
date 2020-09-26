#include "log_widget_item.h"

//void log_widget_item::set_data(const mb_reg_vector& regs)
//{
//    size_t n = regs.size() * sizeof(regs[0]);
//    data.resize(n / sizeof(data[0]));
//    memcpy(&data[0], &regs[0], n);
//}

//void log_widget_item::set_data(const mb_bit_vector& bits)
//{
//    static_assert(std::is_same_v<mb_bit_vector, decltype(data)>, "log_widget_item::data type not same as mb_bit_vector");
//    data = bits;
//}
