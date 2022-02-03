#include "box.h"

void Box::setDimensions(int x, int y, int width, int height) {
    Box::x_pos = x;
    Box::y_pos = y;
    Box::width = width;
    Box::height = height;
}

void Box::newBox(BITMAP *buffer) {
    BITMAP *bufbkp = buffer;
    rect(bufbkp, Box::x_pos, Box::y_pos+Box::height, Box::x_pos+Box::width, Box::y_pos, -1);
    draw_sprite(buffer, bufbkp, 0, 0);
    // For debugging purposes
    //textprintf_ex(buffer, font, Box::x_pos, Box::y_pos, makecol(255,255,255), -1, "%d\n%d", Box::x_pos, Box::y_pos);
}
