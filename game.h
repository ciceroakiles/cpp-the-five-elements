#include <time.h>
#include "functions.h"

// For Debug Mode
int xText = 10;
bool debugMode = false;
// For menu window and cursor
int menuWidth = 150, menuHeight = 50;
int xTLCursor = (WIDTH-menuWidth)/2+10, yTLCursor = (HEIGHT/2)+menuHeight+10;
int wncValue = 0;
// For flow control
bool gameStarted = false, quitGame = false;
// In-game counters
int centerCard = -1;
int plWins = 0, opWins = 0;
// In-game cursor
int wGameCursor = 100-X_ADJ, hGameCursor = PL_CARDS_Y;
int gcValue = 0;
// In-game permissions
bool canCancel = true, endGame = false;
// In-game confirmations
bool PLhasCenterCard = false, OPhasCenterCard = false;

void debug() {
    // Messages and text movement
    printDebugMessages(xText, "DEBUG_MODE_ON", gameStarted, wGameCursor, gcValue, 1);
	xText += 10;
    if (!(xText+10 < WIDTH)) xText = -30;
}

void resetGameCursor() {
    // Reset cursor position
    wGameCursor = 100-X_ADJ;
    hGameCursor = PL_CARDS_Y;
    gcValue = 0;
}

void titleScreen() {
    draw_sprite(buffer, backgr, 0, 0);
    draw_sprite(buffer, frame, 0, 0);
    draw_sprite(buffer, title, (WIDTH-320)/2, (HEIGHT/2)-100);
    if (debugMode) debug();
    // Reset game state
    plWins = 0;
    opWins = 0;
    resetGameCursor();
    // Main menu and its cursor
    displayStartMenu(buffer, menuWidth, menuHeight);
    if (key[KEY_UP]) {
        wncValue--;
        if (wncValue < 0) {
            wncValue = 0;
        } else {
            play_sample(up1, 100, 128, 1000, FALSE);
            yTLCursor -= 20;
        }
    }
    if (key[KEY_DOWN]) {
        wncValue++;
        if (wncValue > 1) {
            wncValue = 1;
        } else {
            play_sample(up1, 100, 128, 1000, FALSE);
            yTLCursor += 20;
        }
    }
    displaySTMCursor(buffer, xTLCursor, yTLCursor);
    // Select an option
    if (key[KEY_ENTER]) {
        play_sample(up2, 100, 128, 1000, FALSE);
        switch (wncValue) {
            case 0: gameStarted = true; break;
            case 1: quitGame = true; break;
            default: break;
        }
    }
    // Redraw
    draw_sprite(screen, buffer, 0, 0);
    clear_bitmap(buffer);
}

void gameLoop() {
    draw_sprite(buffer, backgr, 0, 0);
    draw_sprite(buffer, frame, 0, 0);
    if (debugMode) debug();
    printScore(buffer, plWins, opWins);              // Score (showScore)
    displayCardPositions(buffer);                    // Cursor and card positions (tableExists)
    displayAllOPCards(buffer);                       // Display cards (opponentCards)
    displayAllPLCards(buffer, gcValue);              // Display cards (playerCards)
	displayArrows(buffer);                           // Directions and basic controls (canMove)
	displayCursor(buffer, wGameCursor, hGameCursor); // " " (canMove)
	doMoveCursor(buffer, gcValue, wGameCursor);      // " " (canMove)
    doChooseCard(gcValue);                           // " " (canChoose)
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
    if (OPhasCenterCard) {
        // Show opponent's card
        displayOPCard(buffer);
        // Result
        battleResults(PLhasCenterCard, OPhasCenterCard, plWins, opWins);
        endGame = true;
    }
    // Go back to the main menu
    if (key[KEY_BACKSPACE] && !PLhasCenterCard) {
        play_sample(beep, 100, 128, 1000, FALSE);
        gameStarted = false;
    }
    // Redraw
    draw_sprite(screen, buffer, 0, 0);
	if (endGame) {
        Sleep(SPEED*30); // Wait before clearing
        resetGameCursor();
        endGame = false;
    }
    clear_bitmap(buffer);
}
