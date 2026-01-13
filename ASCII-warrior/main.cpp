#include <iostream>
#include <ctime>
#include <conio.h>
#include <cstdlib>
#include <windows.h>

using namespace std;

enum AttackDirections
{
    up,
    down,
    left,
    right
};

struct Vector2
{
    int x;
    int y;
};

enum EnemyTypes
{
    Walker,
    Crawler,
    Flier,
    Jumper,

    Boss
};

//extraInfo:
//(walker): extraInfo[0]-walking direction
struct Enemy
{
    char visual;
    EnemyTypes type;
    int HP;
    Vector2 position;
    int* extraInfo;
};


struct Animation
{
    bool collion;
    int duration;
    Vector2 position;
    char *output;
    int *collisionFunction(int);
};

struct Player
{
    int jumpsLeft;
    int HP;
    Vector2 *position;
    Vector2 verticalMomentum;
};
//========================================================

void PrintHeader();
void CheckForHits(AttackDirections, Vector2);
void RemoveEnemy(int);
void DamageEnemy(int, int);

Player player;

clock_t lastFrameTime = 0;

const int numberOfWaves = 3;
const int initialWaveNumber = 5;

int enemyTypeCount = 4;
int bossTypes = 1;
const int bossSize = 3;

Enemy* enemiesAlive;

const char air = 0;
const char barrier = 1;
const char platform = 2;
const char player_index = 3;

Vector2 vector_down;
Vector2 vector_up;
Vector2 vector_left;
Vector2 vector_right;
Vector2 vector_zero;

unsigned char **board;

const int height = 31;
const int width = 101;
const int numberOfPlatforms = 9;

const int minPlatformSize = 5;
const int maxPlatformSize = 20;



int attackTimer = 0;
int timeBetweenAttacks = 900;

char* attackUp = "/-\\";
char* attackDown = "\\_/";
char* attackLeft = "/\n|\n\\";
char* attackRight = "\\\n|\n/";

Animation ongoingAnimations[10];
int currentTime[10];
int animationCount = 0;

int attackDuration = 300;


const int crawlerInfoSize = 2;
const int crawlerDirectionXIndex = 0;
const int crawlerDirectionYIndex = 1;
const int crawlerDirectionXDefault = 1;
const int crawlerDirectionYDefault = 0;

const int jumpHeight = -5;
const int maxJumps = 2;

const int flierInfoSize = 1;

const int maximumDistaceToJump = 5;
const int jumpForceEnemyJumper = -3;

const int jumperInfoSize = 4;

const int JumperDirectionDefault = 1;
const int JumperVelocityXDefault = 0;
const int JumperVelocityYDefault = 0;
const int JumperCanJumpDefault = 1;
const int JumperDirectionIndex = 0;
const int JumperVelocityXIndex = 1;
const int JumperVelocityYIndex = 2;
const int JumperCanJumpIndex = 3;

int enemyIndexOffset = 5;

const int playerDMG = 1;

const Animation baseAnimation =
{
    false,
    0,
    vector_zero,
    nullptr
};

//======================================================================================


Animation *CreateAnimation(Vector2 position, char* out, int duration)
{
    Animation *finished = new Animation;

    *finished ={
        false,
        duration,
        position,
        out
    };

    return finished;
}

Animation CreateAnimationValue(Vector2 position, char* out, int duration)
{
    return {
        false,
        duration,
        position,
        out
    };
}

bool AreEqual(Vector2 *v1, Vector2 *v2)
{
    return v1->x==v2->x && v1->y==v2->y;
}

Vector2* AddVectors(Vector2 *v1, Vector2 *v2)
{
    Vector2 *result = new Vector2;

    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;

    return result;
}

void AddVectorsDirectly(Vector2 *v1, Vector2 *v2)
{
    v1->x += v2->x;
    v1->y += v2->y;
}

void  ZeroVector(Vector2 *v)
{
    v->x = 0;
    v->y = 0;
}

int Abs(int);
int RoughDistance(Vector2 *v1, Vector2 *v2)
{
    return Abs(v1->x-v2->x) + Abs(v1->y-v2->y);
}

//======================================================================================

void PrintObject(char objectToPrint)
{
    switch(objectToPrint)
    {
    case air:
        cout<<' ';
        break;
    case barrier:
        cout<<'#';
        break;
    case platform:
        cout<<'=';
        break;
    case player_index:
        cout<<'@';
        break;
    }

    if(objectToPrint>4)
    {
        cout<<enemiesAlive[objectToPrint-enemyIndexOffset].visual;
    }
}

//======================================================================================

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

int GenerateRandom(int minimum, int maximum)
{
    return minimum + (rand()%(maximum - minimum+1));
}

void putPlatforms()
{
    //why not random???
    for(int i = 0; i < numberOfPlatforms; i++)
    {
        int x = GenerateRandom(1, width-2);
        int y = GenerateRandom(1, height-2);

        int platformSize = GenerateRandom(minPlatformSize, maxPlatformSize);

        for(int j = 0; j<platformSize;j++)
        {
            if(x + j >= width-1)
            {
                break;
            }
            board[x + j][y] = platform;
        }
    }
}

void GenerateMap()
{
    ClearBoard();
    PutBorrder();
    putPlatforms();
}


bool isEmpty(int x, int y)
{
    return !board[x][y];
}

bool isEmpty(Vector2 *position)
{
    return !board[position->x][position->y];
}

bool isEmptyOrEnemy(int x, int y)
{
    return board[x][y] != platform && board[x][y] != barrier;
}

bool canStand(Vector2 position)
{
    Vector2 tileUnder =position;
    AddVectorsDirectly(&tileUnder, &vector_down);

    return !isEmptyOrEnemy(tileUnder.x, tileUnder.y)&&isEmpty(position.x, position.y);
}

bool canStandorHang(Vector2 position)
{
    Vector2 tileUp = position;
    AddVectorsDirectly(&tileUp, &vector_up);

    Vector2 tileDown = position;
    AddVectorsDirectly(&tileDown, &vector_down);

    return (!isEmptyOrEnemy(tileUp.x, tileUp.y)||
            !isEmptyOrEnemy(tileDown.x, tileDown.y))&&
            isEmpty(position.x, position.y);
}

bool canCrawl(Vector2 position)
{
    Vector2 tileLeft = position;
    AddVectorsDirectly(&tileLeft,&vector_left);

    Vector2 tileRight = position;
    AddVectorsDirectly(&tileRight,&vector_right);

    return (!isEmptyOrEnemy(tileLeft.x, tileLeft.y)||
            !isEmptyOrEnemy(tileRight.x, tileRight.y))&&
            isEmpty(position.x, position.y);
}

unsigned char isEnemy(Vector2 *position)
{
    return board[position->x][position->y]>=enemyIndexOffset ? board[position->x][position->y]:0;
}

void PrintObject(Vector2 *position)
{
    PrintObject(board[position->x][position->y]);
}

void RemoveObject(Vector2 *position)
{
    int x = position->x;
    int y = position->y;

    board[x][y] = air;

    SetCursorPosition(position);
    cout<<' ';
}

void DecreaseValueByOne(Vector2 *position)
{
    board[position->x][position->y]--;
}

//======================================================================================

void RemoveAnimation(int animationIndex)
{
    Animation *animationToRemove = &ongoingAnimations[animationIndex];

    Vector2 currentPosition = animationToRemove->position;

    SetCursorPosition(&currentPosition);

    char* animationOutput = animationToRemove->output;

    while(*animationOutput)
    {
        if(*animationOutput =='\n')
        {
            currentPosition.y++;
            currentPosition.x--;
            SetCursorPosition(&currentPosition);
            animationOutput++;
            continue;
        }

        PrintObject(&currentPosition);
        currentPosition.x++;
        animationOutput++;
    }

    for(int i = animationIndex+1; i<animationCount;i++)
    {
        ongoingAnimations[i-1] = ongoingAnimations[i];
        currentTime[i-1] = currentTime[i];
    }

    delete[] animationToRemove;
    animationCount--;
}

void NewLine(Vector2 *currentPosition)
{

    currentPosition->y++;
    currentPosition->x--;
    SetCursorPosition(currentPosition);
}

void PrintAnimation(Animation *animation)
{
    Vector2 currentPosition = animation->position;

    SetCursorPosition(&currentPosition);

    char* animationOutput = animation->output;

    while(*animationOutput)
    {
        if(*animationOutput =='\n')
        {
            NewLine(&currentPosition);
            animationOutput++;
            continue;
        }

        cout<<*animationOutput;
        currentPosition.x++;
        animationOutput++;
    }
}

void AddAnimation(Animation *animation)
{
    ongoingAnimations[animationCount] = *animation;
    currentTime[animationCount] = 0;

    PrintAnimation(&ongoingAnimations[animationCount]);

    animationCount++;
}


void tryAttacking(AttackDirections attack, Player *playerObject)
{
    if(attackTimer < timeBetweenAttacks)
    {
        return;
    }
    attackTimer = 0;

    char* animationOutput;
    Vector2 animationPosition;
    ZeroVector(&animationPosition);

    AddVectorsDirectly(&animationPosition, playerObject->position);

    switch(attack)
    {
        case AttackDirections::up:
            AddVectorsDirectly(&animationPosition, &vector_up);
            AddVectorsDirectly(&animationPosition, &vector_left);
            animationOutput = attackUp;
            break;
        case AttackDirections::down:
            AddVectorsDirectly(&animationPosition, &vector_down);
            AddVectorsDirectly(&animationPosition, &vector_left);
            animationOutput = attackDown;
            break;
        case AttackDirections::left:
            AddVectorsDirectly(&animationPosition, &vector_up);
            AddVectorsDirectly(&animationPosition, &vector_left);
            animationOutput = attackLeft;
            break;
        case AttackDirections::right:
            AddVectorsDirectly(&animationPosition, &vector_up);
            AddVectorsDirectly(&animationPosition, &vector_right);
            animationOutput = attackRight;
            break;

    }

    Animation ann = CreateAnimationValue(animationPosition, animationOutput,attackDuration);
    AddAnimation(&ann);
}

void ExecuteAnimationCollision(Animation *animation, int colidedObject)
{
    (*animation).collisionFunction(colidedObject);
}

void CheckCollision(Animation *currentAnimation)
{
    Vector2 currentPosition = currentAnimation->position;
    char* animationOutput = currentAnimation->output;

    while(*animationOutput)
    {
        if(*animationOutput =='\n')
        {
            NewLine(&currentPosition);
            animationOutput++;
            continue;
        }

        if(*animationOutput != air)
        {
            ExecuteAnimationCollision(currentAnimation, *animationOutput);
        }
        currentPosition.x++;
        animationOutput++;
    }
}

void AnimationCollisionCheck()
{
    for(int i = 0; i<animationCount; i++)
    {
        Animation *currentAnimation = ongoingAnimations + i;

        if(currentAnimation->collion)
        {
            CheckCollision(currentAnimation);
        }
    }
}

void AnimatinStep(int delta)
{
    attackTimer+=delta;
    for(int i = 0; i<animationCount; i++)
    {
        currentTime[i]+=delta;
        Animation *currentAnimation = ongoingAnimations + i;

        if(currentTime[i] > currentAnimation->duration)
        {
            RemoveAnimation(i);
        }
    }
}

//======================================================================================

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

bool CollideWithEnemy(int);
void DamagePlayer(int);

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
            bool canGo = CollideWithEnemy(boardValue-5);
            DamagePlayer(1);
            if(!canGo)
            {
                delete[] playerMove;
                return 1;
            }
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

//=====================================================

void AdjustVectorToSize(Vector2 *v, int Size)
{
    for(int i = 0; i<Size/2; i++)
    {
        AddVectorsDirectly(v, &vector_up);
        AddVectorsDirectly(v, &vector_left);
    }
}

bool IsEmpty_Boss(Vector2 position, int Size)
{
    Vector2 current = position;

    int xDirection = 1;
    int yDirection = 1;

    AdjustVectorToSize(&current, Size);

    for(int i = 0; i < Size; i++)
    {
        for(int j = 0; j<Size;j++)
        {
            if(!isEmpty(&current))return false;

            current.x += xDirection;
        }

        xDirection *= -1;
        current.x += xDirection;
        current.y += yDirection;
    }

    return true;
}

void ValidateSpecialPosition(Vector2 *position, int Size)
{
    int halfSize = Size/2;

    if(position->x < 2+halfSize)
    {
        position->x = 2+halfSize;
    }
    else if(position->x > width-2-halfSize)
    {
        position->x = width-2-halfSize;
    }

    if(position->y < 2+halfSize)
    {
        position->y = 2+halfSize;
    }
    else if(position->y > height-2-halfSize)
    {
        position->y = height-2-halfSize;
    }
}

void TeleportSpecial(Enemy *e, Vector2 *newPosition, int Size)
{
    ValidateSpecialPosition(newPosition, Size);

    Vector2 moveFrom = e->position;
    Vector2 moveTo = *newPosition;

    e->position = moveTo;

    AdjustVectorToSize(&moveFrom, Size);
    AdjustVectorToSize(&moveTo, Size);

    int xDirection = 1;
    int yDirection = 1;

    for(int i = 0; i < Size; i++)
    {
        for(int j = 0; j<Size;j++)
        {
            MoveObject(&moveFrom, &moveTo);

            moveFrom.x += xDirection;
            moveTo.x += xDirection;
        }

        xDirection *= -1;
        moveFrom.x += xDirection;
        moveTo.x += xDirection;

        moveFrom.y += yDirection;
        moveTo.y += yDirection;
    }
}

//======================================================================================

int RoundUp(double number)
{
    int integerPart = (int)number;

    return (number-integerPart)>0? integerPart+1: integerPart;
}




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
const int walkerInfoSize = 1;
const int walkerDeffaultDirection = 1;

void SpawnWalker()
{
    Vector2 initialPosition = GetRandomEmptyVector();

    FindTheClosestFloor(&initialPosition);

    Enemy walker = WalkerBlueprint;
    walker.position = initialPosition;
    walker.extraInfo = new int(walkerDeffaultDirection);
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
    crawler.extraInfo = new int[crawlerInfoSize]{crawlerDirectionXDefault,crawlerDirectionYDefault};
    AddEnemy(&crawler);
}

void CrawlerGetNewDirection(Enemy *enemy)
{
    Vector2 position = enemy->position;
    Vector2 direction =
    {
        enemy->extraInfo[crawlerDirectionXIndex],
        enemy->extraInfo[crawlerDirectionYIndex]
    };

    if(direction.x)
    {
        Vector2 testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_up);
        if(canCrawl(testPosition))
        {
            enemy->extraInfo[crawlerDirectionXIndex] = vector_up.x;
            enemy->extraInfo[crawlerDirectionYIndex] = vector_up.y;
            return;
        }

        testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_down);
        if(canCrawl(testPosition))
        {
            enemy->extraInfo[crawlerDirectionXIndex] = vector_down.x;
            enemy->extraInfo[crawlerDirectionYIndex] = vector_down.y;
            return;
        }
    }
    else
    {
        Vector2 testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_left);
        if(canStandorHang(testPosition))
        {
            enemy->extraInfo[crawlerDirectionXIndex] = vector_left.x;
            enemy->extraInfo[crawlerDirectionYIndex] = vector_left.y;
            return;
        }


        testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_right);
        if(canStandorHang(testPosition))
        {
            enemy->extraInfo[crawlerDirectionXIndex] = vector_right.x;
            enemy->extraInfo[crawlerDirectionYIndex] = vector_right.y;
            return;
        }
    }

    enemy->extraInfo[crawlerDirectionXIndex] *= -1;
    enemy->extraInfo[crawlerDirectionYIndex] *= -1;
}

bool CrawerStep(Enemy *enemy)
{
    Vector2 *position = &enemy->position;
    Vector2 direction =
    {
        enemy->extraInfo[crawlerDirectionXIndex],
        enemy->extraInfo[crawlerDirectionYIndex]
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
    flier.extraInfo = new int(flierInfoSize);
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

    jumper.extraInfo = new int[jumperInfoSize]{JumperDirectionDefault,JumperVelocityXDefault,JumperVelocityYDefault,JumperCanJumpDefault};

    AddEnemy(&jumper);
}

void TryToJump(Enemy *jumper, bool &canJump, Vector2 &velocity)
{
    Vector2 *position = &jumper->position;
    int distaceToPlayer = RoughDistance(position, player.position);

    //cout<<canJump<<" "<<distaceToPlayer;
    if(canJump&&distaceToPlayer <= maximumDistaceToJump&&player.position->y < position->y)
    {
        velocity.y = jumpForceEnemyJumper;
        canJump = false;
    }
}


bool JumperStep(Enemy *enemy)
{
    Vector2 *position = &enemy->position;
    int direction = enemy->extraInfo[JumperDirectionIndex];
    Vector2 velocity = {
        enemy->extraInfo[JumperVelocityXIndex],
        enemy->extraInfo[JumperVelocityYIndex]
    };
    bool canJump = enemy->extraInfo[JumperCanJumpIndex];

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

    enemy->extraInfo[JumperDirectionIndex] = direction;
    enemy->extraInfo[JumperVelocityXIndex] = velocity.x;
    enemy->extraInfo[JumperVelocityYIndex] = velocity.y;
    enemy->extraInfo[JumperCanJumpIndex] = canJump;
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


const int bossWarningDuration = 1000;
const int numOfWarnings = 20;
bool BossTeleport(int &counter, Vector2 *positionToTP, Enemy *e)
{
    if(counter%10 == 0 && counter<=numOfWarnings)
    {
        Animation warning = baseAnimation;
        warning.duration = bossWarningDuration;
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
        RemoveEnemy(enemyIndex);
    }
}

//======================================================================================

void AddPlayer()
{
    player.HP = 5;

    Vector2 *playerPosition = new Vector2;

    player.position = playerPosition;

    int X = (int)(width/2) +1;
    int Y = (int)(height/2) +1;

    playerPosition->x = X;
    playerPosition->y = Y;

    board[X][Y] = player_index;
}

void PrintHeader()
{
    Vector2 v = {0,-1};

    SetCursorPosition(&v);
    cout<<"HP: "<<player.HP;
    cout<<endl;
}

void PrintFullBoard()
{
    PrintHeader();

    for(int y = 0; y<height; y++)
    {
        for(int x = 0; x<width; x++)
        {
            PrintObject(board[x][y]);
        }
        cout<<endl;
    }
}

char GetInput()
{
    //TODO better
    if(_kbhit())
    {
        return _getch();
    }

    return 0;
}

float GetDeltaTime()
{
    clock_t current_time = clock();

    //time between the last frame and the new one
    float delta = current_time-lastFrameTime;

    lastFrameTime = current_time;

    return delta;
}

void GameSetup()
{
    board = new unsigned char*[width];

    for(int i = 0; i < width; i++)
    {
        board[i] = new unsigned char[height];
    }
    GenerateMap();
    AddPlayer();

    enemiesAlive = new Enemy[maxEnemiesAllowed];

    vector_down.x = 0;
    vector_down.y = 1;
    vector_left.x = -1;
    vector_left.y = 0;
    vector_right.x = 1;
    vector_right.y = 0;
    vector_up.x = 0;
    vector_up.y = -1;
    vector_zero.x = 0;
    vector_zero.y = 0;
}

void InputManager(char input)
{
    if(input == 0) return;

    if(input == 'a')
    {
        MovePlayer(&vector_left, player.position);
    }
    else if(input == 'd')
    {
        MovePlayer(&vector_right, player.position);
    }
    else if(input == 'w')
    {
        PlayerJump(&player);
    }
    else if(input == 'i')
    {
        tryAttacking(AttackDirections::up, &player);
    }
    else if(input == 'k')
    {
        tryAttacking(AttackDirections::down, &player);
    }
    else if(input == 'l')
    {
        tryAttacking(AttackDirections::right, &player);
    }
    else if(input == 'j')
    {
        tryAttacking(AttackDirections::left, &player);
    }
}

void GameLoop()
{
    PrintFullBoard();
    float timer = 0;
    timer += GetDeltaTime();

    while(true)
    {
        char input = GetInput();
        InputManager(input);

        GravityStep(&vector_down, &player.verticalMomentum, player.position, player.jumpsLeft);
        MovePlayer(&player.verticalMomentum, player.position);

        EnemiesStep();
        AnimationCollisionCheck();

        if(currentEnemiesAlive == 0)break;

        while(timer<=150)
        {
            float delta = GetDeltaTime();
            timer += delta;

            AnimatinStep(delta);
        }
        timer = 0;
    }
}

int main()
{
    system("MODE CON COLS=101 LINES=40");
    GameSetup();

    int curWaveNumber = initialWaveNumber;

    for(int i = 0; i<numberOfWaves; i++)
    {
        SpawnWave(initialWaveNumber);

        GameLoop();

        curWaveNumber += GenerateRandom(3,5);
    }
    return 0;
}
