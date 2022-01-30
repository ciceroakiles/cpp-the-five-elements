#include <cstdlib>
#include <iostream>
#include <string>
#include "box.h"

#define WIDTH 800
#define HEIGHT 600
#define SPEED 75
#define X_ADJ 20
#define Y_ADJ 60
#define OP_CARDS_Y 150-Y_ADJ
#define PL_CARDS_Y 500-Y_ADJ
#define DESC_X 630-X_ADJ

using namespace std;

PALETTE pal;
BITMAP *buffer;

// For visual effects
BITMAP *fundo, *moldura;
BITMAP *vapor, *lotus;

// For card resources
BITMAP *cardimgon[5], *cardimgoff[5];

// For card placement and the like
int chosenCardPL = -1, chosenCardOP = -1;
int cards[2][5];

// To determine who wins
int winner = -2;
int winnerMatrix[5][5] = {{0, -1, 1, 1, -1},
                          {1, 0, -1, -1, 1},
                          {-1, 1, 0, 1, -1},
                          {-1, 1, -1, 0, 1},
                          {1, -1, 1, -1, 0}};

// Paths
string BMP_PATH = "resources\\bmp";
string SOUND_PATH = "resources\\sound";

// Sounds
SAMPLE *up1, *up2, *beep;
MIDI *musica;

void printDebugMessages(int x, string str, int pos, int place) {
    // For debugging purposes
    textprintf_ex(buffer, font, 100, 20, makecol(255,255,255), -1, " | SQUARE_X: %d | SELECT: %d", pos, place);
    textprintf_ex(buffer, font, x, 10, makecol(255,255,255), -1, str.c_str());
    textprintf_ex(buffer, font, 10, 20, makecol(255,255,255), -1, "TEXT_X: %d", x);
    textprintf_ex(buffer, font, 10, 30, makecol(255,255,255), -1, "%d>key[KEY_LEFT ]: %d", KEY_LEFT, key[KEY_LEFT]);
	textprintf_ex(buffer, font, 10, 40, makecol(255,255,255), -1, "%d>key[KEY_RIGHT]: %d", KEY_RIGHT, key[KEY_RIGHT]);
    textprintf_ex(buffer, font, 10, 50, makecol(255,255,255), -1, "%d>key[KEY_UP   ]: %d", KEY_UP, key[KEY_UP]);
	textprintf_ex(buffer, font, 10, 60, makecol(255,255,255), -1, "%d>key[KEY_DOWN ]: %d", KEY_DOWN, key[KEY_DOWN]);
	textprintf_ex(buffer, font, 10, 70, makecol(255,255,255), -1, "%d>key[KEY_SPACE]: %d", KEY_SPACE, key[KEY_SPACE]);
	for (int i = 0; i < 5; i++) {
        textprintf_ex(buffer, font, 230+(i*50), 30, makecol(255,255,255), -1, " | %d ", cards[0][i]);
        textprintf_ex(buffer, font, 230+(i*50), 40, makecol(255,255,255), -1, " | %d ", cards[1][i]);
    }
    textprintf_ex(buffer, font, 230, 50, makecol(255,255,255), -1, " CHOSEN: %d ", chosenCardPL);
    textprintf_ex(buffer, font, 230, 60, makecol(255,255,255), -1, " CPU_CARD: %d ", chosenCardOP);
    textprintf_ex(buffer, font, 230, 70, makecol(255,255,255), -1, " WINNER: %d ", winner);
}

void printScore(BITMAP *buffer, int pts1, int pts2) {
    textprintf_ex(buffer, font, 100-X_ADJ, 470-Y_ADJ, makecol(255,255,255), -1, "WINS: %d", pts1);
    textprintf_ex(buffer, font, 100-X_ADJ, 250-Y_ADJ, makecol(255,255,255), -1, "WINS: %d", pts2);
}

void printCardDescription(BITMAP *buffer, string name, string strong, string weak) {
    textprintf_ex(buffer, font, DESC_X, 220-Y_ADJ, makecol(255,255,255), -1, (name + " CARD").c_str());
    textprintf_ex(buffer, font, DESC_X, 450-Y_ADJ, makecol(255,255,255), -1, "STRONG AGAINST:");
    textprintf_ex(buffer, font, DESC_X, 460-Y_ADJ, makecol(255,255,255), -1, ("- " + strong).c_str());
    textprintf_ex(buffer, font, DESC_X, 480-Y_ADJ, makecol(255,255,255), -1, "WEAK AGAINST:");
    textprintf_ex(buffer, font, DESC_X, 490-Y_ADJ, makecol(255,255,255), -1, ("- " + weak).c_str());
}

void printFullCardDesc(BITMAP *buffer, int place) {
    switch (place) {
        case 0: printCardDescription(buffer, "FIRE", "METAL, WOOD", "EARTH, WATER"); break;
        case 1: printCardDescription(buffer, "EARTH", "FIRE, WATER", "METAL, WOOD"); break;
        case 2: printCardDescription(buffer, "METAL", "EARTH, WOOD", "FIRE, WATER"); break;
        case 3: printCardDescription(buffer, "WOOD", "EARTH, WATER", "FIRE, METAL"); break;
        case 4: printCardDescription(buffer, "WATER", "FIRE, METAL", "EARTH, WOOD"); break;
        default: break;
    }
    // Print tip
    if (chosenCardPL == -1) {
        textprintf_ex(buffer, font, DESC_X, 530-Y_ADJ, makecol(255,255,255), -1, "PRESS [UP]");
        textprintf_ex(buffer, font, DESC_X, 540-Y_ADJ, makecol(255,255,255), -1, "TO SELECT THIS CARD.");
    }
}

void printConfirmMessage(BITMAP *buffer) {
    textprintf_ex(buffer, font, 400-X_ADJ, 425-Y_ADJ, makecol(255,255,255), -1, "[SPACE]: CONFIRM");
    textprintf_ex(buffer, font, 400-X_ADJ, 435-Y_ADJ, makecol(255,255,255), -1, "[DOWN]: CANCEL");
}

void displayArrows(BITMAP *buffer) {
    BITMAP *bufbkp = buffer;
    // Left
    line(bufbkp, 50-X_ADJ, 535-Y_ADJ, 80-X_ADJ, 520-Y_ADJ, -1);
    line(bufbkp, 50-X_ADJ, 535-Y_ADJ, 80-X_ADJ, 550-Y_ADJ, -1);
    line(bufbkp, 80-X_ADJ, 550-Y_ADJ, 80-X_ADJ, 520-Y_ADJ, -1);
    // Right
    line(bufbkp, 600-X_ADJ, 535-Y_ADJ, 570-X_ADJ, 520-Y_ADJ, -1);
    line(bufbkp, 600-X_ADJ, 535-Y_ADJ, 570-X_ADJ, 550-Y_ADJ, -1);
    line(bufbkp, 570-X_ADJ, 550-Y_ADJ, 570-X_ADJ, 520-Y_ADJ, -1);
    draw_sprite(buffer, bufbkp, 0, 0);
}

void displayCardPositions(BITMAP *buffer) {
    int x_pos = 0;
    // All possible choices
    for (int i = 1; i < 6; i++) {
        x_pos = i*100-X_ADJ;
        Box cardPlace1, cardPlace2;
        cardPlace1.setPosition(x_pos, OP_CARDS_Y);
        cardPlace1.newWindow(buffer);
        cardPlace2.setPosition(x_pos, PL_CARDS_Y);
        cardPlace2.newWindow(buffer);
        cards[0][i-1] = x_pos;
    }
    // Central places
    Box chosenCard1, chosenCard2;
    chosenCard1.setPosition(300-X_ADJ, 250-Y_ADJ);
    chosenCard1.newWindow(buffer);
    chosenCard2.setPosition(300-X_ADJ, 400-Y_ADJ);
    chosenCard2.newWindow(buffer);
}

void displayCursor(BITMAP *buffer, int &x, int &y) {
    // Double buffering
    BITMAP *bufbkp = buffer;
    rect(bufbkp, x-3, y+73, x+53, y-3, -1);
    draw_sprite(buffer, bufbkp, 0, 0);
}

void displayTableBitmaps(BITMAP *buffer) {
    // One buffer to rule them all
    BITMAP *cardbuf;
    for (int i = 0; i < 5; i++) {
        stretch_sprite(cardbuf, ((cards[1][i]) ? cardimgon[i] : cardimgoff[i]), cards[0][i], PL_CARDS_Y, 50, 70);
    }
    draw_sprite(buffer, cardbuf, 0, 0);
}

void displayAllOPCards(BITMAP *buffer) {
    // One buffer to rule them all
    BITMAP *cardbuf = buffer;
    for (int i = 0; i < 5; i++) {
        stretch_sprite(cardbuf, cardimgon[i], cards[0][i], OP_CARDS_Y, 50, 70);
    }
    draw_sprite(buffer, cardbuf, 0, 0);
}

void displayAllPLCards(BITMAP *buffer, int &place) {
    BITMAP *bufbkp = buffer;
    BITMAP *cardimgselected;
    displayTableBitmaps(buffer);
    // Change card description
    printFullCardDesc(buffer, place);
    // Change card
    draw_sprite(buffer, cardimgoff[place], 630-X_ADJ, 230-Y_ADJ);
    draw_sprite(buffer, bufbkp, 0, 0);
}

void displayOPCard(BITMAP *buffer) {
    if (chosenCardOP != -1) {
        BITMAP *cardbuf = buffer;
        stretch_sprite(cardbuf, cardimgoff[chosenCardOP], 300-X_ADJ, 250-Y_ADJ, 50, 70);
        draw_sprite(buffer, cardbuf, 0, 0);
    }
}

void displayUpperHit() {
    draw_sprite(buffer, vapor, 290-X_ADJ, 240-Y_ADJ);
}

void displayLowerHit() {
    draw_sprite(buffer, vapor, 290-X_ADJ, 390-Y_ADJ);
}

void doMoveCursor(BITMAP *buffer, int &place, int &screen_pos) {
    BITMAP *bufbkp = buffer;
    // Cursor movement
    if (key[KEY_LEFT]) {
        place--;
        if (place < 0) {
            place = 0;
        } else screen_pos -= 100;
        triangle(bufbkp, 50-X_ADJ, 535-Y_ADJ, 80-X_ADJ, 520-Y_ADJ, 80-X_ADJ, 550-Y_ADJ, -1);
        draw_sprite(buffer, bufbkp, 0, 0);
    }
    if (key[KEY_RIGHT]) {
        place++;
        if (place > 4) {
            place = 4;
        } else screen_pos += 100;
        triangle(bufbkp, 600-X_ADJ, 535-Y_ADJ, 570-X_ADJ, 520-Y_ADJ, 570-X_ADJ, 550-Y_ADJ, -1);
        draw_sprite(buffer, bufbkp, 0, 0);
    }
}

void doChooseCard(int pos) {
    if (key[KEY_UP]) {
        if (cards[1][pos] == 0) play_sample(up1, 100, 128, 1000, FALSE);
        // Choose one and deselect the others
        cards[1][pos] = 1;
        for (int i = 0; i < 5; i++) {
            if (i != pos) cards[1][i] = 0;
        }
    }
}

void doCancel(bool autoCancel) {
    // Cancellation
    if (key[KEY_DOWN] || autoCancel) {
        if (chosenCardPL != -1) {
            for (int i = 0; i < 5; i++) {
                cards[1][i] = 0;
            }
            if (!autoCancel) play_sample(beep, 100, 128, 1000, FALSE);
            chosenCardPL = -1;
        }
    }
}

bool doConfirmDialog(BITMAP *buffer, int card) {
    // Display the card
    chosenCardPL = card;
    stretch_sprite(buffer, cardimgoff[chosenCardPL], 300-X_ADJ, 400-Y_ADJ, 50, 70);
    // Confirmation
    if (key[KEY_SPACE]) {
        play_sample(up2, 100, 128, 1000, FALSE);
        // Opponent's turn
        srand((unsigned) time(NULL));
        chosenCardOP = (rand() % 5);
        return true;
    } else printConfirmMessage(buffer);
    return false;
}

void checkCenterCard() {
    bool flagSelect = false;
    for (int i = 0; i < 5; i++) {
        if (cards[1][i] != 0) flagSelect = true;
    }
    if (!flagSelect) chosenCardPL = -1;
}

void combatAnimation(int res) {
    switch (res) {
        case 1: {
            // Win
            draw_sprite(buffer, lotus, 210-X_ADJ, 340-Y_ADJ);
            draw_sprite(buffer, lotus, 400-X_ADJ, 340-Y_ADJ);
            textprintf_ex(buffer, font, 300-X_ADJ, 360-Y_ADJ, makecol(255,255,255), -1, "YOU WIN!");
            displayUpperHit();
        } break;
        case 0: {
            // Draw
            textprintf_ex(buffer, font, 300-X_ADJ, 360-Y_ADJ, makecol(255,255,255), -1, "DRAW");
            displayUpperHit();
            displayLowerHit();
        } break;
        case -1: {
            // Lose
            textprintf_ex(buffer, font, 300-X_ADJ, 360-Y_ADJ, makecol(255,255,255), -1, "YOU LOSE!");
            displayLowerHit();
        } break;
        default: break;
    }
}

void battleResults(bool &plCard, bool &opCard, int &pts1, int &pts2) {
    if (plCard && opCard) {
        winner = winnerMatrix[chosenCardPL][chosenCardOP];
        switch (winner) {
            case 1: pts1++; break;
            case 0: break;
            case -1: pts2++; break;
            default: break;
        }
        combatAnimation(winner);
        winner = -2;
        doCancel(true);
        opCard = false;
        plCard = false;
    }
}

void preload() {
    set_window_title("The Five Elements");
	buffer = create_bitmap(WIDTH, HEIGHT);
	// Background
    fundo = load_bitmap((BMP_PATH + "\\fundo.bmp").c_str(), pal);
    moldura = load_bitmap((BMP_PATH + "\\moldura.bmp").c_str(), pal);
    vapor = load_bitmap((BMP_PATH + "\\vapor.bmp").c_str(), pal);
    lotus = load_bitmap((BMP_PATH + "\\lotus.bmp").c_str(), pal);
    // Card resources
    for (int i = 0; i < 5; i++) {
        std::ostringstream index;
        index << i+1;
        cardimgoff[i] = load_bitmap((BMP_PATH + "\\card0" + index.str() + "R.bmp").c_str(), pal);
        cardimgon[i] = load_bitmap((BMP_PATH + "\\card0" + index.str() + "V.bmp").c_str(), pal);
    }
    // Music and sounds
    musica = load_midi((SOUND_PATH + "\\Gao_Shan_Liu_Shui.mid").c_str());
    up1 = load_sample((SOUND_PATH + "\\up1.wav").c_str());
    up2 = load_sample((SOUND_PATH + "\\up2.wav").c_str());
    beep = load_sample((SOUND_PATH + "\\wrong1.wav").c_str());
}
