#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include <time.h>
#include <math.h>

#define SCREEN_W 320
#define SCREEN_H 180

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

typedef struct{
    Games gameName;
    int easyDifficultyTime;
    int hardDifficultyTime;
    int harderDifficulatyTime;
}difficultyInfo;

typedef enum{
    PLAYING_STATE,
    LOST_STATE,
    WON_STATE,
}playState;

typedef enum {
    MENU,
    TRANSITION_START,
    PLAYING,
    TRANSITION_END,
    STATS
} GameState;

/*GLOBALS*/
difficultyInfo infos[5] = {{CLIMB_MOUNTAIN,5,4,2},{FIT_SHAPE,4,3,2},{DROP_WEIGHT,4,3,2},{GUESS_WEIGHT,6,5,4},{INFLATE_BALOON,5,4,3,}};
Vector2 virtualMouse = {0};
double startTime = 0; // reset at main
difficulty currentDifficulty = EASY;
GameState state = MENU;

/*MISC FUNCTIONS*/
int getTimeForDifficulty(Games game, difficulty diff){
    switch(diff){
        case EASY:   return infos[game].easyDifficultyTime;
        case HARD:   return infos[game].hardDifficultyTime;
        case HARDER: return infos[game].harderDifficulatyTime;
    }
    return 5;
}

/*----------------CLIMBING GAME----------------*/
int dColour = 0;
int aColour = 1;
Color aCol = RED;
Color dCol = GREEN;
int climb = 0;

playState climbMountain(){
    double maxTime = getTimeForDifficulty(CLIMB_MOUNTAIN,currentDifficulty);
    
    if(aColour == 1 && IsKeyPressed(KEY_A)){
        climb++;
        aColour = 0; dColour = 1;
        dCol = GREEN; aCol = RED;
    }
    else if(dColour == 1 && IsKeyPressed(KEY_D)){
        climb++;
        aColour = 1; dColour = 0;
        dCol = RED; aCol = GREEN;
    }
    
    double elapsed = GetTime() - startTime;
    if(climb >= 30 && elapsed <= maxTime) return WON_STATE;
    if(elapsed > maxTime) return LOST_STATE;

    DrawText("A",100,100,30,aCol);
    DrawText("D",100,70,30,dCol);
    DrawText(TextFormat("%d",climb),100,130,30,BLACK);
    DrawText(TextFormat("%.2f", elapsed),100,160,30,BLACK);
    
    return PLAYING_STATE;
}

/*--------------------------DROP WEIGHT------------------*/
bool dropped = false;
float dropX = 0;
float dropY = 0;

playState dropWeight(Vector2 mousePos){
    if(!dropped){
        DrawRectangle(mousePos.x - 10, mousePos.y - 10, 20, 20, BLACK);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            dropped = true;
            dropX = mousePos.x - 10;
            dropY = mousePos.y - 10;
        }
    } else {
        dropY += 4.0f;
        DrawRectangle(dropX, dropY, 20, 20, BLACK);
        if(dropY >= SCREEN_H - 20) return WON_STATE;
    }
    
    if((GetTime() - startTime) > getTimeForDifficulty(DROP_WEIGHT, currentDifficulty)) return LOST_STATE;
    return PLAYING_STATE;
}

/*--------------------------INFLATE BALLOON------------------*/
static float balloonScale = 1.0f;
playState inflateBalloon(Vector2 mousePos){
    DrawCircle(SCREEN_W/2, SCREEN_H/2, 20 * balloonScale, RED);
    
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) balloonScale += 0.02f;
    
    if(balloonScale > 3.0f) return WON_STATE;
    if((GetTime() - startTime) > getTimeForDifficulty(INFLATE_BALOON, currentDifficulty)) return LOST_STATE;
    
    return PLAYING_STATE;
}

/*--------------------------GUESSING GAME*------------------*/
typedef struct{
    char *name;
    int weight;
}Guessing;

playState guessWeightGame(){
    DrawText("", 100, 100, 20, BLACK);
    if((GetTime() - startTime) > 2.0) return WON_STATE;
    return PLAYING_STATE;
}

/*UI FUNCTIONS*/
char texts[5][16] = {"CLIMB!","FIT!","DROP!","GUESS!","INFLATE!"};

playState playGame(Games currentGame, Vector2 mousePos){
    switch(currentGame){
        case CLIMB_MOUNTAIN: return climbMountain();
        case DROP_WEIGHT:    return dropWeight(mousePos);
        case INFLATE_BALOON: return inflateBalloon(mousePos);
        case GUESS_WEIGHT:   return guessWeightGame();
        case FIT_SHAPE:      return WON_STATE;
        default:             return LOST_STATE;
    }
}

void happyTransition(){ DrawText("WIN",100,80,40,GREEN); }
void loseTransition() { DrawText("FAIL",100,80,40,RED); }
void drawHearts()     { DrawText("Hearts",10, 10, 20, MAROON); }
void drawScore()      { DrawText("Score", 10, 40, 20, BLACK); }

bool DrawHoverButton(float x, float y,float width, float height,float outlineSize,Color outlineColor,
    Color  fillColor, const char *text,int baseFontSize,Color textColor, Vector2 mousePos){
    static float extraSize = 0;
    static double lastGrow = 0;
    const float WOBBLE_AMPLITUDE = 4.0f;
    const float WOBBLE_SPEED = 5.0f;
    const float MAX_SCALE = 1.2f;

    Rectangle baseRect = {x, y, width, height};  

    bool hovered = CheckCollisionPointRec(mousePos, baseRect);

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
    float hoveOffset = 0;
    if(hovered){
        hoveOffset = sinf(time * WOBBLE_SPEED) * WOBBLE_AMPLITUDE;

    }
    float drawW = width + extraSize;
    float drawH = height + extraSize;

    float drawX = x - extraSize/2;
    float drawY = (y - extraSize/2) + hoveOffset;

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

Games chooseGame(){ return rand() % 5; }

bool playOpeningTransition(Texture2D tTexture, Texture2D bTTexture){
    static int i = 0;
    int frames = 60;
    DrawTexture(bTTexture,0,0,WHITE);
    DrawTexture(tTexture,(SCREEN_W / frames) * i,0,WHITE);
    i++;
    if (i >= frames) { i = 0; return true; }
    return false;
}

float scrollX;
float scrollY;
float speed = 40.0f;
void drawBG(Texture2D tBackground){
    float dt = GetFrameTime();
    scrollX -= speed * dt;
    scrollY -= speed * dt;
    if (scrollX <= -320) scrollX += 320;
    if (scrollY <= -180) scrollY += 180;
    DrawTexture(tBackground, (int)scrollX, (int)scrollY, WHITE);
    DrawTexture(tBackground, (int)scrollX + 320, (int)scrollY, WHITE);
    DrawTexture(tBackground, (int)scrollX, (int)scrollY + 180, WHITE);
    DrawTexture(tBackground, (int)scrollX + 320, (int)scrollY + 180, WHITE);
}

void showRequiredControls(Games currentGame){
    // scrap and add to transition elements once done
    if(currentGame == CLIMB_MOUNTAIN) DrawText("USE A & D", 100, 20, 20, GRAY);
    else DrawText("USE MOUSE", 100, 20, 20, GRAY);
}

int main() {
    srand(time(NULL));
    Games currentGame;
    playState pState = PLAYING_STATE;
    double statStartTime = 0;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W * 3, SCREEN_H * 3, "Scalar");
    SetWindowMinSize(SCREEN_W, SCREEN_H);
    SetTargetFPS(60);
    
    Texture2D tBg = LoadTexture("Title.png");
    Texture2D tTexture = LoadTexture("transition.png");
    Texture2D bTTexture = LoadTexture("SnowScene.png");
    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);
    
    while (!WindowShouldClose()) {
        float scale = fminf((float)GetScreenWidth() / SCREEN_W, (float)GetScreenHeight() / SCREEN_H);
        Vector2 mouse = GetMousePosition();
        virtualMouse = (Vector2){ 
            (mouse.x - (GetScreenWidth() - (SCREEN_W * scale)) * 0.5f) / scale,
            (mouse.y - (GetScreenHeight() - (SCREEN_H * scale)) * 0.5f) / scale 
        };

        BeginTextureMode(target);
        ClearBackground(RAYWHITE);

        switch(state) {
            case MENU:
                drawBG(tBg);
                if (DrawHoverButton((SCREEN_W/2)-40, SCREEN_H/2-10, 80, 20, 2, BLACK, GRAY, "START", 10, BLACK, virtualMouse)) state = TRANSITION_START;
                break;

            case TRANSITION_START:
                if (playOpeningTransition(tTexture, bTTexture)) {
                    currentGame = chooseGame();
                    climb = 0; dropped = false; balloonScale = 1.0f;
                    startTime = GetTime();
                    state = PLAYING;
                }
                break;

            case PLAYING:
                showRequiredControls(currentGame);
                pState = playGame(currentGame, virtualMouse);
                if (pState != PLAYING_STATE) state = TRANSITION_END;
                break;

            case TRANSITION_END:
                if(pState == WON_STATE) happyTransition();
                else loseTransition();
                statStartTime = GetTime();
                state = STATS;
                break;

            case STATS:
                drawHearts();
                drawScore();
                if (GetTime() - statStartTime > 2.0) state = TRANSITION_START;
                break;
        }

        EndTextureMode();
        
        BeginDrawing();
            ClearBackground(BLACK);
            Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height };
            Rectangle destRec = { (GetScreenWidth() - (SCREEN_W * scale)) * 0.5f, (GetScreenHeight() - (SCREEN_H * scale)) * 0.5f, SCREEN_W * scale, SCREEN_H * scale };
            DrawTexturePro(target.texture, sourceRec, destRec, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadTexture(tBg); UnloadTexture(tTexture); UnloadTexture(bTTexture);
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}