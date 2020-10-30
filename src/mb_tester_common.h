#ifndef MODBUS_TESTER_MB_TESTER_COMMON_H
#define MODBUS_TESTER_MB_TESTER_COMMON_H

#include <stdexcept>
#include <vector>
#include <string>

using mb_register_t = uint16_t;
using mb_input_t = uint8_t;
using mb_reg_vector = std::vector<mb_register_t>;
using mb_bit_vector = std::vector<mb_input_t>;

inline constexpr int modbus_num_data_types = 4;

enum class mb_dropdown_data_index_t : int
{
    holding_registers = 0,
    input_registers,
    coils,
    discrete_inputs
};

enum class mb_direction_t
{
    read,
    write
};

enum class mb_rtu_type
{
    RS232,
    RS485,
};

//
// Modbus exception
//
class mb_exception : public std::exception
{
public:
    mb_exception(std::string m, int err_no);

    const char* what() const noexcept override
    {
        return msg.c_str();
    }

    int error_code() const noexcept
    {
        return err_code;
    }

    const char* error_descr() const noexcept;

    static std::string build_message(std::string const& what, int err_code);

private:
    static const char* error_descr(int code) noexcept;

    std::string msg;
    int err_code;
};

//
// RAII helper
//
template<typename Setter, typename Resetter>
class RAII_helper
{
    Resetter resetter_;

public:
    RAII_helper(Setter&& setter, Resetter&& resetter)
            : resetter_(std::move(resetter))
    {
        setter();
    }

    ~RAII_helper()
    {
        try {
            resetter_();
        }
        catch(...) {}
    }
};

//
// Modbus options
//
namespace modbus_option {

inline constexpr std::initializer_list<int> baud_rates =
{
    9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
};

inline constexpr std::initializer_list<std::pair<char, const char*>> parity =
{
    { 'N', "None" }, { 'E', "Even" }, { 'O', "Odd" }
};

inline constexpr std::initializer_list<int> data_bits =
{
    5, 6, 7, 8
};

inline constexpr std::initializer_list<int> stop_bits =
{
    1, 2
};

} // namspace modbus_option

#endif //MODBUS_TESTER_MB_TESTER_COMMON_H
