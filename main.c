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

difficulty currentDifficulty = EASY;
typedef struct{
    Games gameName;
    int easyDifficultyTime;
    int hardDifficultyTime;
    int harderDifficulatyTime;
}difficultyInfo;
difficultyInfo infos[5] = {{CLIMB_MOUNTAIN,5,4,2},{FIT_SHAPE,4,3,2},{DROP_WEIGHT,4,3,2},{GUESS_WEIGHT,6,5,4},{INFLATE_BALOON,5,4,3,}};
int getTimeForDifficulty(Games game, difficulty diff){
    switch(diff){
        case EASY:   return infos[game].easyDifficultyTime;
        case HARD:   return infos[game].hardDifficultyTime;
        case HARDER: return infos[game].harderDifficulatyTime;
    }
}

float balloonScale = 1;
int dColour = 0;
int aColour = 1;
Color aCol = RED;
Color dCol = GREEN;
int climb = 0;
double startTime = 0;

bool inflateBalloon(Vector2 mousePos){
    ClearBackground(RAYWHITE);
    DrawCircle(500,500,50 * balloonScale,BLACK);
    Rectangle TouchRect = {400,400,50,50};
    DrawRectangle(400,400,50,50,RED);

    if(CheckCollisionPointRec(mousePos, TouchRect) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        balloonScale += 0.01;
        printf("Down Scale %f", balloonScale);
    }
    
    return false;
}
bool climbMountain(){
    //0 is the key that should be pressed, 1 is the key that shouldnt
    double maxTime = getTimeForDifficulty(CLIMB_MOUNTAIN,currentDifficulty);
    ClearBackground(RAYWHITE);
    if(aColour == 1 && IsKeyPressed(KEY_A)){
        climb++;
        aColour = 0;
        dColour = 1;
        dCol = GREEN;
        aCol = RED;
    }
    else if(dColour == 1 && IsKeyPressed(KEY_D)){
        climb++;
        aColour = 1;
        dColour = 0;
        dCol = RED;
        aCol = GREEN;
    }
    double elapsed = GetTime() - startTime;
    if(climb >= 30 &&  elapsed > maxTime){
        //draw Happy thing
        //play win sound once
        return true;
    }
    DrawText("A",100,100,30,aCol);
    DrawText("D",100,70,30,dCol);
    DrawText(TextFormat("%d",climb),100,130,30,BLACK);

    DrawText(TextFormat("%.2f", GetTime() - startTime ),100,160,30,BLACK);
    
    return false;
}
typedef struct{
    char *name;
    int weight;
}Guessing;
bool guessWeight(){
    //Type or click? what sort
    return true;
}

char texts[8][16] = {"CLIMB!","FIT!","DROP!","GUESS!","INFLATE!"};
bool playGame(Games currentGame, Vector2 mousePos){
    switch(currentGame){
        case CLIMB_MOUNTAIN:
            return climbMountain();
            // return whether you won the minigame
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
            return inflateBalloon(mousePos);
            DrawText("inflating",100,100,100,BLACK);
            
                // return whether you won the minigame
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
                     Color  fillColor, const char *text,int baseFontSize,Color textColor, Vector2 mousePos){
    static float extraSize = 0;
    static double lastGrow = 0;
    const float WOBBLE_AMPLITUDE = 4.0f;
    const float WOBBLE_SPEED = 5.0f;
    const float MAX_SCALE = 1.1f;

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


bool playOpeningTransition(Texture2D tTexture, Texture2D bTTexture){
    static int i = 0;
    ClearBackground(RAYWHITE);
    DrawTexture(bTTexture,0,0,WHITE);
    DrawTexture(tTexture,(320 / 60) * i,0,WHITE);
    DrawRectangle(SCREEN_W - 8, 0, 8, SCREEN_H, BLACK);
    //show keybaord/mouse, show CLIMB! "%s",texts[currentGame]
    //showRequiredControls()? Just dont remake drawing loop
    i++;
    if (i >= 60) {
        i = 0;
        return true;
    }
    return false;
}
void playClosingTransition(){
    DrawText("CLosing Transition",100,400,100,BLACK);
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
int main() {
    srand(time(NULL));
    Games currentGame;
    bool won = false;
    int startime;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Scalar");
    SetWindowMinSize(SCREEN_W, SCREEN_H);
    SetTargetFPS(60);
    
    Texture2D tBg = LoadTexture("Title.png");
    Texture2D tTexture = LoadTexture("transition.png");
    Texture2D bTTexture = LoadTexture("SnowScene.png");
    
    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);
    
    while (!WindowShouldClose()) {
        
        float scale = fminf((float)GetScreenWidth() / SCREEN_W, (float)GetScreenHeight() / SCREEN_H);
        Vector2 mouse = GetMousePosition();
        Vector2 virtualMouse = {0};
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (SCREEN_W * scale)) * 0.5f) / scale;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (SCREEN_H * scale)) * 0.5f) / scale;

        BeginTextureMode(target);
        ClearBackground(RAYWHITE);

        switch(state) {

            case MENU:
            drawBG(tBg);
            if (DrawHoverButton(150, 150, 40, 40,4,BLACK, GRAY,"START",30,BLACK, virtualMouse)){
                state = TRANSITION_START;
                
            }
  
            break;

            case TRANSITION_START:
                if (playOpeningTransition(tTexture, bTTexture)) {
                    //Combine with show controls and snippet?
                    currentGame = chooseGame();
                    
                    showRequiredControls(currentGame);
                    //showSnippet(currentGame); // CLIMB! etc
                    
                    climb = 0;
                    aColour = 1;
                    dColour = 0;
                    aCol = RED;
                    dCol = GREEN;
                    startTime = GetTime();
                    balloonScale = 1;
                    
                    state = PLAYING;
                }

                break;

            case PLAYING:
                won = playGame(currentGame, virtualMouse);
                if (won) {
                    state = TRANSITION_END;
                }

                break;

            case TRANSITION_END:
                if(won){
                    happyTransition(); // screen
                }else{
                    loseTransition();//cracked screen
                }
                state = STATS;

                break;


            case STATS:
                drawHearts();
                drawScore();
                state = TRANSITION_START;

                break;

        }

        EndTextureMode();
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height };
        Rectangle destRec = { 
            (GetScreenWidth() - ((float)SCREEN_W * scale)) * 0.5f, 
            (GetScreenHeight() - ((float)SCREEN_H * scale)) * 0.5f, 
            (float)SCREEN_W * scale, 
            (float)SCREEN_H * scale 
        };
        
        DrawTexturePro(target.texture, sourceRec, destRec, (Vector2){ 0, 0 }, 0.0f, WHITE);
        
        EndDrawing();
    }

    UnloadTexture(tBg);
    UnloadTexture(tTexture);
    UnloadTexture(bTTexture);
    UnloadRenderTexture(target);
    CloseWindow();
}