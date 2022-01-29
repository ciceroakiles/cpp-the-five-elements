#include <allegro.h>
#include <winalleg.h>

class Box {
    int x_pos, y_pos;
    
    public:
        void setPosition(int x, int y);
        void newWindow(BITMAP *buffer);
};
