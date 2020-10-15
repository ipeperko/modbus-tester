#include "server_reg_model.h"
#include <QSettings>

server_reg_model::server_reg_model(QObject* parent)
    : QAbstractTableModel(parent)
{
    memset(&mb_map, 0, sizeof(modbus_mapping_t));

    QSettings set;

    buff_holding_reg = set.value(("server_mb_reg/holding_reg")).value<QVector<mb_register_t>>();
    buff_holding_reg.resize(buffer_size);
    mb_map.nb_registers = buffer_size;
    mb_map.tab_registers = buff_holding_reg.data();

    buff_input_reg = set.value(("server_mb_reg/input_reg")).value<QVector<mb_register_t>>();
    buff_input_reg.resize(buffer_size);
    mb_map.nb_input_registers = buffer_size;
    mb_map.tab_input_registers = buff_input_reg.data();

    buff_coils = set.value(("server_mb_reg/coils")).value<QVector<mb_input_t>>();
    buff_coils.resize(buffer_size);
    mb_map.nb_bits = buffer_size;
    mb_map.tab_bits = buff_coils.data();

    buff_discrete_inputs = set.value(("server_mb_reg/discrete_inputs")).value<QVector<mb_input_t>>();
    buff_discrete_inputs.resize(buffer_size);
    mb_map.nb_input_bits = buffer_size;
    mb_map.tab_input_bits = buff_discrete_inputs.data();
}

server_reg_model::~server_reg_model()
{
    QSettings set;

    set.setValue("server_mb_reg/holding_reg", QVariant::fromValue(buff_holding_reg));
    set.setValue("server_mb_reg/input_reg", QVariant::fromValue(buff_input_reg));
    set.setValue("server_mb_reg/coils", QVariant::fromValue(buff_coils));
    set.setValue("server_mb_reg/discrete_inputs", QVariant::fromValue(buff_discrete_inputs));
}

int server_reg_model::rowCount(const QModelIndex&) const
{
    return modbus_num_data_types;
}

int server_reg_model::columnCount(const QModelIndex&) const
{
    return buffer_size;
}

Qt::ItemFlags server_reg_model::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant server_reg_model::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    if (index.row() >= modbus_num_data_types || index.row() < 0)
        return {};

    if (index.column() >= buffer_size || index.column() < 0)
        return {};

    if (role == Qt::DisplayRole) {

        auto didx = static_cast<mb_dropdown_data_index_t>(index.row());

        switch(didx) {
            case mb_dropdown_data_index_t::holding_registers :
                return static_cast<int16_t>(buff_holding_reg.at(index.column()));
            case mb_dropdown_data_index_t::input_registers :
                return static_cast<int16_t>(buff_input_reg.at(index.column()));
            case mb_dropdown_data_index_t::coils :
                return static_cast<int>(buff_coils.at(index.column()));
            case mb_dropdown_data_index_t::discrete_inputs :
                return static_cast<int>(buff_discrete_inputs.at(index.column()));
            default:
                return {};
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    return {};
}

bool server_reg_model::setData(const QModelIndex& index, const QVariant& val, int role)
{
    if (index.isValid() && role == Qt::EditRole) {

        if (index.row() >= modbus_num_data_types || index.row() < 0)
            return false;

        if (index.column() >= buffer_size || index.column() < 0)
            return false;

        auto didx = static_cast<mb_dropdown_data_index_t>(index.row());
        bool result = true;
        bool ok;
        int vint = val.toInt(&ok);

        if (!ok)
            return false;

        switch(didx) {
            case mb_dropdown_data_index_t::holding_registers :
                buff_holding_reg[index.column()] = static_cast<uint16_t>(vint);
                break;
            case mb_dropdown_data_index_t::input_registers :
                buff_input_reg[index.column()] = static_cast<uint16_t>(vint);
                break;
            case mb_dropdown_data_index_t::coils :
                if (vint != 0 && vint != 1) {
                    vint = 0;
                }
                buff_coils[index.column()] = static_cast<uint8_t>(vint);
                break;
            case mb_dropdown_data_index_t::discrete_inputs :
                if (vint != 0 && vint != 1) {
                    vint = 0;
                }
                buff_discrete_inputs[index.column()] = static_cast<uint8_t>(vint);
                break;
            default:
                result = false;
        }

        if (result) {
            emit dataChanged(index, index);
        }
        return result;
    }

    return false;
}

QVariant server_reg_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {

        if (orientation == Qt::Horizontal) {
            return section;
        }
        else if (orientation == Qt::Vertical) {
            switch(section) {
                case static_cast<int>(mb_dropdown_data_index_t::holding_registers) :
                    return "Holding registers";
                case static_cast<int>(mb_dropdown_data_index_t::input_registers) :
                    return "Input registers";
                case static_cast<int>(mb_dropdown_data_index_t::coils) :
                    return "Coils";
                case static_cast<int>(mb_dropdown_data_index_t::discrete_inputs) :
                    return "Discrete inputs";
                default:
                    return {};
            }
        }
    }

    return {};
}

void server_reg_model::refresh_all()
{
    QModelIndex i1 = index(0, 0);
    emit dataChanged(i1, index(rowCount(i1) - 1, columnCount(i1) - 1));
}
