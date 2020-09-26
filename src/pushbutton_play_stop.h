#ifndef MODBUS_TESTER_PUSHBUTTON_PLAY_STOP_H
#define MODBUS_TESTER_PUSHBUTTON_PLAY_STOP_H

#include <QPushButton>

class pushbutton_play_stop : public QPushButton
{
    Q_OBJECT
public:
    pushbutton_play_stop(QWidget* parent = nullptr)
        : QPushButton(parent)
    {}

    void update_status(bool active);
};
#endif //MODBUS_TESTER_PUSHBUTTON_PLAY_STOP_H
