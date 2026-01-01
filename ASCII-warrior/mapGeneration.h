#include <cstdlib>
#include <objects.h>
#include <windows.h>

void SetCursorPosition(Vector2 *position)
{
    //coment ts
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD coordinates =
    {
         (position -> x),
         (position -> y)+1
    };

    SetConsoleCursorPosition(hStdOut, coordinates);
}

Vector2 vector_down;
Vector2 vector_up;
Vector2 vector_left;
Vector2 vector_right;
Vector2 vector_zero;

unsigned char **board;

const int height = 31;
const int width = 101;
const int numberOfPlatforms = 9;

const int minPlatformSize = 5;
const int maxPlatformSize = 20;

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

int GenerateRandom(int minimum, int maximum)
{
    return minimum + (rand()%(maximum - minimum+1));
}

void putPlatforms()
{
    //why not random???
    for(int i = 0; i < numberOfPlatforms; i++)
    {
        int x = GenerateRandom(1, width-2);
        int y = GenerateRandom(1, height-2);

        int platformSize = GenerateRandom(minPlatformSize, maxPlatformSize);

        for(int j = 0; j<platformSize;j++)
        {
            if(x + j >= width-1)
            {
                break;
            }
            board[x + j][y] = platform;
        }
    }
}

void GenerateMap()
{
    ClearBoard();
    PutBorrder();
    putPlatforms();
}


bool isEmpty(int x, int y)
{
    return !board[x][y];
}

bool isEmpty(Vector2 *position)
{
    return !board[position->x][position->y];
}

bool isEmptyOrEnemy(int x, int y)
{
    return board[x][y] != platform && board[x][y] != barrier;
}

bool canStand(Vector2 position)
{
    Vector2 tileUnder =
    {
        position.x,
        position.y+1
    };

    return !isEmptyOrEnemy(tileUnder.x, tileUnder.y)&&isEmpty(position.x, position.y);
}

//to fix
bool canStandorHang(Vector2 position)
{
    Vector2 tileUp =
    {
        position.x,
        position.y - 1
    };

    Vector2 tileDown =
    {
        position.x,
        position.y + 1
    };

    return (!isEmptyOrEnemy(tileUp.x, tileUp.y)||
            !isEmptyOrEnemy(tileDown.x, tileDown.y))&&
            isEmpty(position.x, position.y);
}

bool canCrawl(Vector2 position)
{
    Vector2 tileLeft =
    {
        position.x - 1,
        position.y
    };

    Vector2 tileRight =
    {
        position.x + 1,
        position.y
    };

    return (!isEmptyOrEnemy(tileLeft.x, tileLeft.y)||
            !isEmptyOrEnemy(tileRight.x, tileRight.y))&&
            isEmpty(position.x, position.y);
}

unsigned char isEnemy(Vector2 *position)
{
    return board[position->x][position->y]>=4 ? board[position->x][position->y]:0;
}

void PrintObject(Vector2 *position)
{
    PrintObject(board[position->x][position->y]);
}

void RemoveObject(Vector2 *position)
{
    int x = position->x;
    int y = position->y;

    board[x][y] = air;

    SetCursorPosition(position);
    cout<<' ';
}

void DecreaseValueByOne(Vector2 *position)
{
    board[position->x][position->y]--;
}
