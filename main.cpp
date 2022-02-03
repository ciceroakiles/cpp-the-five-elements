#include "game.h"

void init();
void deinit();

int main() {
    init();
    preload();
    // Music
    bool playMusic = true;
    if (playMusic) {
        if (!musica) allegro_message("Couldn't load midi!");
        else play_midi(musica, TRUE);
        set_volume(200, 150);
    }
    // Main loop
    do {
        Sleep(SPEED);
        if (!gameStarted) {
            titleScreen();
            if (quitGame) break;
        } else {
            gameLoop();
        }
    } while (!key[KEY_ESC]);
    // Destroy objects
    destroy_bitmap(backgr);
    destroy_bitmap(frame);
    destroy_bitmap(title);
    destroy_bitmap(buffer);
    destroy_midi(musica);
    destroy_sample(beep);
    destroy_sample(up2);
    destroy_sample(up1);
    // Finish
    deinit();
    return 0;
}
END_OF_MAIN()

void init() {
    int depth, res;
    allegro_init();
    depth = desktop_color_depth();
    if (depth == 0) depth = 32;
    set_color_depth(depth);
    res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, WIDTH, HEIGHT, 0, 0);
    if (res != 0) {
    	allegro_message(allegro_error);
    	exit(-1);
    }
    install_timer();
    install_keyboard();
    install_mouse();
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
}

void deinit() {
    clear_keybuf();
}
