#include <iostream>
#include <ctime>
#include <conio.h>
#include <movement.h>

using namespace std;

Player player;
clock_t lastFrameTime;

unsigned int **board;
const int height = 31;
const int width = 101;

Vector2 gravityPull;

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
            switch(board[x][y])
            {
                case air:
                    cout<<" ";
                    break;
                case barrier:
                    cout<<"#";
                    break;
                case player_index:
                    cout<<"@";
                    break;
            }
        }
        cout<<endl;
    }
}

char GetInput()
{
    //TODO better
    return _getch();
}

float GetDeltaTime()
{
    clock_t current_time = clock();

    //time between the last frame and the new one
    float delta = (lastFrameTime-current_time)/(float)CLOCKS_PER_SEC;

    lastFrameTime = current_time;
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
}

int main()
{
    system("MODE CON COLS=101 LINES=40");
    GameSetup();
    PrintFullBoard();

    GravityStep(&gravityPull, board, &player);
    while(true)
    {
    }

    return 0;
}
