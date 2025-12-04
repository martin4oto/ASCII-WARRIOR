#include <characters.h>
#include <objects.h>
#include <windows.h>

void SetCursorPosition(Vector2 *position)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD coordinates =
    {
         (position -> x),
         (position -> y)+1
    };

    SetConsoleCursorPosition(hStdOut, coordinates);
}

void MoveObject(Vector2 *moveFrom, Vector2 *moveTo)
{
    SetCursorPosition(moveFrom);
    std::cout<<"A";
}

void GravityStep(Vector2 *gravityScale, unsigned int** board, Player* playerObject)
{
    Vector2 *playerPosition = playerObject->position;

    //calculate where the player should move
    Vector2* playerMove = AddVectors(playerPosition, gravityScale);

    //there is nothing under the player
    if(board[playerMove->x][playerMove->y] != barrier)
    {

        //check for colision
        //TODO
        MoveObject(playerPosition,playerMove);

        playerObject->position = playerMove;
        delete[] playerPosition;
    }
    else
    {
        delete[] playerMove;
    }
}
