#include "motion.h"
#include "scheduler.h"
#include "../rendering/Pen.h"
#include <cmath>

namespace ts::block {
    void motion_movesteps(double steps) {
        int x1 = (int) current_sprite->x;
        int y1 = (int) current_sprite->y;
        
        double angle = current_sprite->direction * M_PI / 180;

        current_sprite->x += cos(angle) * steps;
        current_sprite->y += sin(angle) * steps;

        draw_line(current_sprite, x1, y1, current_sprite->x, current_sprite->y);
    }
    void motion_turnright(double degrees) {
        ts::current_sprite->direction = fmod(ts::current_sprite->direction + degrees, 360);
    }

    void motion_turnleft(double degrees) {
        ts::current_sprite->direction = fmod(ts::current_sprite->direction - degrees, 360);
    }

    void motion_gotoxy(double x, double y) {
        int x1 = (int) current_sprite->x;
        int y1 = (int) current_sprite->y;

        current_sprite->x = x;
        current_sprite->y = y;

        draw_line(current_sprite, x1, y1, x, y);
    }

    void motion_goto(Sprite* to) {
        motion_gotoxy(to->x, to->y);
    }

    void motion_pointindirection(ts::Number d) {
        current_sprite->direction = (double) d;
    }
    void motion_setrotationstyle(ts::String mode) {
        current_sprite->rotation_mode = std::string(mode);
    }

    void motion_setx(double x) {
        int x1 = (int) current_sprite->x;

        current_sprite->x = x;

        draw_line(current_sprite, x1, current_sprite->y, x, current_sprite->y);
    }

    void motion_sety(double y) {
        int y1 = (int) current_sprite->y;

        current_sprite->y = y;

        draw_line(current_sprite, current_sprite->x, y1, current_sprite->x, y);
    }

    void motion_changexby(double x) {
        int x1 = (int) current_sprite->x;

        current_sprite->x += x;

        draw_line(current_sprite, x1, current_sprite->y, current_sprite->x, current_sprite->y);
    }

    void motion_changeyby(double y) {
        int y1 = (int) current_sprite->y;

        current_sprite->y += y;

        draw_line(current_sprite, current_sprite->x, y1, current_sprite->x, current_sprite->y);
    }

    double motion_xposition() {
        return current_sprite->x;
    }

    double motion_yposition() {
        return current_sprite->y;
    }
}