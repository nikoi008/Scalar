<<<<<<< Updated upstream
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


difficultyInfo infos[5] = {{CLIMB_MOUNTAIN,5,4,2},{FIT_SHAPE,4,3,2},{DROP_WEIGHT,4,3,2},{GUESS_WEIGHT,6,5,4},{INFLATE_BALOON,5,4,3,}};
Vector2 virtualMouse = {0};
double startTime = 0; // reset at main
difficulty currentDifficulty = EASY;
GameState state = MENU;


int getTimeForDifficulty(Games game, difficulty diff){
    switch(diff){
        case EASY:   return infos[game].easyDifficultyTime;
        case HARD:   return infos[game].hardDifficultyTime;
        case HARDER: return infos[game].harderDifficulatyTime;
    }
    return 5;
}


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


static float balloonScale = 1.0f;
playState inflateBalloon(Vector2 mousePos){
    DrawCircle(SCREEN_W/2, SCREEN_H/2, 20 * balloonScale, RED);
    
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) balloonScale += 0.02f;
    
    if(balloonScale > 3.0f) return WON_STATE;
    if((GetTime() - startTime) > getTimeForDifficulty(INFLATE_BALOON, currentDifficulty)) return LOST_STATE;
    
    return PLAYING_STATE;
}
typedef struct{
    char *name;
    int weight;
}Guessing;

playState guessWeightGame(){
    DrawText("", 100, 100, 20, BLACK);
    if((GetTime() - startTime) > 2.0) return WON_STATE;
    return PLAYING_STATE;
}


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
=======
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include <time.h>
#include "raymath.h"
#include <math.h>

#define SCREEN_W 320
#define SCREEN_H 180

typedef enum {
    EASY,
    HARD,
    HARDER
} difficulty;

typedef enum {
    CLIMB_MOUNTAIN,
    FIT_SHAPE,
    DROP_WEIGHT,
    GUESS_WEIGHT,
    INFLATE_BALOON
} Games;

typedef struct {
    Games gameName;
    int easyDifficultyTime;
    int hardDifficultyTime;
    int harderDifficulatyTime;
} difficultyInfo;

typedef enum {
    PLAYING_STATE,
    LOST_STATE,
    WON_STATE,
} playState;

typedef enum {
    MENU,
    TRANSITION_START,
    PLAYING,
    TRANSITION_END,
    STATS,
    GAME_WIN,
    GAME_OVER
} GameState;

int hearts;
int score;
difficultyInfo infos[5] = {
    {CLIMB_MOUNTAIN, 5, 4, 2},
    {FIT_SHAPE, 6, 4, 2},
    {DROP_WEIGHT, 4, 3, 2},
    {GUESS_WEIGHT, 8, 6, 5},
    {INFLATE_BALOON, 5, 4, 3}
};
Vector2 virtualMouse = {0};
double startTime = 0;
difficulty currentDifficulty = EASY;
GameState state = MENU;

int getTimeForDifficulty(Games game, difficulty diff) {
    switch(diff) {
        case EASY:   return infos[game].easyDifficultyTime;
        case HARD:   return infos[game].hardDifficultyTime;
        case HARDER: return infos[game].harderDifficulatyTime;
    }
    return 5;
}

int dColour = 0;
int aColour = 1;
Color aCol = RED;
Color dCol = GREEN;
int climb = 0;
Texture2D climberFrames[6];
Texture2D keyA[2];
Texture2D keyD[2];

playState climbMountain() {
    double maxTime = getTimeForDifficulty(CLIMB_MOUNTAIN, currentDifficulty);
    double elapsed = GetTime() - startTime;

    if (aColour == 1 && IsKeyPressed(KEY_A)) {
        climb++;
        aColour = 0; dColour = 1;
    } else if (dColour == 1 && IsKeyPressed(KEY_D)) {
        climb++;
        aColour = 1; dColour = 0;
    }

    if (climb >= 20) {
        DrawTexture(climberFrames[5], 0, 0, WHITE);
    }
    
    if (elapsed > maxTime && climb >= 20) return WON_STATE;
    if (elapsed > maxTime) return LOST_STATE;

    int currentFrame = climb / 4;
    if (climb < 20) {
        DrawTexture(climberFrames[currentFrame], 0, 0, WHITE);
    }

    DrawTexture(keyA[aColour], 10, 10, WHITE);
    DrawTexture(keyD[dColour], 30, 10, WHITE);

    float timeBarWidth = (1.0f - (elapsed / maxTime)) * 100;
    DrawRectangle(0, 170, (int)timeBarWidth, 5, GREEN);
    DrawRectangleLines(0, 170, (int)timeBarWidth, 5, BLACK);
    
    return PLAYING_STATE;
}

bool dropped = false;
float dropX = 0;
float dropY = 0;
Texture2D dropFrames[5] = { 0 };
Texture2D weightCursor;

const float FALL_SPEED = 400.0f;
const float IMPACT_DURATION = 0.5f;
static bool isAnimating = false;
static float impactTimer = 0.0f;

playState dropWeight(Vector2 mousePos) {
    float maxTime = getTimeForDifficulty(DROP_WEIGHT, currentDifficulty);
    float elapsedGameTime = GetTime() - startTime;

    if (!dropped && !isAnimating) {
        DrawTexture(dropFrames[0], 0, 0, WHITE);
        
        float drawX = mousePos.x - (weightCursor.width / 2.0f);
        float drawY = mousePos.y - (weightCursor.height / 2.0f);
        DrawTexture(weightCursor, (int)drawX, (int)drawY, WHITE);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (mousePos.x >= 74 && mousePos.x <= 109) {
                dropped = true;
                dropX = mousePos.x;
                dropY = mousePos.y;
            } else {
                return LOST_STATE;
            }
        }
    } else if (dropped && !isAnimating) {
        DrawTexture(dropFrames[0], 0, 0, WHITE);
        dropY += FALL_SPEED * GetFrameTime();

        if (dropY >= 75.0f) {
            dropY = 75.0f;
            dropped = false;
            isAnimating = true; 
            impactTimer = 0.0f; 
        }

        float drawX = dropX - (weightCursor.width / 2.0f);
        float drawY = dropY - (weightCursor.height / 2.0f);
        DrawTexture(weightCursor, (int)drawX, (int)drawY, WHITE);
    } else if (isAnimating) {
        impactTimer += GetFrameTime();
        float progress = impactTimer / IMPACT_DURATION;
        int frameIndex = (int)(progress * 5); 
        
        if (frameIndex > 4) frameIndex = 4;

        DrawTexture(dropFrames[frameIndex], 0, 0, WHITE);
        DrawTexture(weightCursor, 320, 320, WHITE);

        if (progress >= 1.0f && elapsedGameTime - 0.5 > maxTime) return WON_STATE;
    }

    if (elapsedGameTime > maxTime + 1) return LOST_STATE;
    return PLAYING_STATE;
}

static float balloonScale = 1.0f;
static float balloonY = SCREEN_H / 2.0f;
Texture2D tBalloon; 

playState inflateBalloon(Vector2 mousePos) {
    float maxTime = getTimeForDifficulty(INFLATE_BALOON, currentDifficulty);
    float elapsed = GetTime() - startTime;
    ClearBackground(BLUE);
    
    if (balloonScale < 3.0f) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            balloonScale += 0.02f;
        }
        balloonY = SCREEN_H / 2.0f; 
    } else {
        balloonY -= 4.0f;
    }

    float width = tBalloon.width * balloonScale;
    float height = tBalloon.height * balloonScale;

    Rectangle destRec = { SCREEN_W / 2.0f, balloonY, width, height };
    Vector2 origin = { width / 2.0f, height / 2.0f };
    
    DrawTexturePro(tBalloon, 
        (Rectangle){ 0, 0, tBalloon.width, tBalloon.height }, 
        destRec, origin, 0, WHITE);

    if (balloonY + (height / 2.0f) < 0) return WON_STATE;
    if (elapsed > maxTime) return LOST_STATE;

    return PLAYING_STATE;
}

typedef struct {
    char name[16];
    float weight;
    Rectangle rect;
    int linkedTo;
} GuessItem;

GuessItem items[3] = { {"Apple", 0.25f, {80, 40, 60, 30}, -1}, {"Child", 50.0f, {80, 80, 60, 30}, -1}, {"Elephant", 5000.0f, {80, 120, 60, 30}, -1} };
GuessItem weights[3] = { {"250g", 0.25f, {160, 40, 60, 30}, -1}, {"50kg", 50.0f, {160, 80, 60, 30}, -1}, {"5t", 5000.0f, {160, 120, 60, 30}, -1} };
int selectedItem = -1;
Texture2D guessbg;

playState guessWeightGame(Vector2 mousePos) {
    DrawTexture(guessbg, 0, 0, WHITE);
    for (int i = 0; i < 3; i++) {
        if (CheckCollisionPointRec(mousePos, items[i].rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (items[i].linkedTo != -1) {
                weights[items[i].linkedTo].linkedTo = -1;
                items[i].linkedTo = -1;
            }
            selectedItem = i;
        }

        if (CheckCollisionPointRec(mousePos, weights[i].rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (selectedItem != -1) {
                if (weights[i].linkedTo != -1) items[weights[i].linkedTo].linkedTo = -1;
                items[selectedItem].linkedTo = i;
                weights[i].linkedTo = selectedItem;
                selectedItem = -1; 
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        Color itemCol = (selectedItem == i) ? YELLOW : LIGHTGRAY;
        DrawRectangleRec(items[i].rect, itemCol);
        DrawRectangleLinesEx(items[i].rect, 2, BLACK);
        DrawText(items[i].name, items[i].rect.x + 5, items[i].rect.y + 10, 10, BLACK);

        DrawRectangleRec(weights[i].rect, LIGHTGRAY);
        DrawRectangleLinesEx(weights[i].rect, 2, BLACK);
        DrawText(weights[i].name, weights[i].rect.x + 5, weights[i].rect.y + 10, 10, BLACK);

        if (items[i].linkedTo != -1) {
            Vector2 start = { items[i].rect.x + items[i].rect.width, items[i].rect.y + items[i].rect.height/2 };
            Vector2 end = { weights[items[i].linkedTo].rect.x, weights[items[i].linkedTo].rect.y + weights[items[i].linkedTo].rect.height/2 };
            DrawLineEx(start, end, 3, BLUE);
        }
    }

    int correctMatches = 0;
    int totalLinks = 0;
    for (int i = 0; i < 3; i++) {
        if (items[i].linkedTo != -1) {
            totalLinks++;
            if (items[i].weight == weights[items[i].linkedTo].weight) {
                correctMatches++;
            }
        }
    }
    
    double elapsed = GetTime() - startTime;
    if (correctMatches == 3 && elapsed > getTimeForDifficulty(GUESS_WEIGHT, currentDifficulty)) return WON_STATE;
    if (elapsed > getTimeForDifficulty(GUESS_WEIGHT, currentDifficulty)) return LOST_STATE;

    return PLAYING_STATE;
}

double coinSize = 60.0f;
bool isDragging = false;
Texture2D tPiggy;
Texture2D tCoin;
Texture2D piggyBank;

Vector2 coinPos = { 160, 30 };
bool isFalling = false;

playState fitShape(Vector2 mousePos) {
    Vector2 targetPos = { 153, 132 }; 
    float fallSpeed = 300.0f; 

    DrawTexture(tPiggy, 0, 0, WHITE);

    if (!isFalling) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) isDragging = true;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) isDragging = false;

        if (isDragging) {
            float dx = mousePos.x - coinPos.x;
            float dy = mousePos.y - coinPos.y;
            float distance = sqrtf(dx * dx + dy * dy);
            coinSize = distance * 2.0f;

            if (coinSize > 150.0f) coinSize = 150.0f;
            if (coinSize < 10.0f)  coinSize = 10.0f;
        }

        if (coinSize <= 30.0f && !isDragging) {
            isFalling = true;
        }
    } else {
        if (coinPos.y < targetPos.y) {
            coinPos.y += fallSpeed * GetFrameTime();
        }
        
        if (coinPos.x < targetPos.x) coinPos.x += 1;
        if (coinPos.x > targetPos.x) coinPos.x -= 1;

        if (coinPos.y >= targetPos.y && (GetTime() - startTime) + 1.5 > getTimeForDifficulty(FIT_SHAPE, currentDifficulty)) return WON_STATE;
    }

    Rectangle coinRect = {
        coinPos.x - (coinSize / 2.0f),
        coinPos.y - (coinSize / 2.0f),
        coinSize,
        coinSize
    };

    DrawTexturePro(tCoin, (Rectangle){0, 0, tCoin.width, tCoin.height}, coinRect, (Vector2){0, 0}, 0, WHITE);
    DrawTexture(piggyBank, 96, 90, WHITE); 

    if ((GetTime() - startTime) > getTimeForDifficulty(FIT_SHAPE, currentDifficulty)) return LOST_STATE;

    return PLAYING_STATE;
}

playState playGame(Games currentGame, Vector2 mousePos) {
    switch(currentGame) {
        case CLIMB_MOUNTAIN: return climbMountain();
        case DROP_WEIGHT:    return dropWeight(mousePos);
        case INFLATE_BALOON: return inflateBalloon(mousePos);
        case GUESS_WEIGHT:   return guessWeightGame(mousePos);
        case FIT_SHAPE:      return fitShape(mousePos);
        default:             return LOST_STATE;  
    }
}

bool DrawHoverButton(float x, float y, float width, float height, float outlineSize, Color outlineColor,
    Color fillColor, const char *text, int baseFontSize, Color textColor, Vector2 mousePos) {
    
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
    if (hovered) {
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

Games gameChoices[5] = { CLIMB_MOUNTAIN, FIT_SHAPE, DROP_WEIGHT, GUESS_WEIGHT, INFLATE_BALOON };
int choiceIndex = 5; 
Games lastGamePlayed = -1;

Games chooseGame() {
    if (choiceIndex >= 5) {
        for (int i = 0; i < 5; i++) {
            int target = rand() % 5;
            Games temp = gameChoices[i];
            gameChoices[i] = gameChoices[target];
            gameChoices[target] = temp;
        }
        choiceIndex = 0;

        if (gameChoices[0] == lastGamePlayed) {
            Games temp = gameChoices[0];
            gameChoices[0] = gameChoices[1];
            gameChoices[1] = temp;
        }
    }

    Games picked = gameChoices[choiceIndex];
    choiceIndex++;
    lastGamePlayed = picked;
    
    return picked;
}

Texture2D tKeyboard;
Texture2D tMouse;

bool playOpeningTransition(Texture2D tTexture, Texture2D bTTexture, Games currentGame) {
    static int i = 0;
    int frames = 60;
    switch(currentGame){
        case CLIMB_MOUNTAIN: DrawTexture(climberFrames[0], 0, 0, WHITE); break;
        case FIT_SHAPE:      DrawTexture(tPiggy, 0, 0, WHITE); break;
        case GUESS_WEIGHT: DrawTexture(guessbg,0,0,WHITE); break;
        case INFLATE_BALOON: DrawRectangle(0,0,320,320,BLUE); break;
        case DROP_WEIGHT: DrawTexture(dropFrames[0],0,0,WHITE); break;
    }
    DrawTexture(bTTexture, 0, 0, WHITE);
    DrawTexture(tTexture, (SCREEN_W / frames) * i, 0, WHITE);
    
    if (currentGame == CLIMB_MOUNTAIN) {
        DrawTexture(tKeyboard, ((SCREEN_W / frames) * i) + 97, 109, WHITE);
    } else {
        DrawTexture(tKeyboard, ((SCREEN_W / frames) * i) + 147, 111, WHITE);
    }
    
    i++;
    if (i >= frames) { i = 0; return true; }
    return false;
}

void drawLivesandScore(Texture2D tTexture) {
    DrawTexture(tTexture, 0, 0, WHITE);
    DrawText(TextFormat("Life: %d", hearts), 10, 10, 20, MAROON);
    DrawText(TextFormat("Score: %d", score), 10, 40, 20, BLACK);
}

Texture2D tWin;
Texture2D tLose;

bool playClosingTransition(Texture2D transitionTexture) {
    static int i = 60;          
    static float waitTimer = 0; 
    int frames = 60;
    float posX = (SCREEN_W / (float)frames) * i;

    if (i > 0) {
        DrawTexture(transitionTexture, (int)posX, 0, WHITE);
        i--;
        return false;
    } else {
        DrawTexture(transitionTexture, 0, 0, WHITE); 
        waitTimer += GetFrameTime();
        
        if (waitTimer >= 1.0f) {
            i = 60;
            waitTimer = 0;
            return true;
        }
    }
    return false;
}

float scrollX;
float scrollY;
float speed = 40.0f;

void drawBG(Texture2D tBackground) {
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

void drawDifficultyUp(Texture2D tTexture) {
    DrawTexture(tTexture, 0, 0, WHITE);
    const char* text = "DIFFICULTY UP!";
    int fontSize = 25;
    int textWidth = MeasureText(text, fontSize);
    int xPos = (SCREEN_W / 2) - (textWidth / 2);
    int yPos = (SCREEN_H / 2) - (fontSize / 2);

    DrawText(text, xPos + 2, yPos + 2, fontSize, BLACK);
    DrawText(text, xPos, yPos, fontSize, YELLOW);
}

int main() {
    srand(time(NULL));
    Games currentGame = chooseGame(); 
    playState pState = PLAYING_STATE;
    double statStartTime = 0;
    bool soundPlayed = false;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W * 3, SCREEN_H * 3, "Scalar");
    SetWindowMinSize(SCREEN_W, SCREEN_H);
    InitAudioDevice();
    SetTargetFPS(60);

    char path[64];
    for (int i = 0; i < 5; i++) {
        sprintf(path, "sprites/ClimbMountain/frame%d.png", i + 1);
        climberFrames[i] = LoadTexture(path);
    }
    climberFrames[5] = LoadTexture("sprites/ClimbMountain/frame6.png");

    for(int i = 0; i < 5; i++) {
        sprintf(path, "sprites/dropWeight/frame%d.png", i + 1);
        dropFrames[i] = LoadTexture(path);
    }
    
    tBalloon = LoadTexture("sprites/InflateBalloon/balloon.png");
    weightCursor = LoadTexture("sprites/dropWeight/cursor.png");
    keyA[0] = LoadTexture("sprites/ClimbMountain/unpressed_a.png");
    keyA[1] = LoadTexture("sprites/ClimbMountain/click_a.png");
    keyD[0] = LoadTexture("sprites/ClimbMountain/unpressed_d.png");
    keyD[1] = LoadTexture("sprites/ClimbMountain/click_d.png");
    guessbg = LoadTexture("sprites/guessWeight/bg.png");
    Texture2D tBg = LoadTexture("Menu/Title.png");
    Texture2D tTexture = LoadTexture("Menu/transition.png");
    Texture2D bTTexture = LoadTexture("SnowScene.png");
    tPiggy = LoadTexture("sprites/fitShape/piggybank.png");
    tCoin = LoadTexture("sprites/fitShape/coin.png");   
    Texture2D tTitleText = LoadTexture("Menu/TitleText.png"); 
    tKeyboard = LoadTexture("Menu/keyboard.png");
    tMouse = LoadTexture("Menu/mouse.png");
    tWin = LoadTexture("Menu/happyTransition.png");
    tLose = LoadTexture("Menu/loseTransition.png");
    piggyBank = LoadTexture("sprites/fitShape/pig.png");

    Sound sClimb = LoadSound("Menu/Climb.mp3");
    Sound sPiggy = LoadSound("Menu/PiggyBank.mp3");
    Sound sDrop = LoadSound("Menu/dropwiehgt.mp3"); 
    Sound sGuess = LoadSound("Menu/Guesswight.mp3"); 
    Sound sInflate = LoadSound("Menu/inflate.mp3");
    Sound sWin = LoadSound("Menu/Win.mp3");
    Sound sLose = LoadSound("Menu/Lose.mp3");
    Sound sMenu = LoadSound("Menu/MAIN MENU.mp3");
    Sound Next = LoadSound("Menu/next.mp3");
    Sound sDifficultyUp = LoadSound("Menu/dup.mp3");
    
    float diffUpTimer = 0.0f;
    bool showingDiffUp = false;
    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);
    PlaySound(sMenu);

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
                hearts = 4;
                score = 0;
                drawBG(tBg);
                if (!IsSoundPlaying(sMenu)) PlaySound(sMenu);

                float time = GetTime();
                float bounceY = fabsf(sinf(time * 3.0f)) * -10.0f; 
                float pulse = 1.0f + (fabsf(sinf(time * 3.0f)) * 0.05f);
                float posX = (SCREEN_W / 2) - (tTitleText.width * pulse / 2);
                float posY = (SCREEN_H / 4) + bounceY; 

                DrawTextureEx(tTitleText, (Vector2){ posX + 20, posY }, 0.0f, pulse, WHITE);

                if (DrawHoverButton((SCREEN_W/2)-40, SCREEN_H/2+20, 80, 20, 2, BLACK, GRAY, "START", 10, BLACK, virtualMouse)) {
                    state = TRANSITION_START;
                    StopSound(sMenu);
                }
                break;

            case TRANSITION_START:
                if (playOpeningTransition(tTexture, bTTexture, currentGame)) {
                    climb = 0; 
                    aColour = 1;
                    dColour = 0;
                    balloonScale = 1.0f; 
                    coinSize = 60.0f;
                    coinPos = (Vector2){160, 30};
                    isFalling = false;
                    isDragging = false; 
                    dropped = false; 
                    isAnimating = false;
                    impactTimer = 0.0f; 
                    dropY = 0;
                    dropX = 0;
                    balloonY = SCREEN_H / 2.0f;

                    float yPos[3] = { 40, 80, 120 };
                    for (int i = 0; i < 3; i++) {
                        int targetIdx = rand() % 3;
                        float temp = yPos[i];
                        yPos[i] = yPos[targetIdx];
                        yPos[targetIdx] = temp;
                    }
                    for (int i = 0; i < 3; i++) items[i].rect.y = yPos[i];
                    selectedItem = -1;
                    for(int i = 0; i < 3; i++) { items[i].linkedTo = -1; weights[i].linkedTo = -1; }

                    switch(currentGame) {
                        case CLIMB_MOUNTAIN: PlaySound(sClimb);   break;
                        case FIT_SHAPE:      PlaySound(sPiggy);   break;
                        case DROP_WEIGHT:    PlaySound(sDrop);    break;
                        case GUESS_WEIGHT:   PlaySound(sGuess);   break;
                        case INFLATE_BALOON: PlaySound(sInflate); break;
                    }

                    startTime = GetTime();
                    state = PLAYING;
                }
                break;

            case PLAYING:
                pState = playGame(currentGame, virtualMouse);
                if (pState != PLAYING_STATE) {
                    state = TRANSITION_END;
                    soundPlayed = false; 
                    switch(currentGame) {
                        case CLIMB_MOUNTAIN: StopSound(sClimb);   break;
                        case FIT_SHAPE:      StopSound(sPiggy);   break;
                        case DROP_WEIGHT:    StopSound(sDrop);    break;
                        case GUESS_WEIGHT:   StopSound(sGuess);   break;
                        case INFLATE_BALOON: StopSound(sInflate); break;
                    }
                }
                break;

            case TRANSITION_END:
                if (!soundPlayed) {
                    if (pState == WON_STATE) { PlaySound(sWin); score++; }
                    else { PlaySound(sLose); hearts--; }
                    soundPlayed = true; 
                }

                Texture2D resTex = (pState == WON_STATE) ? tWin : tLose;
                if (playClosingTransition(resTex)) {
                    statStartTime = GetTime();
                    state = STATS;
                    soundPlayed = false;
                }
                break;

            case STATS:
                if (!soundPlayed) {
                    PlaySound(Next);
                    soundPlayed = true; 

                    if (!showingDiffUp) {
                        if (score == 10 || score == 15) {
                            currentDifficulty++;
                            diffUpTimer = 4.0f;
                            showingDiffUp = true;
                            PlaySound(sDifficultyUp); 
                        }
                    }
                    if (score == 20) {
                        state = GAME_WIN;
                    }
                }

                drawLivesandScore(tTexture);

                if (showingDiffUp && diffUpTimer > 0) {
                    drawDifficultyUp(tTexture);
                    diffUpTimer -= GetFrameTime();
                }
                
                if (GetTime() - statStartTime > 1.5 && diffUpTimer <= 0) {
                    showingDiffUp = false; 
                    if (hearts <= 0) {
                        state = GAME_OVER; 
                        soundPlayed = false;
                    } else {
                        currentGame = chooseGame();
                        state = TRANSITION_START;
                        soundPlayed = false;
                    }
                }
                break;

            case GAME_WIN:
                DrawTexture(tTexture, 0, 0, WHITE); 
                const char* winText = "YOU WIN!";
                DrawText(winText, SCREEN_W/2 - MeasureText(winText, 20)/2, 40, 20, GOLD);
                DrawText(TextFormat("Final Score: %d", score), SCREEN_W/2 - MeasureText(TextFormat("Final Score: %d", score), 10)/2, 70, 10, BLACK);

                if (DrawHoverButton(SCREEN_W/2 - 50, 100, 100, 20, 2, BLACK, LIGHTGRAY, "MAIN MENU", 10, BLACK, virtualMouse)) {
                    state = MENU;
                    score = 0; 
                    hearts = 4;
                    currentDifficulty = EASY;
                }

                if (DrawHoverButton(SCREEN_W/2 - 50, 130, 100, 20, 2, BLACK, LIGHTGRAY, "PLAY AGAIN", 10, BLACK, virtualMouse)) {
                    score = 0;
                    hearts = 4;
                    currentDifficulty = EASY;
                    currentGame = chooseGame();
                    state = TRANSITION_START;
                }
                break;

            case GAME_OVER:
                DrawTexture(tTexture, 0, 0, WHITE);
                const char* loseText = "GAME OVER";
                DrawText(loseText, SCREEN_W/2 - MeasureText(loseText, 30)/2, 40, 30, RED);
                DrawText(TextFormat("Final Score: %d", score), SCREEN_W/2 - MeasureText(TextFormat("Final Score: %d", score), 10)/2, 80, 10, DARKGRAY);

                if (DrawHoverButton(SCREEN_W/2 - 50, 110, 100, 20, 2, BLACK, LIGHTGRAY, "MENU", 10, BLACK, virtualMouse)) {
                    state = MENU;
                }

                if (DrawHoverButton(SCREEN_W/2 - 50, 140, 100, 20, 2, BLACK, LIGHTGRAY, "RETRY", 10, BLACK, virtualMouse)) {
                    score = 0;
                    hearts = 4;
                    currentDifficulty = EASY;
                    currentGame = chooseGame();
                    state = TRANSITION_START;
                }
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

    UnloadTexture(tBg); 
    UnloadTexture(tTexture); 
    UnloadTexture(bTTexture);
    UnloadTexture(tPiggy); 
    UnloadTexture(tCoin); 
    UnloadTexture(tTitleText);
    UnloadTexture(tKeyboard); 
    UnloadTexture(tMouse); 
    UnloadTexture(tWin); 
    UnloadTexture(tLose);
    UnloadTexture(piggyBank);
    UnloadTexture(tBalloon);
    UnloadTexture(guessbg);
    UnloadTexture(weightCursor);
    UnloadTexture(keyA[0]); UnloadTexture(keyA[1]);
    UnloadTexture(keyD[0]); UnloadTexture(keyD[1]);

    for (int i = 0; i < 6; i++) UnloadTexture(climberFrames[i]);
    for (int i = 0; i < 5; i++) UnloadTexture(dropFrames[i]);

    UnloadSound(sClimb); UnloadSound(sPiggy); UnloadSound(sDrop);
    UnloadSound(sGuess); UnloadSound(sInflate); UnloadSound(sWin);
    UnloadSound(sLose); UnloadSound(sMenu); UnloadSound(Next);
    UnloadSound(sDifficultyUp);

    UnloadRenderTexture(target);
    CloseAudioDevice();
    CloseWindow();

    return 0;
>>>>>>> Stashed changes
}