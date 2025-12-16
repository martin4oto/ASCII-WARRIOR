#include <iostream>
#include <ctime>
#include <conio.h>
#include <movement.h>

using namespace std;

Player player;
clock_t lastFrameTime = 0;

const int height = 31;
const int width = 101;

Vector2 gravityPull;
Vector2 vector_left;
Vector2 vector_right;

void ClearBoard()
{
    for(int i = 0; i<width; i++)
    {
        for(int j = 0; j<height; j++)
        {
            board[i][j] = 0;
        }
    }
}

void AddPlayer()
{
    player.HP = 5;

    Vector2 *playerPosition = new Vector2;

    player.position = playerPosition;

    int X = (int)(width/2) +1;
    int Y = (int)(height/2) +1;

    playerPosition->x = X;
    playerPosition->y = Y;

    board[X][Y] = player_index;
}

void PutBorrder()
{
    //top and down
    for(int i = 0; i<width; i++)
    {
        board[i][0] = 1;
        board[i][height-1] = 1;
    }


    //left and right
    for(int i = 0; i<height; i++)
    {
        board[0][i] = 1;
        board[width-1][i] = 1;
    }
}

void PrintHeader()
{
    cout<<"HP: "<<player.HP;
    cout<<endl;
}

void PrintFullBoard()
{
    PrintHeader();

    for(int y = 0; y<height; y++)
    {
        for(int x = 0; x<width; x++)
        {
            PrintObject(board[x][y]);
        }
        cout<<endl;
    }
}

char GetInput()
{
    //TODO better
    if(_kbhit())
    {
        return _getch();
    }

    return 0;
}

float GetDeltaTime()
{
    clock_t current_time = clock();

    //time between the last frame and the new one
    float delta = current_time-lastFrameTime;

    lastFrameTime = current_time;

    return delta;
}

void GameSetup()
{
    board = new unsigned int*[width];

    for(int i = 0; i < width; i++)
    {
        board[i] = new unsigned int[height];
    }
    ClearBoard();
    PutBorrder();
    AddPlayer();

    gravityPull.x = 0;
    gravityPull.y = 1;
    vector_left.x = -1;
    vector_left.y = 0;
    vector_right.x = 1;
    vector_right.y = 0;
}

void InputManager(char input)
{
    if(input == 0) return;

    if(input == 'a')
    {
        MovePlayer(vector_left, &player);
    }
    else if(input == 'd')
    {
        MovePlayer(vector_right, &player);
    }
    else if(input == ' ' && player.jumpsLeft)
    {
        PlayerJump();
    }
}

int main()
{
    system("MODE CON COLS=101 LINES=40");
    GameSetup();
    PrintFullBoard();
    float timer = 0;
    timer += GetDeltaTime();

    while(true)
    {
        GravityStep(&gravityPull, &player);

        char input = GetInput();
        InputManager(input);

        while(timer<=150)
        {
            float delta = GetDeltaTime();
            timer += delta;
        }
        timer = 0;
    }

    return 0;
}
