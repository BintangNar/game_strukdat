#include "raylib.h"
#include "raymath.h"
#include "string"
#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>

/*------------------------------------class&stuff------------------------------------------------*/

class Bullet {
public:
    float positionX;
    float positionY;
    float speedX;
    float speedY;
    float radius;
    bool active;

    Bullet(float x, float y, float targetX, float targetY, float speed) :
        positionX(x), positionY(y), radius(5.0f), active(true) {
        float directionX = targetX - x;
        float directionY = targetY - y;
        float length = sqrt(directionX * directionX + directionY * directionY);

        speedX = (directionX / length) * speed;
        speedY = (directionY / length) * speed;
    }

    void update(float deltaTime) {
        if (active) {
            positionX += speedX * deltaTime;
            positionY += speedY * deltaTime;

            if (positionX < 0 || positionX > 1600 || positionY < 0 || positionY > 850) {
                active = false;
            }
        }
    }

    void draw() const {
        if (active) {
            DrawCircle(static_cast<int>(positionX), static_cast<int>(positionY), radius, BLACK);
        }
    }
};

class player {
public:
    int health;
    int attack;
    int score;
    float positionX;
    float positionY;
    float speedX;
    float speedY;
    bool invisible = false;
    float iframelimit = 2.0f;
    float iframetime = 0.0f;
    float iframecd = 3.0f;
    float iframecdtime = 0.0f;
    std::vector<Bullet> bullets;
    Texture2D texture;

    player(int health, int atk, int score, float speedX, float speedY) :
        health(health), attack(atk), score(score),
        speedX(speedX), speedY(speedY), positionX(500), positionY(425) {}

    void Hmovement(float deltaTime) {
        if (IsKeyDown(KEY_D)) {
            positionX += speedX * deltaTime;
        } else if (IsKeyDown(KEY_A)) {
            positionX -= speedX * deltaTime;
        }
    }

    void Vmovement(float deltaTime) {
        if (IsKeyDown(KEY_S)) {
            positionY += speedY * deltaTime;
        } else if (IsKeyDown(KEY_W)) {
            positionY -= speedY * deltaTime;
        }
    }

    void shoot(float targetX, float targetY){
        float bulletSpeed = 250.0f;
        bullets.push_back(Bullet(positionX+20.0f, positionY+20.0f, targetX, targetY, bulletSpeed));
    }

    void updateBullets(float deltaTime){
        for(auto& bullet : bullets){
            bullet.update(deltaTime);
        }
    }

    void iframe(float deltaTime){
        if(invisible){
            iframetime += deltaTime;
            if(iframetime >= iframelimit){
                invisible = false;
                iframetime = 0.0f;
                iframecdtime = 0.0f;
            }
        }

        if(!invisible){
            iframecdtime += deltaTime;
            if (iframecdtime >= iframecd){
                iframecdtime = iframecd;
            }
        }
    }

    void setTexture(Texture2D newtexture){
        texture = newtexture;
    }

    void draw() const {
        if (!invisible || (static_cast<int>(iframetime * 10) % 2 == 0)) {
        DrawTexture(texture,static_cast<int>(positionX),static_cast<int>(positionY),WHITE);
        }
        for(const auto& bullet : bullets){
            bullet.draw();
        }
    }

    void drawiframe() const {
        if(iframecdtime < iframecd){
        float cooldownPercentage = iframecdtime / iframecd;
        DrawRectangle(static_cast<int>(positionX), static_cast<int>(positionY) - 10, static_cast<int>(40 * cooldownPercentage), 5, BLUE);
        }
    }

    void healthupdate(int damage){  
        if(!invisible && iframecdtime >= iframecd){
            health -= damage;
            invisible = true;
        if(health < 0){
            health = 0;
            }
        }    
    }
    
    float GetX() const { return positionX; }
    float GetY() const { return positionY; }
};

class enemy {
public:
    float positionX;
    float positionY;
    int health;
    int attack;
    int scorevalue;
    bool alive;
    float width = 30.0f;
    float height = 30.0f;
    Texture2D texture;
    Color color;

    enemy(int enemyid, int health, int atk, int scorevalue, float speedX, float speedY, bool alive, Color color) :
        enemyid(enemyid),
        health(health),
        attack(atk),
        scorevalue(scorevalue),
        speedX(speedX),
        speedY(speedY),
        alive(true),
        positionX(0),
        positionY(0),
        color(color) {}

    void draw() const {
        DrawTexture(texture,static_cast<int>(positionX), static_cast<int>(positionY), color);
    }

    void playerChase(player& player, float deltaTime) {
        float playerX = player.GetX();
        float playerY = player.GetY();
        float directionX = playerX - positionX;
        float directionY = playerY - positionY;

        float distance = sqrt(directionX * directionX + directionY * directionY);
        if (distance < 1.0f) distance = 1.0f;

        directionX = directionX / distance;
        directionY = directionY / distance;
        if(distance < 40.0f){
            player.healthupdate(attack);
        } else {
            positionX = positionX + (directionX * speedX * deltaTime);
            positionY = positionY + (directionY * speedY * deltaTime);
        }
    }

    void spawnposition(int screenWidth, int screenHeight) {
        int spawnpoint = GetRandomValue(0, 3);
        switch (spawnpoint) {
        case 0:
            positionX = GetRandomValue(100, 1000);
            positionY = 10;
            break;
        case 1:
            positionX = GetRandomValue(100, 900);
            positionY = 800;
            break;
        case 2:
            positionX = 1000;
            positionY = GetRandomValue(100, 800);
            break;
        case 3:
            positionX = 100;
            positionY = GetRandomValue(100, 800);
            break;
        }
    }
    
    bool checkCollision(const Bullet& bullet, const enemy& enemy) {
        float closestX = fmaxf(enemy.positionX, fminf(bullet.positionX, enemy.positionX + enemy.width));
        float closestY = fmaxf(enemy.positionY, fminf(bullet.positionY, enemy.positionY + enemy.height));

        float distanceX = bullet.positionX - closestX;
        float distanceY = bullet.positionY - closestY;
    
        return (distanceX * distanceX + distanceY * distanceY) < (bullet.radius * bullet.radius);
    }



    void takedamage(int damage){
        health -= damage;
        if (health <= 0){
            alive = false;
        }
    }

private:
    float speedX;
    float speedY;
    int enemyid;
};

struct button
{
    Rectangle bounds;
    std::string text;
    Color color;
    Color textColor;
    bool hovered;
    bool clicked;

    button(Rectangle bounds, std::string text, Color color, Color textColor)
        : bounds(bounds), text(text), color(color), textColor(textColor), hovered(false), clicked(false) {}

    void Update() {
        hovered = CheckCollisionPointRec(GetMousePosition(), bounds);

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            clicked = true;
        } else {
            clicked = false;
        }
    }

    void Draw() const {
        // Draw button rectangle
        DrawRectangleRec(bounds, hovered ? DARKGRAY : color);

        // Draw button text
        DrawText(text.c_str(), bounds.x + 10, bounds.y + bounds.height/2 - 10, 20, textColor);
    }
};


/*------------------------------------main function------------------------------------------------*/

//game states
enum GameState{
    main_menu,
    Game,
    End_win,
    End_lose
};

void updatePlayerTexture(player& player, Texture2D playerRight, Texture2D playerLeft) {
    if (GetMousePosition().x > player.positionX) {
        player.setTexture(playerRight);
    } else {
        player.setTexture(playerLeft);
    }
}

int main(void) {
    /*------------------------------------initiation--------------------------------------------------*/
    InitWindow(1600, 850, "gamename");
    SetTargetFPS(60);
    int frameCounter = 0;
    srand(time(nullptr));

    Texture2D playerright;
    Texture2D playerleft;
    Texture2D enemytexture;
    
    playerright = LoadTexture("resource/playersprite.png");
    playerleft = LoadTexture("resource/playersprite_left.png");
    enemytexture = LoadTexture("resource/enemysprite.png");

    GameState gameState = main_menu;
    player player(100, 10, 0, 190.0f, 190.0f);    
    
    player.setTexture(playerleft);
    
    std::vector<enemy> enemies;
    std::vector<Bullet> bullet;
    float spawntimer = 3.0f;
    float timeSinceLastSpawn = 0.0f;
    float timercount = 0.0f;

    float atkInterv = 0.0f;
    float intervCheck = 1.2f;
 
    int timeMin = 0;
    int spawnlimit;
    int enemyNumber = 0;

   

    Color ballcolor = BLACK;

    button startButton({500,300,250,100},"Start",DARKBLUE,WHITE);
    button exitButton({500,500,250,100},"exit",RED,WHITE);
    button restartButton({300,300,150,50},"replay",DARKBLUE,WHITE);
    button HomeButton({500,300,150,50},"Main Menu",DARKBLUE,WHITE);
    button exitButton2({400,370,150,50},"exit",RED,WHITE);


    /*-----------------------------------------------------------------------------------------------*/

    /*------------------------------------game loop---------------------------------------------------*/
    while (!WindowShouldClose()) {
        // in-game updates
        float deltaTime = GetFrameTime();
        startButton.Update();
        exitButton.Update();
        restartButton.Update();
        HomeButton.Update();
        exitButton2.Update();
    //main menu
    switch (gameState){
        case main_menu: {
            enemies.clear();
            timercount = 0.0f;
            player.score = 0;
            player.health = 100;
            if(startButton.clicked){
                gameState = Game;
                
            } else if(exitButton.clicked){
                CloseWindow();
            }
            break;
        }
        //game
        case Game:{  
        updatePlayerTexture(player,playerright,playerleft);
        timeSinceLastSpawn += deltaTime;
        timercount += deltaTime;

        if (timercount >= 50.0f) {
            spawnlimit = 9;
        } else if (timercount >= 30.0f) {
            spawnlimit = 7;
        } else {
            spawnlimit = 5;
        }

        //enemy spawn
        if (enemyNumber < spawnlimit) {
            if (timeSinceLastSpawn >= spawntimer) {
                timeSinceLastSpawn = 0.0f;
                int spawnID = GetRandomValue(1, 4);

                if (timercount >= 60.0f) {
                    spawnID = GetRandomValue(1, 4);
                } else if (timercount >= 30.0f) {
                    spawnID = GetRandomValue(1, 2);
                } else {
                    spawnID = 1;
                }

                switch (spawnID) {
                case 1:
                    enemies.push_back(enemy(1, 10, 10, 100, 100.0f, 100.0f, true, GREEN));
                    enemyNumber++;
                    break;
                case 2:
                    enemies.push_back(enemy(2, 40, 10, 300, 90.0f, 90.0f, true, BLUE));
                    enemyNumber++;
                    break;
                case 3:
                    enemies.push_back(enemy(3, 30, 40, 400, 100.0f, 100.0f, true, RED));
                    enemyNumber++;
                    break;
                case 4:
                    enemies.push_back(enemy(4, 10, 10, 300, 120.0f, 120.0f, true, VIOLET));    
                }
                for (auto& e:enemies){
                    e.texture = enemytexture;
                enemies.back().spawnposition(1100, 850);
                }
            }
        }

        //player movement
        player.Hmovement(deltaTime);
        player.Vmovement(deltaTime);
        if (player.positionX > 1090) {
            player.positionX = 100;
        } else if (player.positionX < 0) {
            player.positionX = 1000;
        }

        if (player.positionY < 0) {
            player.positionY = 800;
        } else if (player.positionY > 850) {
            player.positionY = 100;
        }

        player.iframe(deltaTime);
        player.drawiframe();

        //player attacking ig
        atkInterv += deltaTime;
        if (atkInterv > intervCheck) {
            atkInterv = intervCheck;
        }

            if (atkInterv >= intervCheck && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePosition = GetMousePosition();
            player.shoot(mousePosition.x, mousePosition.y);
            atkInterv = 0.0f;
             }

            player.updateBullets(deltaTime);

            for(auto& enemy : enemies){
                for(auto& bullet : player.bullets){
                    if (bullet.active && enemy.checkCollision(bullet,enemy)) {
                        enemy.takedamage(player.attack);
                        bullet.active = false;
                    }
                }
            }

            if(fmod(timercount,20.0f) < deltaTime){
            player.score += (player.health * 0.1);
            }

            enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [&](const enemy& e) {
            if (e.health <= 0) {
                enemyNumber--;
                player.score += e.scorevalue;
                return true;
            }
            return false;
            }), enemies.end());

            //cheat for testing
            if(IsKeyPressed(KEY_RIGHT_SHIFT)){
                player.health= player.health-90;
            }
            if(IsKeyPressed(KEY_BACKSPACE)){
                timercount += 170;
            }
            if(IsKeyPressed(KEY_EQUAL)){
                player.score += 100;
            }
            if(IsKeyPressed(KEY_ZERO)){
                timercount += 20;
            }
            //
            
            if (player.health <= 0){
            gameState = End_lose;
            }
            if (timercount >= 180){
            gameState = End_win;
             }
             break;
        }
        case End_lose:{
            if(HomeButton.clicked){
                gameState = main_menu;
            } else if(restartButton.clicked){
                timercount = 0.0f;
                player.score = 0;
                player.health = 100;
                enemies.clear();
                gameState = Game;
            } else if (exitButton2.clicked){
                CloseWindow();
            }
            break;
        }
         case End_win:{
            if(HomeButton.clicked){
                gameState = main_menu;
            } else if(restartButton.clicked){
                timercount = 0.0f;
                player.score = 0;
                player.health = 100;
                enemies.clear();
                gameState = Game;
            } else if (exitButton2.clicked){
                CloseWindow();
            }
            break;
        }
    }
        // Draw everything
        BeginDrawing();
        ClearBackground(RAYWHITE);
        switch(gameState){
    
        case main_menu:
            DrawText("Shoot they're here", 230, 150, 80, GREEN);
            startButton.Draw();
            exitButton.Draw();
            break;
        case Game:
            player.draw();    
            for (auto& enemy : enemies){
                enemy.playerChase(player, deltaTime);
                enemy.draw();
            }
            break;
        case End_win:
            DrawText("YOU WIN",300, 150,80,GOLD);
            DrawText("Your Score: ",250,240,50,RED);
            DrawText(TextFormat("%i",player.score),580,240,60,GOLD);
            HomeButton.Draw();
            restartButton.Draw();
            exitButton2.Draw();
            break;
        case End_lose:
            DrawText("YOU LOSE",300, 150,80,PURPLE);
            DrawText("Your Score: ",250,240,50,RED);
            DrawText(TextFormat("%i",player.score),580,240,60,GOLD);
            HomeButton.Draw();
            restartButton.Draw();
            exitButton2.Draw();
            break;
        }
        // Side menu
        DrawRectangle(1100, 0, 500, 850, BLACK);
        DrawText("Time", 1150, MeasureText("test", 30)/2, 30, PINK);
        DrawText(TextFormat("%03.0f", timercount), 1150, MeasureText("40.00.00", 50)/2-20, 50, YELLOW);
        DrawText("Score", 1150, MeasureText("score", 30)/2+100, 30, PINK);
        DrawText(TextFormat("%i", player.score), 1150, MeasureText("000000", 50)/2+110, 50, YELLOW);
        DrawText("Health", 1150, MeasureText("100", 30)/2+220, 30, PINK);
        DrawText(TextFormat("%d", player.health), 1150, MeasureText("100", 50)/2+240, 50, YELLOW);

        DrawText("Enemies: ", 1150, MeasureText("100", 30)/2+350, 30, PINK);
        DrawText("Green = Basic ", 1150, MeasureText("100", 30)/2+400, 30, GREEN);
        DrawText("Blue = Higher Health ", 1150, MeasureText("100", 30)/2+450, 30, BLUE);
        DrawText("Red = Higher Damage ", 1150, MeasureText("100", 30)/2+500, 30, RED);
        DrawText("Violet = Higher Speed ", 1150, MeasureText("100", 30)/2+550, 30, VIOLET);

        EndDrawing();
    }
    UnloadTexture(playerright);
    UnloadTexture(playerleft);
    UnloadTexture(enemytexture);

    CloseWindow();
    return 0;
}
