#include <string>
#include "../types/ScratchTypes.h"
#include "../scheduler.h"

namespace ts::block {
void sound_stopallsounds();
std::string sound_sounds_menu(std::string sound);
void sound_changevolumeby(ts::Number v);
void sound_setvolumeto(ts::Number v);

void sound_play(std::string sound);
ts::Thread sound_playuntildone(std::string sound);

ts::Number sound_volume();
};