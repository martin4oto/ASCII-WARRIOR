#include <characters.h>
#include <objects.h>
#include <windows.h>

const int jumpHeight = -5;
const int maxJumps = 2;


int sign(int number)
{
    int signBit = ((number & (1<<31))>>31);

    return signBit?-1:1;
}

//returns the free spaces that are <= to the abs(iterations)
//if the next position is not ocupied, then the function should retun a number grater than abs(iterations)
int CheckForVerticalSpace(Vector2 *startingPosition, int iterations)
{
    int x = startingPosition->x;
    int y = startingPosition->y;

    int direction = sign(iterations);
    iterations *= direction;

    for(int i = 1; i<=iterations+1; i++)
    {
        if(!isEmpty(x,
                    y + (i*direction)))
        {
            return i-1;
        }
    }
    return iterations + 1;
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
    //TODO: make into diff functions
    Vector2 *momentum = &player->verticalMomentum;

    AddVectorsDirectly(momentum, gravityPull);

    int freePosibleSpaces = CheckForVerticalSpace(
                            player->position,
                            momentum->y);

    int scalarMomentum = momentum->y;

    if(freePosibleSpaces == 0||scalarMomentum == 0)
    {
        ZeroVector(momentum);
        return;
    }


    if(freePosibleSpaces>scalarMomentum)
    {
        player->verticalMomentum.y = scalarMomentum;
    }
    else
    {
        if(sign(momentum->y) == 1)
        {
            player->jumpsLeft = maxJumps;
        }
        player->verticalMomentum.y = freePosibleSpaces;
    }

    MovePlayer(player->verticalMomentum, player);
}

void PlayerJump(Player* playerObject)
{
    if(!(playerObject->jumpsLeft))
    {
        return;
    }

    playerObject->verticalMomentum.y = jumpHeight;
    playerObject->jumpsLeft--;
}
