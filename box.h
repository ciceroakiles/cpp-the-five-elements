#include <allegro.h>
#include <winalleg.h>

class Box {
    int x_pos, y_pos, width, height;
    
    public:
        void setDimensions(int x, int y, int width, int height);
        void newBox(BITMAP *buffer);
};
