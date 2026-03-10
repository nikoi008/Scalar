#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include <time.h>

typedef enum{
    EASY,
    HARD,
    HARDER
}difficulty;
typedef enum{
    CLIMB_MOUNTAIN,
    FIT_SHAPE, // toddler toy things
    DROP_WEIGHT, 
    GUESS_WEIGHT, //get pngs of like 5 things, make the user type in a text ox
    INFLATE_BALOON, //User holds until a specific time - too small = object goes further down, too large- it pops

}Games;

char texts[8][16] = {"CLIMB!","FIT!","DROP!","ALTERNATE!","INFLATE!"};
bool playGame(Games currentGame){
    switch(currentGame){
        case CLIMB_MOUNTAIN:
            DrawText("CLimbing",100,100,100,BLACK);
                return true; // return whether you won the minigame
            break;
        case FIT_SHAPE:
            DrawText("Firritng",100,100,100,BLACK);
                return true; // return whether you won the minigame
            break;
        case DROP_WEIGHT:
            DrawText("dROPPING",100,100,100,BLACK);
                return true; // return whether you won the minigame
            break;
        case INFLATE_BALOON:
            DrawText("inflating",100,100,100,BLACK);
                return true; // return whether you won the minigame
            break;
        case(GUESS_WEIGHT):
            DrawText("guessing",100,100,100,BLACK);
            return true;
            break;
        
    }
    return true; // return whether you won the minigame
}

void happyTransition(){
    DrawText("Happy",100,100,100,BLACK);
}
void loseTransition(){
    DrawText("LOST",100,100,100,BLACK);
}
void drawHearts(){
    DrawText("hearts",100,100,100,BLACK);
}
void drawScore(){
    DrawText("score",100,300,100,BLACK);
}
bool DrawHoverButton(float x, float y,float width, float height,float outlineSize,Color outlineColor,
                     Color  fillColor, const char *text,int baseFontSize,Color textColor){
    static float extraSize = 0;
    static double lastGrow = 0;

    const float MAX_SCALE = 1.1f;

    Vector2 mouse = GetMousePosition();
    Rectangle baseRect = {x, y, width, height};  

    bool hovered = CheckCollisionPointRec(mouse, baseRect);

    double time = GetTime();

    float maxExtra = width * (MAX_SCALE - 1.0f);

    if (hovered) {
        if (time - lastGrow > 0.005 && extraSize < maxExtra) {
            extraSize += 2;
            if (extraSize > maxExtra) extraSize = maxExtra;
            lastGrow = time;
        }
    } else {
        extraSize = 0;
    }

    float drawW = width + extraSize;
    float drawH = height + extraSize;

    float drawX = x - extraSize/2;
    float drawY = y - extraSize/2;

    Rectangle drawRect = {drawX, drawY, drawW, drawH};

    DrawRectangleRec(drawRect, fillColor);
    DrawRectangleLinesEx(drawRect, outlineSize, outlineColor);

    float scale = (drawW / width) * 1.5;
    int fontSize = (int)(baseFontSize * scale);

    int textWidth = MeasureText(text, fontSize);

    DrawText(
        text,
        drawRect.x + drawRect.width/2 - textWidth/2,
        drawRect.y + drawRect.height/2 - fontSize/2,
        fontSize,
        textColor
    );

    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
typedef enum {
    MENU,
    TRANSITION_START,
    PLAYING,
    TRANSITION_END,
    STATS
} GameState;
Games chooseGame(){
    return rand() % 5;
}
void showMouse(){
    DrawText("Mouse",100,100,100,BLACK);
}
void showKeyBoard(){
    DrawText("Keyboard",100,100,100,BLACK);
}
void showRequiredControls(Games currentGame){
    switch(currentGame){
        case CLIMB_MOUNTAIN:
            showKeyBoard();
            break;
        case  FIT_SHAPE:
            showMouse();
            break;
        case GUESS_WEIGHT:
        //click on the closest one?
            showMouse();
            break;
        case INFLATE_BALOON:
            showMouse();
            break;
        
    }
}
GameState state = MENU;

void showSnippet(Games currentGame){
    DrawText(TextFormat("%s",texts[currentGame]),100,200,100,BLACK);
}
void playOpeningTransition(){
    DrawText("Opening Transition",100,300,100,BLACK);
}
void playClosingTransition(){
    DrawText("CLosing Transition",100,400,100,BLACK);
}

int main() {
    srand(time(NULL));
    Games currentGame;
    bool won = false;
    int startime;
    InitWindow(1000, 1000, "Scalar");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(state) {

            case MENU:
            if (DrawHoverButton(300, 300, 220, 80,4,BLACK, GRAY,"START",30,BLACK)){
                state = TRANSITION_START;
                WaitTime(0.5);
            }
            break;

            case TRANSITION_START:
                playOpeningTransition();
                currentGame = chooseGame();
                
                showRequiredControls(currentGame);
                showSnippet(currentGame); // CLIMB! etc
                state = PLAYING;
                playClosingTransition();
                startime = GetTime();
                WaitTime(0.5);

                break;

            case PLAYING:
                won = playGame(currentGame);
                state = TRANSITION_END;
                WaitTime(0.5);
                break;

            case TRANSITION_END:
                if(won){
                    happyTransition(); // screen
                }else{
                    loseTransition();//cracked screen
                }
                state = STATS;
                WaitTime(0.5);
                break;


            case STATS:
                drawHearts();
                drawScore();
                state = TRANSITION_START;
                WaitTime(0.5);
                break;

        }

        EndDrawing();
    }

    CloseWindow();
}