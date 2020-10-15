#ifndef MODBUS_TESTER_SERVER_REG_MODEL_H
#define MODBUS_TESTER_SERVER_REG_MODEL_H

#include "mb_tester_common.h"
#include <QAbstractTableModel>
#include <modbus/modbus.h>

class server_reg_model : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit server_reg_model(QObject* parent = nullptr);
    ~server_reg_model();

    modbus_mapping_t& mapping() { return mb_map; }
    modbus_mapping_t const& mapping() const { return mb_map; }

    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;

    static constexpr int buffer_size = 8192;

private:
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& val, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    modbus_mapping_t mb_map;
    QVector<mb_register_t> buff_holding_reg;
    QVector<mb_register_t> buff_input_reg;
    QVector<mb_input_t> buff_coils;
    QVector<mb_input_t> buff_discrete_inputs;

public slots:
    void refresh_all();
};

#endif //MODBUS_TESTER_SERVER_REG_MODEL_H
