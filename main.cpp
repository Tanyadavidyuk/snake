#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <vector>

using namespace std;

enum direction {STOP,LEFT, RIGHT, UP, DOWN};
static const int    WIDTH = 25;
static const int    HEIGHT = 25;
int                 score;
int                 x,y;   //координаты головы змеи
int                 apple_x, apple_y;
bool                gameOver;
int                 snake_dir;
vector<int>         snake_body_x;
vector<int>         snake_body_y;
char                game_Field[WIDTH][HEIGHT];
struct              termios saved_attributes;

void drow();
void initGame();
void appleFactory();
void eatApple();
void move();
void change_direction();
void checkCollisions();
void game_over();
void clear_gameField();


void set_presskey(){
    struct termios new_attributes;
    tcgetattr(0,&saved_attributes);/* считываем текущий режим работы терминала */
    new_attributes = saved_attributes;
    /* включаем неканонический режим без эха */
    new_attributes.c_lflag &= ~(ICANON|ECHO);
    /* считываем минимум по одному символу */
    new_attributes.c_cc[VMIN] = 1;
    /* без ограничения времени ожидания */
    new_attributes.c_cc[VTIME] = 0;
    /* устанавливаем новый режим работы терминала */
    tcsetattr(0, TCSANOW, &new_attributes);

}

void reset_presskey(){
    tcsetattr(0, TCSANOW, &saved_attributes);
}

int main()
{
    fd_set rfds;
    struct timeval tv;
    int retval;
    set_presskey();
    initGame();
    while (!gameOver) {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        retval = select(2, &rfds, nullptr, nullptr, &tv);
       if(retval != 0)
           change_direction();
       move();
       drow();
       sleep(1);//в сек
    }
    game_over();
    reset_presskey();
    return 0;
}

void game_over()
{
    cout << "GAME OVER" << endl;
}

void initGame()
{
    //snake_dir = RIGHT;
    snake_dir = 0 + rand() % 4;
    gameOver = false;
    score = 0;
    x = WIDTH / 2;
    y = WIDTH / 2;
    snake_body_x.push_back(x);
    snake_body_y.push_back(y);
    appleFactory();
    drow();
}

void appleFactory()
{
    apple_x = rand() % WIDTH;
    apple_y = rand() % HEIGHT;
}

void move()
{
    checkCollisions();
    for(size_t i = snake_body_x.size()-1; i > 0; i--){
        snake_body_x[i] = snake_body_x[i-1];
        snake_body_y[i] = snake_body_y[i-1];
    }
    if(snake_dir == RIGHT)
        x++;
    else if( snake_dir == UP)
        y--;
    else if( snake_dir == LEFT)
        x--;
    else if (snake_dir == DOWN)
        y++;

    snake_body_x[0] = x;
    snake_body_y[0] = y;
}

void change_direction()
{
    int direction = getc(stdin);
    switch (direction) {
        case 'd':
            snake_dir = RIGHT;
            break;
        case 's':
            snake_dir = DOWN;
            break;
        case 'a':
            snake_dir = LEFT;
            break;
        case 'w':
            snake_dir = UP;
            break;
        default:
            break;
    }
}

void eatApple()
{
    if( snake_dir == LEFT){
        snake_body_x.push_back(snake_body_x[snake_body_x.size()-1]-1);
        snake_body_y.push_back(snake_body_y[snake_body_y.size()-1]);
    }
    else if( snake_dir == RIGHT){
        snake_body_x.push_back(snake_body_x[snake_body_x.size()-1]+1);
        snake_body_y.push_back(snake_body_y[snake_body_y.size()-1]);
    }
    else if( snake_dir == UP){
        snake_body_x.push_back(snake_body_x[snake_body_x.size()-1]);
        snake_body_y.push_back(snake_body_y[snake_body_y.size()-1]-1);
    }
    else if( snake_dir == DOWN){
        snake_body_x.push_back(snake_body_x[snake_body_x.size()-1]);
        snake_body_y.push_back(snake_body_y[snake_body_y.size()-1]+1);
    }

}

void checkCollisions()
{
    if( x == WIDTH || x == 0 || y == 0 || y == HEIGHT)
        gameOver = true;
    else if ( x == apple_x && y == apple_y){
        score++;
        eatApple();
        appleFactory();
    }
    else{
        for(size_t i = 1; i < snake_body_x.size(); i++){
            if(x == snake_body_x[i] && y == snake_body_y[i])
                gameOver = true;
        }
    }
}


void clear_gameField(){
    for (int row = 0 ; row <= HEIGHT;row++) {
        for(int column = 0; column <= WIDTH; column++){
            if(column == 0 || column == WIDTH || row == 0 || row == HEIGHT)
                game_Field[row][column] = '#';
            else
                game_Field[row][column] = ' ';
        }
    }
    game_Field[apple_y][apple_x] = '*';
    for(size_t i = 0; i < snake_body_x.size();i++)
        game_Field[snake_body_y[i]][snake_body_x[i]] = '@';
}

void drow()
{
    system("clear");
    clear_gameField();
    for (int i = 0 ; i <= HEIGHT;i++) {
        for(int j = 0; j <= WIDTH; j++){
            cout << game_Field[i][j];
        }
        cout << endl;
    }
    cout << "Score: " << score << endl;
}
