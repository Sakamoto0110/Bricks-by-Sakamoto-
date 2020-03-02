#include <iostream>
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <time.h>
#include <conio.h>

#define ISRUNNING 1

#define boundsWidth 30
#define boundsHeight 20
#define xoffset 2
#define yoffset 1

#define cWidth 320
#define cHeight 500


enum Color
{
    Blue = 1,
    Green,
    Aqua,
    Red,
    Purple,
    Yellow,
    White,
    Gray,
    LightBlue,
    Black,
    LightGreen,
    LightAqua,
    LightRed,
    LightPurple,
    LightYellow,
    BrightWhite
};


using namespace std;
void gotoxy(int x, int y);
void ShowConsoleCursor(bool showFlag);
void drawAt(char code, int x, int y, int width, int height, int color);
void vanishAt(int x, int y, int width, int height);


class Wall
{
private:
    int x;
    int y;
    int width;
    int height;
    char code = '#';
    bool hasUpdate = true;
public:

    Wall(int _x, int _y, int _width, int _height)
    {
        x = _x;
        y = _y;
        width = _width;
        height = _height;

    }

    void update()
    {

    }

    void render()
    {
        if(hasUpdate){
            drawAt(code,x,y,width,height, White);
            hasUpdate = false;
        }
    }
};




class Entity
{
protected:
    float x,y;
    float dx = 0,dy = 0;
    float oldx,oldy;
    int width;
    int height;


public:
    char code;
    bool hasUpdate = true;
    float setX(float _x){ x = _x;}
    float setY(float _y){ y = _y;}
    float getX(){return x;}
    float getY(){return y;}
    float getWidth(){return width;}
    float getHeight(){return height;}
    float getDx(){return dx;}
    float getDy(){return dy;}
    Entity(){}
    Entity(char _code, float _x, float _y, int _width, int _height)
    {
        code = _code;
        x = _x;
        y = _y;
        width = _width;
        height = _height;
    }

    bool collide(Entity other)
    {
        int nx = (int)(getX());
        int ny = (int)(getY());
        return ny>=other.getY()&&ny<=other.getY()+other.getHeight()&&nx>=other.getX()&&nx<other.getX()+other.getWidth();
    }

    void update()
    {

    }

    void render()
    {

    }

};

class Player : public Entity
{
private:
    bool canMove(int dir)
    {
        if(dir < 0)
        {
            return x-1 > xoffset;
        }
        if(dir > 0)
        {
            return x+width-1 < xoffset+boundsWidth;
        }
        return false;
    }

protected:
    int life = 5;
    int score = 0;
public:
    bool goLeft = false;
    bool goRight = false;
    int getLife(){return life;}
    int getScore(){return score;}
    void addScore(int n){score+=n;}
    void hit(){life--;}
    Player()
    {
        width = 6;
        height = 1;
        x = ((xoffset+boundsWidth)/2)-width/2;
        y = yoffset+boundsHeight-1;
        code = '@';
    }
    void update()
    {
        oldx = x;
        oldy = y;
        if(goLeft && canMove(-1)){
            x--;
        }
        if(goRight && canMove(1)){
            x++;
        }
    }

    void render()
    {

        if(hasUpdate){
            vanishAt(oldx,oldy,width,height);
            drawAt(code,x,y,width,height,White);
            //hasUpdate = false;
        }

    }

};

class Ball : public Entity
{
protected:

public:
    bool onPlayerCollide = false;
    bool onEnemyCollide = false;

    bool dir = false;

    Ball(){}
    Ball(float _x, float _y, float _dx, float _dy)
    {
        x = _x+xoffset;
        y = _y+yoffset;
        dx = _dx;
        dy = _dy;
        width = 1;
        height = 1;
        code = 'o';
    }


    void update()
    {
        dir = dy > 0;
        if(onPlayerCollide || onEnemyCollide)
        {
            dy*=-1;
            onPlayerCollide = false;
            onEnemyCollide = false;
        }
        oldx = x;
        oldy = y;
        x+=dx;
        y+=dy;

        if(getX() < xoffset+2 || getX() >= xoffset + boundsWidth )
        {
            dx *= -1;
        }
        if(getY() <= yoffset+1)
        {
            dy*=-1;
        }
    }

    void render()
    {
        vanishAt(oldx,oldy,height,height);
        drawAt(code,(int)x,(int)y,width,height, White);
    }
};

class Enemy : public Entity
{
protected:
    int color;
public:
    bool isDead = false;
    Enemy(){}
    Enemy(int _x, int _y, int _width, int _height, int _color)
    {
        x = _x;
        y = _y;
        width = _width;
        height = _height;
        code = '@';
        color = _color;
    }

    void update()
    {
        if(isDead) code = ' ';
    }

    void render()
    {
        if(hasUpdate)
        {
            hasUpdate = false;
            drawAt(code,x,y,width,height, color);
        }
    }
};

class Game
{
private:
    int nRows = 4;
    int nCols = 10;
    int n = 0;
    bool endGame = false;
    void spawnBall()
    {
        srand(time(0));
        int xx = xoffset + rand()%boundsWidth-1;
        int yy = yoffset + boundsHeight/2;
        float dx = (rand()%10)%2==0?0.5f:-0.5f;
        ball = Ball(xx,yy,dx,-0.5f);

    }


protected:

      // top down left right
    Wall bounds[4] = {Wall(xoffset,yoffset,boundsWidth+1,1),
                      Wall(xoffset,yoffset+boundsHeight+1,boundsWidth+1,1),
                      Wall(xoffset,yoffset,1,boundsHeight+1),
                      Wall(xoffset+boundsWidth+1,yoffset,1,boundsHeight+2)
                     };



public:
    bool gameOver(){return endGame;}
    Enemy enemies[10][4];
    Player player;
    Ball ball;
    Game()
    {

        spawnBall();
        for(int y = 0; y < nRows; y++)
        {
            for(int x = 0; x < nCols; x++)
            {
                int xx = 1 + xoffset + 3 * x;
                int yy = 1 + yoffset + 1 * y;
                enemies[x][y] = Enemy(xx,yy,3,1,Red);

            }
        }


        player = Player();

    }

    void update()
    {

        if(ball.collide(player))
        {
            ball.onPlayerCollide = true;
        }
        int nDead = 0;
        for(int y = 0; y < nRows; y++)
        {
            for(int x = 0; x < nCols; x++)
            {
                if( !enemies[x][y].isDead && ball.collide(enemies[x][y]))
                {
                    ball.onEnemyCollide = true;
                    enemies[x][y].isDead = true;
                    enemies[x][y].hasUpdate = true;
                    player.addScore(10);
                }
                enemies[x][y].update();
                if(enemies[x][y].isDead)
                {
                    nDead++;
                }
            }
        }
        ball.update();
        player.update();

        if( ball.getY() >= player.getY()+1)
        {
            n++;
            player.hit();
            spawnBall();

        }
        if(nDead == nRows * nCols)
        {
            // WIN

            gotoxy(5,yoffset+boundsHeight/2);
            cout << "Voce venceu!!!!" << endl;
            system("pause");
            endGame = true;
        }
        if(player.getLife() <= 0)
        {
            // LOOSE
            gotoxy(5,yoffset+boundsHeight/2);
            cout << "Voce perdeu!";
            gotoxy(5,1+yoffset+boundsHeight/2);
            cout << "Mais sorte na proxima." << endl;
            gotoxy(0,23);
            system("pause");
            endGame = true;
        }
    }

    void render()
    {
        ball.render();
        for(int c = 0; c < 4; c++)
        {
            bounds[c].render();
        }

         for(int y = 0; y < nRows; y++)
        {
            for(int x = 0; x < nCols; x++)
            {
                enemies[x][y].render();
            }
        }
        player.render();
        gotoxy(0,24);
        cout << "                                              ";
        gotoxy(0,24);
        cout << "Score: " << player.getScore();
        gotoxy(0,25);
        cout << "                                              ";
        gotoxy(0,25);
        cout << "Lifes: " << player.getLife();
    }
};


int main()
{
    HWND console = GetConsoleWindow();
    SetConsoleTitle("Bricks! By Sakamoto");
    SetWindowPos(
                 console,
                 HWND_TOP,
                 500,
                 100,
                 cWidth,
                 cHeight,
                 SWP_NOOWNERZORDER
                 );



    Game game = Game();
    clock_t stime = clock();
    int time = 0;
    int old_time = 0;
    float FPS = 10;
    ShowConsoleCursor(false);
    while(ISRUNNING){
        time = (clock()-stime)/(CLOCKS_PER_SEC/FPS);
        if(time != old_time)
        {
            old_time = time;
            if(!game.gameOver())
            {
                game.update();
                game.render();
            }else
            {
                game = Game();
            }

        }

        if(GetKeyState(0x41) != 0 && GetKeyState(0x41) != 1)
        {
            game.player.goLeft = true;
        }else
        {
            game.player.goLeft = false;
        }
        if( GetKeyState(0x44) != 0 && GetKeyState(0x44) != 1 )
        {
            game.player.goRight = true;
        }else
        {
            game.player.goRight = false;
        }




    }
    return 0;
}






void gotoxy(int x, int y)
{
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),(COORD){x,y});
}
void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}
void vanishAt(int x, int y, int width, int height)
{
    for(int yy = y; yy < height+y; yy++)
    {
        for(int xx = x; xx < width+x; xx++)
        {
            gotoxy(xx,yy);
            cout << ' ';
        }
    }
};
void drawAt(char code, int x, int y, int width, int height, int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    for(int yy = y; yy < height+y; yy++)
        {
        for(int xx = x; xx < width+x; xx++)
        {
            gotoxy(xx,yy);
            cout << code;
        }
    }
};
