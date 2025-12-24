#include <iostream>
#include <ctime>
#include <conio.h>
#include <enemies.h>

using namespace std;

clock_t lastFrameTime = 0;

const int numberOfWaves = 3;

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
    Vector2 v = {0,-1};

    SetCursorPosition(&v);
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
    board = new unsigned char*[width];

    for(int i = 0; i < width; i++)
    {
        board[i] = new unsigned char[height];
    }
    GenerateMap();
    AddPlayer();

    enemiesAlive = new Enemy[maxEnemiesAllowed];

    vector_down.x = 0;
    vector_down.y = 1;
    vector_left.x = -1;
    vector_left.y = 0;
    vector_right.x = 1;
    vector_right.y = 0;
    vector_up.x = 0;
    vector_up.y = -1;
    vector_zero.x = 0;
    vector_zero.y = 0;
}

void InputManager(char input)
{
    if(input == 0) return;

    if(input == 'a')
    {
        MovePlayer(&vector_left, player.position);
    }
    else if(input == 'd')
    {
        MovePlayer(&vector_right, player.position);
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

    int curWaveNumber = initialWaveNumber;

    for(int i = 0; i<numberOfWaves; i++)
    {
        SpawnWave(curWaveNumber);

        PrintFullBoard();
        float timer = 0;
        timer += GetDeltaTime();

        while(true)
        {
            char input = GetInput();
            InputManager(input);

            GravityStep(&vector_down, &player.verticalMomentum, player.position, player.jumpsLeft);
            MovePlayer(&player.verticalMomentum, player.position);

            EnemiesStep();

            if(currentEnemiesAlive == 0)break;

            while(timer<=150)
            {
                float delta = GetDeltaTime();
                timer += delta;
                AnimatinStep(delta);
            }
            timer = 0;
        }

        curWaveNumber += GenerateRandom(3,5);
    }
    return 0;
}
