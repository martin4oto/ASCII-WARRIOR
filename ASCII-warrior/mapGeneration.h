#include <cstdlib>
#include <objects.h>

char **board;

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

void putPlatforms()
{
    //why not random???
    for(int i = 0; i < numberOfPlatforms; i++)
    {
        int x = 1 + (rand()%(width-5));
        int y = 1 + (rand()%(height-2));

        int platformSize = minPlatformSize + (rand()%(maxPlatformSize - minPlatformSize));

        for(int j = 0; j<platformSize;j++)
        {
            if(x + j >= width)
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

bool isEmptyOrEnemy(int x, int y)
{
    return board[x][y] != platform && board[x][y] != barrier;
}


void PrintObject(Vector2 *position)
{
    PrintObject(board[position->x][position->y]);
}
