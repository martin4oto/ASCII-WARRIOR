#include <characters.h>
#include <objects.h>
#include <windows.h>

//returns the free spaces that are <= to the iterations
int CheckForVerticalSpace(Vector2 *startingPosition, int iterations, int direction)
{
    int x = startingPosition->x;
    int y = startingPosition->y;

    for(int i = 0; i<iterations; i++)
    {
        if(!isEmpty(x,y + (i*direction)))
        {
            return i;
        }
    }
    return iterations;
}

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

    board[moveTo->x][moveTo->y] = objectToMove;
    board[moveFrom->x][moveFrom->y] = air;
}

int MovePlayer(Vector2 direction, Player* playerObject)
{
    Vector2 *playerPosition = playerObject->position;

    //calculate where the player should move
    Vector2* playerMove = AddVectors(playerPosition, &direction);

    //there is nothing under the player
    if(board[playerMove->x][playerMove->y] != barrier)
    {
        //check for colision
        //TODO
        MoveObject(playerPosition,playerMove);

        playerObject->position = playerMove;
        delete[] playerPosition;
        return 0;
    }
    else
    {
        delete[] playerMove;
        return 1;
    }
}

void GravityStep(Vector2 *gravityPull,Player *player)
{
    Vector2 momentum = player->verticalMomentum;

    AddVectorsDirectly(&momentum, gravityPull);



    MovePlayer(player->verticalMomentum, player);
}

void PlayerJump()
{

}
