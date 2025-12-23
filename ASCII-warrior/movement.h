#include <characters.h>
#include <attackHandling.h>

const int jumpHeight = -5;
const int maxJumps = 2;


int sign(int number)
{
    int signBit = number & (1<<31);

    return signBit?-1:1;
}

int Abs(int number)
{
    return number*sign(number);
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
            //cout<<i-1;
            return i-1;
        }
    }
    return iterations + 1;
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

int MovePlayer(Vector2 *direction, Player* playerObject)
{
    Vector2 *playerPosition = playerObject->position;

    //calculate where the player should move
    Vector2* playerMove = AddVectors(playerPosition, direction);
    //cout<<playerPosition->y;

    //there is nothing under the player
    if(isEmptyOrEnemy(playerMove->x, playerMove->y))
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


    if(freePosibleSpaces == 0)
    {
        ZeroVector(momentum);
        if(sign(scalarMomentum) == 1)
        {
            player->jumpsLeft = maxJumps;
        }
        return;
    }

    if(scalarMomentum == 0) return;

    //cout<<freePosibleSpaces<<momentum->y;
    //cout<<endl<<endl<<freePosibleSpaces;

    if(freePosibleSpaces>Abs(scalarMomentum))
    {
        player->verticalMomentum.y = scalarMomentum;
    }
    else
    {
        if(sign(scalarMomentum) == 1)
        {
            player->jumpsLeft = maxJumps;
        }
        player->verticalMomentum.y = freePosibleSpaces * sign(scalarMomentum);
    }

    MovePlayer(&player->verticalMomentum, player);
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
