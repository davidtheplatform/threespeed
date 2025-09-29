#pragma once
#include <string>
#include "../Sprite.h"
#include "../types/ScratchTypes.h"

namespace ts::block
{
    void motion_movesteps(double steps);
    void motion_turnright(double degrees);
    void motion_turnleft(double degrees);

    void motion_gotoxy(double x, double y);
    void motion_goto(Sprite* to);

    void motion_pointindirection(ts::Number d);
    void motion_setrotationstyle(ts::String mode);

    void motion_setx(double x);
    void motion_sety(double y);
    void motion_changexby(double x);
    void motion_changeyby(double y);

    double motion_xposition();
    double motion_yposition();
};