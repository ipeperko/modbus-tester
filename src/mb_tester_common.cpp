#include "mb_tester_common.h"
#include <modbus/modbus.h>

//
// Modbus exception
//
mb_exception::mb_exception(std::string m, int err_no)
    : err_code(err_no)
{
    try {
        msg = build_message(m, err_no);
    }
    catch(...)
    {
        msg = std::move(m);
    }
}

const char* mb_exception::error_descr() const noexcept
{
    return error_descr(err_code);
}

std::string mb_exception::build_message(const std::string &what, int err_code)
{
    std::string msg = what + " - reason " + std::string(error_descr(err_code)) + " (" + std::to_string(err_code) + ")";
    return std::__cxx11::string();
}

const char* mb_exception::error_descr(int code) noexcept
{
    return modbus_strerror(code);
}
