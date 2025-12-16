using namespace std;

unsigned int **board;

const unsigned int air = 0;
const unsigned int barrier = 1;
const unsigned int player_index = 2;

void PrintObject(unsigned int objectToPrint)
{
    switch(objectToPrint)
    {
    case air:
        putch(' ');
        break;
    case barrier:
        putch('#');
        break;
    case player_index:
        putch('@');
        break;
    }
}

bool isEmpty(int x, int y)
{
    return !board[x][y];
}
