#include <mapGeneration.h>

using namespace std;

const unsigned int air = 0;
const unsigned int barrier = 1;
const unsigned int platform = 2;
const unsigned int player_index = 3;

void PrintObject(unsigned int objectToPrint)
{
    switch(objectToPrint)
    {
    case air:
        cout<<' ';
        break;
    case barrier:
        cout<<'#';
        break;
    case platform:
        cout<<'=';
        break;
    case player_index:
        cout<<'@';
        break;
    }
}

bool isEmpty(int x, int y)
{
    return !board[x][y];
}
