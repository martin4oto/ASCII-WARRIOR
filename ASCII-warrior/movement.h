#include <characters.h>
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

void MoveObject(Vector2 *moveFrom, Vector2 *moveTo)
{
    unsigned int objectToMove = board[moveFrom->x][moveFrom->y];

    SetCursorPosition(moveFrom);
    PrintObject(air);

    SetCursorPosition(moveTo);
    PrintObject(objectToMove);
}

void GravityStep(Vector2 *gravityScale, Player* playerObject)
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
