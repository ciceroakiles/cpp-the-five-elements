#include <sstream>
#include <time.h>
#include "functions.h"

void init();
void deinit();

int main() {
	init();
	preload();
	// For Debug Mode
	int xText = 10;
	bool debugMode = false;
    // More variables
    int wCursor = 100-X_ADJ, hCursor = 500-Y_ADJ;
    int cValue = 0, centerCard = -1;
    // Counters
    int plWins = 0, opWins = 0;
    // Check permissions
    bool tableExists = true;
    bool playerCards = true, opponentCards = true;
    bool canMove = true, canChoose = true, canCancel = true;
    // Confirmations
    bool PLhasCenterCard = false, OPhasCenterCard = false;
    // Music
    if (!musica) allegro_message("Couldn't load midi!");
    else play_midi(musica, TRUE);
    set_volume(200, 150);
    // Main game loop
    do {
        Sleep(SPEED);
		draw_sprite(buffer, fundo, 0, 0);
		draw_sprite(buffer, moldura, 0, 0);
		// Messages and text movement
    	if (debugMode) {
        	printDebugMessages(xText, "DEBUG_MODE_ON", wCursor, cValue);
    		xText += 10;
            if (!(xText+10 < WIDTH)) xText = -30;
        }
        // Score
        printScore(buffer, plWins, opWins);
        // Directions
        displayArrows(buffer);
        // Cursor and card positions
        if (tableExists) displayCardPositions(buffer);
        displayCursor(buffer, wCursor, hCursor);
        // Display cards
        if (opponentCards) displayAllOPCards(buffer);
        if (playerCards) displayAllPLCards(buffer, cValue);
        // Basic controls
        if (canMove) doMoveCursor(buffer, cValue, wCursor);
        if (canChoose) doChooseCard(cValue);
        // Look for the chosen one
        for (int i = 0; i < 5; i++) {
            if (cards[1][i]) {
                centerCard = i;
                PLhasCenterCard = true;
                break;
            }
            PLhasCenterCard = false;
        }
        // Chosen card in center: wait for cancel or confirmation
        if (PLhasCenterCard) {
            if (canCancel) doCancel(false);
            if (doConfirmDialog(buffer, centerCard)) OPhasCenterCard = true;
            checkCenterCard();
        }
        bool flagReady = false;
        if (OPhasCenterCard) {
            // Show opponent's card
            displayOPCard(buffer);
            // Result
            battleResults(PLhasCenterCard, OPhasCenterCard, plWins, opWins);
            flagReady = true;
        }
        // Draw and redraw
    	draw_sprite(screen, buffer, 0, 0);
    	if (flagReady) {
            // Wait before clearing
            Sleep(SPEED*30);
            // Reset cursor position
            wCursor = 100-X_ADJ;
            hCursor = 500-Y_ADJ;
            cValue = 0;
            flagReady = false;
        }
    	clear_bitmap(buffer);
	} while (!key[KEY_ESC]);
	// Destroy objects
	destroy_bitmap(fundo);
	destroy_bitmap(moldura);
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
