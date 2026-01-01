#include <movement.h>

int RoundUp(double number)
{
    int integerPart = (int)number;

    return (number-integerPart)>0? integerPart+1: integerPart;
}



const int initialWaveNumber = 5;

Player player;
void PrintHeader();

void DamagePlayer(int DMG)
{
    player.HP-=DMG;
    PrintHeader();
}

const int maxEnemiesAllowed = 64;
int currentEnemiesAlive = 0;

const Enemy WalkerBlueprint =
{
    'E',
    EnemyTypes::Walker,
    1,
    vector_zero,
    nullptr
};

const Enemy CrawlerBlueprint =
{
    'C',
    EnemyTypes::Crawler,
    1,
    vector_zero,
    nullptr
};

const Enemy FlierBlueprint =
{
    'F',
    EnemyTypes::Flier,
    1,
    vector_zero,
    nullptr
};

const Enemy JumperBlueprint =
{
    'J',
    EnemyTypes::Jumper,
    1,
    vector_zero,
    nullptr
};

const Enemy BossBlueprint =
{
    'B',
    EnemyTypes::Boss,
    10,
    vector_zero,
    nullptr
};

AddSpecialEnemy(Enemy* enemyToAdd, int enemySize)
{
    enemiesAlive[currentEnemiesAlive] = *enemyToAdd;
    int offset = enemySize/2;

    for(int i = 0; i<enemySize; i++)
    {
        for(int j = 0; j<enemySize; j++)
        {
            board[enemyToAdd->position.x + i - offset][enemyToAdd->position.y + j - offset] = currentEnemiesAlive+5;
        }
    }

    currentEnemiesAlive++;
}

void AddEnemy(Enemy* enemyToAdd)
{
    enemiesAlive[currentEnemiesAlive] = *enemyToAdd;

    board[enemyToAdd->position.x][enemyToAdd->position.y] = currentEnemiesAlive+5;
    currentEnemiesAlive++;
}

void RemoveEnemy(int EnemyIndex)
{
    delete[] enemiesAlive[EnemyIndex].extraInfo;

    Vector2 *position = &enemiesAlive[EnemyIndex].position;
    RemoveObject(position);

    for(int i = EnemyIndex+1; i<currentEnemiesAlive; i++)
    {
        DecreaseValueByOne(&enemiesAlive[i].position);
        enemiesAlive[i-1] = enemiesAlive[i];
    }

    currentEnemiesAlive--;
}

bool CollideWithEnemy(int EnemyIndex)
{
    if(enemiesAlive[EnemyIndex].type == Boss)
    {
        return false;
    }

    RemoveEnemy(EnemyIndex);
    return true;
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
    while(!isEmpty(v.x, v.y));
    return v;
}

Vector2 GetRandomEmptyVector(int Size)
{
    Vector2 v;
    do
    {
        v = {
            GenerateRandom(1, width-2),
            GenerateRandom(1, height-2)
        };
    }
    while(!IsEmpty_Boss(v, Size));
    return v;
}

//===============================
void SpawnWalker()
{
    Vector2 initialPosition = GetRandomEmptyVector();

    FindTheClosestFloor(&initialPosition);

    Enemy walker = WalkerBlueprint;
    walker.position = initialPosition;
    walker.extraInfo = new int(1);
    AddEnemy(&walker);
}

bool WalkerStep(Enemy *enemy)
{
    Vector2 *position = &enemy->position;
    int direction = *enemy->extraInfo;

    Vector2 desiredPosition = *position;
    desiredPosition.x += direction;

    if(AreEqual(&desiredPosition,player.position))
    {
        return true;
    }

    if(canStand(desiredPosition))
    {
        MoveObject(position, &desiredPosition);
        *position = desiredPosition;

        return false;
    }

    //change direction
    *enemy->extraInfo *= -1;
    return false;
}
//=========================================

void SpawnCrawler()
{
    Vector2 initialPosition = GetRandomEmptyVector();

    FindTheClosestFloor(&initialPosition);

    Enemy crawler = CrawlerBlueprint;
    crawler.position = initialPosition;
    crawler.extraInfo = new int[2]{1,0};
    AddEnemy(&crawler);
}

void CrawlerGetNewDirection(Enemy *enemy)
{
    Vector2 position = enemy->position;
    Vector2 direction =
    {
        enemy->extraInfo[0],
        enemy->extraInfo[1]
    };

    if(direction.x)
    {
        Vector2 testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_up);
        if(canCrawl(testPosition))
        {
            enemy->extraInfo[0] = vector_up.x;
            enemy->extraInfo[1] = vector_up.y;
            return;
        }

        testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_down);
        if(canCrawl(testPosition))
        {
            enemy->extraInfo[0] = vector_down.x;
            enemy->extraInfo[1] = vector_down.y;
            return;
        }
    }
    else
    {
        Vector2 testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_left);
        if(canStandorHang(testPosition))
        {
            enemy->extraInfo[0] = vector_left.x;
            enemy->extraInfo[1] = vector_left.y;
            return;
        }


        testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_right);
        if(canStandorHang(testPosition))
        {
            enemy->extraInfo[0] = vector_right.x;
            enemy->extraInfo[1] = vector_right.y;
            return;
        }
    }

    enemy->extraInfo[0] *= -1;
    enemy->extraInfo[1] *= -1;
}

bool CrawerStep(Enemy *enemy)
{
    Vector2 *position = &enemy->position;
    Vector2 direction =
    {
        enemy->extraInfo[0],
        enemy->extraInfo[1]
    };

    Vector2 desiredPosition = direction;

    AddVectorsDirectly(&desiredPosition, position);

    if(AreEqual(&desiredPosition,player.position))
    {
        return true;
    }

    //change direction

    if(direction.x)
    {
        if(canStandorHang(desiredPosition))
        {
            MoveObject(position, &desiredPosition);
            *position = desiredPosition;

            return false;
        }
    }
    else
    {
        if(canCrawl(desiredPosition))
        {
            MoveObject(position, &desiredPosition);
            *position = desiredPosition;

            return false;
        }
    }

    CrawlerGetNewDirection(enemy);
    return false;
}
//=================================================

void SpawnFlier()
{
    Vector2 initialPosition = GetRandomEmptyVector();

    Enemy flier = FlierBlueprint;
    flier.position = initialPosition;
    flier.extraInfo = new int(1);
    AddEnemy(&flier);
}

const int flyPercentage = 10;

bool Fly(Vector2 *position)
{
    //change y position- flyPercentage% chance
    int flyNumber = GenerateRandom(1, 100);

    if(flyPercentage >= flyNumber)
    {
        int yOffset = 0;

        Vector2 desiredPosition = vector_zero;

        desiredPosition.y = player.position->y-position->y;
        if(!desiredPosition.y) return false;

        desiredPosition.y = sign(desiredPosition.y);

        AddVectorsDirectly(&desiredPosition, position);

        if(AreEqual(&desiredPosition,player.position))
        {
            return true;
        }

        if(isEmpty(desiredPosition.x, desiredPosition.y))
        {
            MoveObject(position, &desiredPosition);
            *position = desiredPosition;
            return false;
        }
    }
    return false;
}

bool FlierStep(Enemy *enemy)
{
    Vector2 *position = &enemy->position;
    int direction = *enemy->extraInfo;

    Vector2 desiredPosition = *position;
    desiredPosition.x += direction;

    if(AreEqual(&desiredPosition,player.position))
    {
        return true;
    }

    if(isEmpty(desiredPosition.x, desiredPosition.y))
    {
        MoveObject(position, &desiredPosition);
        *position = desiredPosition;
        return Fly(position);
    }


    //change direction
    *enemy->extraInfo *= -1;
    return false;
}
//==================================================


void SpawnJumper()
{
    Vector2 initialPosition = GetRandomEmptyVector();

    FindTheClosestFloor(&initialPosition);

    Enemy jumper = JumperBlueprint;
    jumper.position = initialPosition;

    jumper.extraInfo = new int[4]{1,0,0,true};

    AddEnemy(&jumper);
}

const int maximumDistaceToJump = 5;
const int jumpForceEnemyJumper = 3;
void TryToJump(Enemy *jumper, bool &canJump, Vector2 &velocity)
{
    Vector2 *position = &jumper->position;
    int distaceToPlayer = RoughDistance(position, player.position);

    //cout<<canJump<<" "<<distaceToPlayer;
    if(canJump&&distaceToPlayer <= maximumDistaceToJump&&player.position->y < position->y)
    {
        velocity.y = -jumpForceEnemyJumper;
        canJump = false;
    }
}

bool JumperStep(Enemy *enemy)
{
    Vector2 *position = &enemy->position;
    int direction = *enemy->extraInfo;
    Vector2 velocity = {
        enemy->extraInfo[1],
        enemy->extraInfo[2]
    };
    bool canJump = enemy->extraInfo[3];

    Vector2 desiredPosition = *position;
    desiredPosition.x += direction;

    if(AreEqual(&desiredPosition,player.position))
    {
        return true;
    }

    if(isEmpty(desiredPosition.x, desiredPosition.y))
    {
        MoveObject(position, &desiredPosition);
        *position = desiredPosition;
    }
    else
    {
        //change direction
        direction *= -1;
    }
    TryToJump(enemy, canJump, velocity);

    GravityStep(&vector_down, &velocity, position, (int&)canJump);
    MoveEnemy(&velocity, position);

    enemy->extraInfo[0] = direction;
    enemy->extraInfo[1] = velocity.x;
    enemy->extraInfo[2] = velocity.y;
    enemy->extraInfo[3] = canJump;
    return false;
}
//====================================================
void SpawnBoss()
{
    Enemy boss = BossBlueprint;

    boss.position = GetRandomEmptyVector(bossSize);
    boss.extraInfo = new int[3]{0,0,0};

    AddSpecialEnemy(&boss, bossSize);
    cout<<currentEnemiesAlive;
}

const Animation bossWarning =
{
    1000,
    vector_zero,
    "!"
};

const int numOfWarnings = 20;
bool BossTeleport(int &counter, Vector2 *positionToTP, Enemy *e)
{
    if(counter%10 == 0 && counter<=numOfWarnings)
    {
        Animation warning = bossWarning;
        warning.position = *positionToTP;
        AddAnimation(&warning);

        counter++;
        return false;
    }
    else if(counter>numOfWarnings)
    {
        TeleportSpecial(e, positionToTP, bossSize);
        counter = 0;
        *positionToTP = vector_zero;
        return false;
    }

    counter++;
    return false;
}

bool BossStep(Enemy *enemy)
{
    bool result = false;

    Vector2 chosenPosition =
    {
        enemy->extraInfo[0],
        enemy->extraInfo[1]
    };
    int counter = enemy->extraInfo[2];

    if(!AreEqual(&chosenPosition, &vector_zero))
    {
        result = BossTeleport(counter, &chosenPosition, enemy);
    }
    else
    {
        chosenPosition = *player.position;
        ValidateSpecialPosition(&chosenPosition, bossSize);
    }

    enemy->extraInfo[0] = chosenPosition.x;
    enemy->extraInfo[1] = chosenPosition.y;
    enemy->extraInfo[2] = counter;
    return result;
}
//====================================================

void SpawnEnemy()
{
    int randomEnemy = rand()%enemyTypeCount;
    EnemyTypes enemyType = (EnemyTypes)randomEnemy;

    switch(enemyType)
    {
    case Walker:
        SpawnWalker();
        break;
    case Crawler:
        SpawnCrawler();
        break;
    case Flier:
        SpawnFlier();
        break;
    case Jumper:
        SpawnJumper();
        break;
    }
}

void EnemiesStep()
{
    for(int i = 0; i<currentEnemiesAlive; i++)
    {
        EnemyTypes currentType = enemiesAlive[i].type;
        bool hit = false;

        switch(currentType)
        {
        case EnemyTypes::Walker:
            hit = WalkerStep(enemiesAlive + i);
            break;
        case Crawler:
            hit  = CrawerStep(enemiesAlive + i);
            break;
        case Flier:
            hit = FlierStep(enemiesAlive + i);
            break;
        case Jumper:
            hit = JumperStep(enemiesAlive + i);
            break;
        case Boss:
            hit = BossStep(enemiesAlive+i);
            break;
        }

        if(hit)
        {
            DamagePlayer(1);
            RemoveEnemy(i);
        }
    }
}

void SpawnWave(int numOfEnemies)
{
    for(int i = 0; i<numOfEnemies; i++)
    {
        SpawnEnemy();
    }
}

void DamageEnemy(int enemyIndex, int amount)
{
    Enemy *e = enemiesAlive + enemyIndex;
    e->HP -= amount;

    if(e->HP <= 0)
    {
        //DIE
    }
}
