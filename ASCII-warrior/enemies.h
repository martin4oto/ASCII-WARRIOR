#include <movement.h>

const int maxEnemiesAllowed = 64;
int currentEnemiesAlive = 0;
Enemy* enemiesAlive;

const Enemy WalkerBlueprint =
{
    EnemyTypes::Walker,
    1,
    vector_zero,
    1
};

void AddEnemy(Enemy* enemyToAdd)
{
    enemiesAlive[currentEnemiesAlive] = *enemyToAdd;
    currentEnemiesAlive++;
}

void RemoveEnemy(int EnemyIndex)
{
    for(int i = EnemyIndex+1; i<currentEnemiesAlive; i++)
    {
        enemiesAlive[i-1] = enemiesAlive[i];
    }

    currentEnemiesAlive--;
}

void FindTheClosestFloor(Vector2 *position)
{
    while(!canStand(*position))
    {
        position->y++;
    }
}

Vector2 GetRandomEmptyVector()
{
    Vector2 v;
    do
    {
        v = {
            GenerateRandom(1, width-2),
            GenerateRandom(1, height-2)
        };
    }
    while(isEmpty(v.x, v.y));
    return v;
}


void SpawnWalker()
{
    Vector2 initialPosition = GetRandomEmptyVector();

    FindTheClosestFloor(&initialPosition);

    board[initialPosition.x][initialPosition.y] = walker_object;

    Enemy walker = WalkerBlueprint;
    walker.position = initialPosition;
    AddEnemy(&walker);
}

//==================

void WalkerStep(Enemy *enemy)
{
    Vector2 *position = &enemy->position;
    int direction = enemy->extraInfo;

    Vector2 desiredPosition = *position;
    desiredPosition.x += direction;

    if(canStand(desiredPosition))
    {
        MoveObject(position, &desiredPosition);
        *position = desiredPosition;
        return;
    }

    //change direction
    enemy->extraInfo *= -1;
}

void SpawnEnemy()
{
    int randomEnemy = rand()%enemyTypeCount;
    EnemyTypes enemyType = (EnemyTypes)randomEnemy;

    switch(enemyType)
    {
    case Walker:
        SpawnWalker();
        break;
    }
}

void EnemiesStep()
{
    for(int i = 0; i<currentEnemiesAlive; i++)
    {
        EnemyTypes currentType = enemiesAlive[i].type;

        switch(currentType)
        {
        case EnemyTypes::Walker:
            WalkerStep(enemiesAlive + i);
            break;
        }
    }
}
