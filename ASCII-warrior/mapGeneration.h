unsigned int **board;

const int height = 31;
const int width = 101;

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

}

void GenerateMap()
{
    ClearBoard();
    PutBorrder();
}
