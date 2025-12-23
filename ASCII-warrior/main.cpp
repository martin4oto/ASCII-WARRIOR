#include <iostream>
#include <ctime>
#include <conio.h>
#include <movement.h>

using namespace std;

Player player;
clock_t lastFrameTime = 0;

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
    board = new char*[width];

    for(int i = 0; i < width; i++)
    {
        board[i] = new char[height];
    }
    GenerateMap();
    AddPlayer();

    vector_down.x = 0;
    vector_down.y = 1;
    vector_left.x = -1;
    vector_left.y = 0;
    vector_right.x = 1;
    vector_right.y = 0;
    vector_up.x = 0;
    vector_up.y = -1;
}

void InputManager(char input)
{
    if(input == 0) return;

    if(input == 'a')
    {
        MovePlayer(&vector_left, &player);
    }
    else if(input == 'd')
    {
        MovePlayer(&vector_right, &player);
    }
    else if(input == 'w')
    {
        PlayerJump(&player);
    }
    else if(input == 'i')
    {
        tryAttacking(AttackDirections::up, &player);
    }
    else if(input == 'k')
    {
        tryAttacking(AttackDirections::down, &player);
    }
    else if(input == 'l')
    {
        tryAttacking(AttackDirections::right, &player);
    }
    else if(input == 'j')
    {
        tryAttacking(AttackDirections::left, &player);
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
        char input = GetInput();
        InputManager(input);

        GravityStep(&vector_down, &player);

        while(timer<=150)
        {
            float delta = GetDeltaTime();
            timer += delta;
            AnimatinStep(delta);
        }
        timer = 0;
    }

    return 0;
}
