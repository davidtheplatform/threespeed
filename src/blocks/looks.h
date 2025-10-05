#pragma once
#include <string>
#include "../Sprite.h"
#include "../types/String.h"

namespace ts::block
{
    void looks_hide();
    void looks_show();

    void looks_setsizeto(double size);
    ts::Number looks_size();

    void looks_switchcostumeto(ts::String costume);
    ts::String looks_costume(ts::String costume);

    void looks_switchbackdropto(ts::String backdrop);
    ts::String looks_backdrops(ts::String backdrop);

    ts::Value looks_costumenumbername(ts::String option);

    void looks_seteffectto(ts::Number value, ts::String effect);
    void looks_changeeffectby(ts::Number value, ts::String effect);
    void looks_cleargraphiceffects();
};