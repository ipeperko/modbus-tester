#ifndef MODBUS_TESTER_IPV4_VALIDATOR_H
#define MODBUS_TESTER_IPV4_VALIDATOR_H

#include <QValidator>

// stolen from https://forum.qt.io/topic/5619/solved-qlineedit-validator-doesn-t-allow-to-input-ip-address
class IPv4_validator : public QValidator
{
public:
    explicit IPv4_validator(QObject *parent=nullptr)
        : QValidator(parent)
    {}

    void fixup(QString &input) const override
    {}

    State validate(QString &input, int &pos) const override
    {
        if(input.isEmpty())
            return Acceptable;

        QStringList slist = input.split(".");
        if(slist.size() > 4)
            return Invalid;

        bool emptyGroup = false;

        for (auto const& it : slist) {
            bool ok;
            if(it.isEmpty()){
                emptyGroup = true;
                continue;
            }
            int val = it.toInt(&ok);
            if(!ok || val<0 || val>255)
                return Invalid;
        }

        if(slist.size() < 4 || emptyGroup)
            return Intermediate;

        return Acceptable;
    }
};

#endif //MODBUS_TESTER_IPV4_VALIDATOR_H
