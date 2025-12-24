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

void RemoveEnemy(int);


void MoveObject(Vector2 *moveFrom, Vector2 *moveTo)
{
    if(AreEqual(moveFrom, moveTo))return;

    unsigned int objectToMove = board[moveFrom->x][moveFrom->y];

    SetCursorPosition(moveFrom);
    PrintObject(air);

    SetCursorPosition(moveTo);
    PrintObject(objectToMove);

    board[moveFrom->x][moveFrom->y] = air;
    board[moveTo->x][moveTo->y] = objectToMove;
}

bool MoveEnemy(Vector2 *direction, Vector2* position)
{
    if(AreEqual(direction, &vector_zero))return 1;
    //calculate where the player should move
    Vector2* enemyMove = AddVectors(position, direction);
    //cout<<playerPosition->y;

    //there is nothing under the player
    if(isEmpty(enemyMove->x, enemyMove->y))
    {
        MoveObject(position,enemyMove);

        *position = *enemyMove;
        delete[] enemyMove;
        return true;
    }
    else
    {
        delete[] enemyMove;
        return false;
    }
}

int MovePlayer(Vector2 *direction, Vector2* playerPosition)
{
    if(AreEqual(direction, &vector_zero))return 1;
    //calculate where the player should move
    Vector2* playerMove = AddVectors(playerPosition, direction);
    //cout<<playerPosition->y;

    //there is nothing under the player
    if(isEmptyOrEnemy(playerMove->x, playerMove->y))
    {
        //check for colision
        unsigned char boardValue;
        if(boardValue = isEnemy(playerMove))
        {
            RemoveEnemy(boardValue-5);
        }


        MoveObject(playerPosition,playerMove);

        *playerPosition = *playerMove;
        delete[] playerMove;
        return 0;
    }
    else
    {
        delete[] playerMove;
        return 1;
    }
}

void GravityStep(Vector2 *gravityPull,Vector2 *momentum, Vector2 *position, int &jumpsLeft)
{
    AddVectorsDirectly(momentum, gravityPull);

    int freePosibleSpaces = CheckForVerticalSpace(
                            position,
                            momentum->y);

    int scalarMomentum = momentum->y;


    if(freePosibleSpaces == 0)
    {
        ZeroVector(momentum);
        if(sign(scalarMomentum) == 1)
        {
            jumpsLeft = maxJumps;
        }
        return;
    }

    if(scalarMomentum == 0) return;

    //cout<<freePosibleSpaces<<momentum->y;
    //cout<<endl<<endl<<freePosibleSpaces;

    if(freePosibleSpaces>Abs(scalarMomentum))
    {
        momentum->y = scalarMomentum;
    }
    else
    {
        if(sign(scalarMomentum) == 1)
        {
            jumpsLeft = maxJumps;
        }
        momentum->y = freePosibleSpaces * sign(scalarMomentum);
    }
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
