#include "pushbutton_play_stop.h"

void pushbutton_play_stop::update_status(bool active)
{
    setText(active ? "Disconnect" : "Connect");
    setIcon(active ? QIcon(":/img/ico_stop.png") : QIcon(":/img/ico_start.png"));
}