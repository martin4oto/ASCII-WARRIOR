#include <iostream>
#include <ctime>
#include <characters.h>
#include <conio.h>
#include <objects.h>

using namespace std;

Player player;
clock_t lastFrameTime;

unsigned int **board;
const int height = 29;
const int width = 100;

void ClearBoard()
{
    for(int i = 0; i<height; i++)
    {
        for(int j = 0; j<width; j++)
        {
            board[i][j] = 0;
        }
    }
}

void PutBorrder()
{
    //top and down
    for(int i = 0; i<width; i++)
    {
        board[0][i] = 1;
        board[height-1][i] = 1;
    }


    //left and right
    for(int i = 0; i<height; i++)
    {
        board[i][0] = 1;
        board[i][width-1] = 1;
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

    for(int i = 0; i<height; i++)
    {
        for(int j = 0; j<width; j++)
        {
            switch(board[i][j])
            {
                case air:
                    cout<<" ";
                    break;
                case barrier:
                    cout<<"#";
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
    player.HP = 5;

    board = new unsigned int*[height];

    for(int i = 0; i < height; i++)
    {
        board[i] = new unsigned int[width];
    }
    ClearBoard();
    PutBorrder();
}

int main()
{
    system("MODE CON COLS=100 LINES=40");
    GameSetup();
    PrintFullBoard();
    return 0;
}
