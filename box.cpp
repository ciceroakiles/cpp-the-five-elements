#include "box.h"

void Box::setPosition(int x, int y) {
    Box::x_pos = x;
    Box::y_pos = y;
}

void Box::newWindow(BITMAP *buffer) {
    BITMAP *bufbkp = buffer;
    rect(bufbkp, Box::x_pos, Box::y_pos+70, Box::x_pos+50, Box::y_pos, -1);
    draw_sprite(buffer, bufbkp, 0, 0);
    // For debugging purposes
    //textprintf_ex(buffer, font, Box::x_pos, Box::y_pos, makecol(255,255,255), -1, "%d\n%d", Box::x_pos, Box::y_pos);
}
