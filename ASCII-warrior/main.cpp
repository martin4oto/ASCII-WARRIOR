#include <iostream>
#include <ctime>
#include <conio.h>
#include <cstdlib>
#include <windows.h>
#include <climits>

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

    Crusher,
    Shooter
};

enum Color {
    COLOR_BLACK = 0,
    COLOR_DARK_BLUE = 1,
    COLOR_DARK_GREEN = 2,
    COLOR_DARK_CYAN = 3,
    COLOR_DARK_RED = 4,
    COLOR_DARK_MAGENTA = 5,
    COLOR_DARK_YELLOW = 6,
    COLOR_GRAY = 7,
    COLOR_DARK_GRAY = 8,
    COLOR_BLUE = 9,
    COLOR_GREEN = 10,
    COLOR_CYAN = 11,
    COLOR_RED = 12,
    COLOR_MAGENTA = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15
};

//extraInfo:
//(walker): extraInfo[0]-walking direction
struct Enemy
{
    char visual;
    EnemyTypes type;
    int HP;
    int Size;
    Color color;
    Vector2 position;
    int* extraInfo;
};


struct Animation
{
    bool collion;
    Color color;
    int duration;
    char* output;
    void (*collisionFunction)(int);
    Vector2 position;
};

struct Player
{
    int jumpsLeft;
    int HP;
    Vector2* position;
    Vector2 verticalMomentum;
};
//========================================================

void PrintHeader();
void CheckForHits(AttackDirections, Vector2);
void RemoveEnemy(int);
void DamageEnemy(int, int);
bool CollideWithEnemy(int);
void DamagePlayer(int);
void PrintFullBoard();
void AddDiagonalShootingAnimation(Enemy*, char);
void AddLineShootingAnimations(Enemy*, char);
void CleanShooter(Enemy*);
int Abs(int);
unsigned char TranslateObjectToEnemy(unsigned char);

Player player;

clock_t lastFrameTime = 0;

const int numberOfWaves = 3;
const int initialWaveNumber = 5;

const int enemyTypeCount = 4;
const int bossTypeCount = 2;

Enemy* enemiesAlive = nullptr;

const char air = 0;
const char barrier = 1;
const char platform = 2;
const char player_index = 3;

Vector2 vector_down;
Vector2 vector_up;
Vector2 vector_left;
Vector2 vector_right;
Vector2 vector_zero;

unsigned char** board = nullptr;

const int height = 21;
const int width = 101;
const int numberOfPlatforms = 9;

const int minPlatformSize = 5;
const int maxPlatformSize = 20;

int attackTimer = 0;
int timeBetweenAttacks = 900;

const char* attackUp = "/-\\";
const char* attackDown = "\\_/";
const char* attackLeft = "/\n|\n\\";
const char* attackRight = "\\\n|\n/";
const int attackCollisionDuration = 370;

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

const int frameDelay = 150;

const Animation baseAnimation =
{
    false,
    COLOR_WHITE,
    0,
    nullptr,
    nullptr,
    vector_zero
};

short* shootingStringSizes = nullptr;

const int maxEnemiesAllowed = 64;
int currentEnemiesAlive = 0;

bool lost = false;

//======================================================================================

/*
  CreateAnimation
  Create a heap-allocated Animation object and initialize its fields.
  Returns:
   - pointer to newly allocated Animation, or nullptr on allocation failure
 */
Animation* CreateAnimation(Color color,Vector2 position, char* out, int duration)
{
    // allocate new animation
    Animation* finished = new Animation;
    if (!finished) return nullptr;

    *finished = {
        false,
        color,
        duration,
        out,
        nullptr,
        position
    };

    return finished;
}

/*
  CreateAnimationValue
  Create Animation by value (stack) for immediate use.
  Parameters are same as CreateAnimation.
 */
Animation CreateAnimationValue(Color color, Vector2 position, char* out, int duration)
{
    return {
        false,
        color,
        duration,
        out,
        nullptr,
        position
    };
}

/*
 AreEqual
 Compare two Vector2 pointers.
 Returns false if either pointer is null.
 */
bool AreEqual(Vector2* v1, Vector2* v2)
{
    if (!v1 || !v2) return false;
    // if two vectors are equal
    return v1->x == v2->x && v1->y == v2->y;
}

/*
  AddVectors
  Return a new Vector2 allocated on the heap which is v1 + v2.
  If either pointer is null returns nullptr.
*/
Vector2* AddVectors(Vector2* v1, Vector2* v2)
{
    if (!v1 || !v2) return nullptr;

    Vector2* result = new Vector2;
    if (!result) return nullptr;

    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;

    return result;
}

/*
  AddVectorsDirectly
  In-place addition: v1 += v2.
  If either pointer is null the function returns immediately.
*/
void AddVectorsDirectly(Vector2* v1, Vector2* v2)
{
    if (!v1 || !v2) return;
    v1->x += v2->x;
    v1->y += v2->y;
}

/*
  ZeroVector
  Set vector to zero if pointer valid.
 */
void ZeroVector(Vector2* v)
{
    if (!v) return;
    v->x = 0;
    v->y = 0;
}

/*
  RoughDistance
  Manhattan distance between two points.
  If either pointer is null returns INT_MAX.
 */
int RoughDistance(Vector2* v1, Vector2* v2)
{
    if (!v1 || !v2) return INT_MAX;
    return Abs(v1->x - v2->x) + Abs(v1->y - v2->y);
}

//======================================================================================

//set console color
void setColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

Color wallColor = COLOR_DARK_GRAY;
Color playerColor = COLOR_WHITE;
Color platformColor = COLOR_WHITE;

/*
  PrintObject
  Map board values to console characters and print.
  objectToPrint is a board tile value, not a pointer.
 */
void PrintObject(char objectToPrint)
{
    switch (objectToPrint)
    {
    case air:
        cout << ' ';
        break;
    case barrier:
        setColor(wallColor);
        cout << '#';
        break;
    case platform:
        setColor(platformColor);
        cout << '=';
        break;
    case player_index:
        setColor(playerColor);
        cout << '@';
        break;
    }

    if (objectToPrint > 4)
    {
        int enemyIndex = TranslateObjectToEnemy(objectToPrint);
        // validate enemy bounds
        if (enemyIndex >= 0)
        {
            setColor(enemiesAlive[enemyIndex].color);

            cout << enemiesAlive[enemyIndex].visual;
        }
    }
}

//======================================================================================

/*
  SetCursorPosition
  Set console cursor position. No-op if position is null.
*/
void SetCursorPosition(Vector2* position)
{
    if (!position) return;

    // stdHandle
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    // create COORD to set the console
    COORD coordinates =
    {
         (SHORT)(position->x),
         (SHORT)(position->y) + 1
    };

    // set position
    SetConsoleCursorPosition(hStdOut, coordinates);
}

/*
  ClearBoard
  Set all board cells to air. No-op if board not allocated.
*/
void ClearBoard()
{
    if (!board) return;

    for (int i = 0; i < width; i++)
    {
        if (!board[i]) continue;
        for (int j = 0; j < height; j++)
        {
            board[i][j] = 0;
        }
    }
}

/*
  PutBorrder
  Draw border tiles into board. No-op if board is not allocated.
*/
void PutBorrder()
{
    if (!board) return;

    // top and bottom
    for (int i = 0; i < width; i++)
    {
        if (board[i]) board[i][0] = 1;
        if (board[i]) board[i][height - 1] = 1;
    }

    // left and right
    for (int i = 0; i < height; i++)
    {
        if (board[0]) board[0][i] = 1;
        if (board[width - 1]) board[width - 1][i] = 1;
    }
}

/*
  GenerateRandom
  Returns a random integer in [minimum, maximum]. Assumes minimum <= maximum.
*/
int GenerateRandom(int minimum, int maximum)
{
    if(minimum>maximum)return 0;
    if (maximum < minimum) return minimum;
    return minimum + (rand() % (maximum - minimum + 1));
}

/*
  putPlatforms
  Fill randomly placed platforms into the board. No-op if board is not allocated.
*/
void putPlatforms()
{
    if (!board) return;

    for (int i = 0; i < numberOfPlatforms; i++)
    {
        // random starting position (not at the edge)
        int x = GenerateRandom(1, width - 2);
        int y = GenerateRandom(1, height - 2);

        // random size
        int platformSize = GenerateRandom(minPlatformSize, maxPlatformSize);

        for (int j = 0; j < platformSize; j++)
        {
            if (x + j >= width - 1)
            {
                break;
            }
            if (board[x + j]) board[x + j][y] = platform;
        }
    }
}

void GenerateMap()
{
    ClearBoard();
    PutBorrder();
    putPlatforms();
}

unsigned char BoardValue(const Vector2* position)
{
    if (!position) return air;
    if (position->x < 0 || position->x >= width) return air;
    if (position->y < 0 || position->y >= height) return air;

    if (!board) return air;
    if (!board[position->x]) return air;

    return board[position->x][position->y];
}

bool isEmpty(int x, int y)
{
    if (!board) return false;
    if (x < 0 || x >= width) return false;
    if (y < 0 || y >= height) return false;
    if (!board[x]) return false;

    return !board[x][y];
}

bool ValidateVectorForBoard(Vector2* position)
{
    if (!position) return false;
    if (position->x < 0 || position->x >= width) return false;
    if (position->y < 0 || position->y >= height) return false;
    if (!board) return false;
    if (!board[position->x]) return false;

    return true;
}

bool isEmpty(Vector2* position)
{
    if (!ValidateVectorForBoard(position)) return false;
    return !board[position->x][position->y];
}

bool isEmptyOrEnemy(int x, int y)
{
    if (!board) return false;
    if (x < 0 || x >= width) return false;
    if (y < 0 || y >= height) return false;
    if (!board[x]) return false;

    return board[x][y] != platform && board[x][y] != barrier;
}

bool canStand(Vector2 position)
{
    if (!ValidateVectorForBoard(&position)) return false;

    Vector2 tileUnder = position;
    AddVectorsDirectly(&tileUnder, &vector_down);

    return !isEmptyOrEnemy(tileUnder.x, tileUnder.y) && isEmpty(&position);
}

bool canStandorHang(Vector2 position)
{
    if (!ValidateVectorForBoard(&position)) return false;

    // generate tile above
    Vector2 tileUp = position;
    AddVectorsDirectly(&tileUp, &vector_up);

    // generate tile below
    Vector2 tileDown = position;
    AddVectorsDirectly(&tileDown, &vector_down);

    // if the square is empty and there is something over it or below it
    return (!isEmptyOrEnemy(tileUp.x, tileUp.y) ||
        !isEmptyOrEnemy(tileDown.x, tileDown.y)) &&
        isEmpty(&position);
}

bool canCrawl(Vector2 position)
{
    // generate the tile on the left
    Vector2 tileLeft = position;
    AddVectorsDirectly(&tileLeft, &vector_left);

    // generate the tile on the right
    Vector2 tileRight = position;
    AddVectorsDirectly(&tileRight, &vector_right);

    // there is something on the left or the right side
    return (!isEmptyOrEnemy(tileLeft.x, tileLeft.y) ||
        !isEmptyOrEnemy(tileRight.x, tileRight.y)) &&
        isEmpty(&position);
}

unsigned char isObjectEnemy(unsigned char object)
{
    return object >= enemyIndexOffset ? object : 0;
}

unsigned char TranslateObjectToEnemy(unsigned char object)
{
    return isObjectEnemy(object) ? object - enemyIndexOffset : 0;
}

bool isBoss(unsigned char index)
{
    if (!enemiesAlive) return false;
    if (index < 0) return false;
    // bounds check: index must be < currentEnemiesAlive
    if (index >= currentEnemiesAlive) return false;

    Enemy* enemyToCheck = &enemiesAlive[index];

    int enemyType = enemyToCheck->type;
    if (enemyType >= enemyTypeCount)
    {
        return true;
    }
    return false;
}

unsigned char isEnemy(Vector2* position)
{
    if (!ValidateVectorForBoard(position)) return 0;
    return isObjectEnemy(board[position->x][position->y]);
}

/*
  PrintObject (overload)
  Print tile at position; returns if position invalid.
 */
void PrintObject(Vector2* position)
{
    if (!ValidateVectorForBoard(position)) return;
    PrintObject(board[position->x][position->y]);
}

void RemoveObject(Vector2* position)
{
    if (!ValidateVectorForBoard(position)) return;

    int x = position->x;
    int y = position->y;

    board[x][y] = air;

    SetCursorPosition(position);
    cout << ' ';
}

void DecreaseValueByOne(Vector2* position)
{
    if (!ValidateVectorForBoard(position)) return;
    board[position->x][position->y]--;
}

void VisualizePlayer()
{
    if (!player.position || !ValidateVectorForBoard(player.position)) return;

    Vector2* position = player.position;

    board[position->x][position->y] = player_index;

    SetCursorPosition(position);

    PrintObject(position);
}

int minimum(int a, int b)
{
    return a >= b ? b : a;
}

//======================================================================================

/*
  NewLine
  Move currentPosition to the next visual text row while printing animations.
  Validates currentPosition.
*/
void NewLine(Vector2* currentPosition, int lineBeginingX)
{
    if (!ValidateVectorForBoard(currentPosition)) return;

    // going to the new line for animations
    currentPosition->y++;
    currentPosition->x = lineBeginingX;
    SetCursorPosition(currentPosition);
}

/*
  RemoveAnimation
  Remove animation at index animationIndex.
  Validates index and animation output pointers.
*/
void RemoveAnimation(int animationIndex)
{
    if (animationIndex < 0 || animationIndex >= animationCount) return;

    Animation* animationToRemove = &ongoingAnimations[animationIndex];

    Vector2 currentPosition = animationToRemove->position;
    int lineBeginingX = currentPosition.x;

    // setting the position where we have to print the animation
    SetCursorPosition(&currentPosition);

    const char* animationOutput = animationToRemove->output;
    if (!animationOutput)
    {
        // still need to shift array entries
        for (int i = animationIndex + 1; i < animationCount; i++)
        {
            ongoingAnimations[i - 1] = ongoingAnimations[i];
            currentTime[i - 1] = currentTime[i];
        }
        animationCount--;
        return;
    }

    while (*animationOutput)
    {
        if (*animationOutput == '\n')
        {
            NewLine(&currentPosition, lineBeginingX);
            animationOutput++;
            continue;
        }

        PrintObject(&currentPosition);
        // adding to the position
        currentPosition.x++;
        animationOutput++;
    }

    for (int i = animationIndex + 1; i < animationCount; i++)
    {
        // tidy the list
        ongoingAnimations[i - 1] = ongoingAnimations[i];
        currentTime[i - 1] = currentTime[i];
    }

    animationCount--;
}

/*
  PrintAnimation
  Render animation to console. No-op if animation null or output null.
*/
void PrintAnimation(Animation* animation)
{
    if (animation == nullptr) return;
    if (!animation->output) return;


    Vector2 currentPosition = animation->position;
    int lineBeginingX = currentPosition.x;

    SetCursorPosition(&currentPosition);

    const char* animationOutput = animation->output;

    while (*animationOutput)
    {
        if (*animationOutput == '\n')
        {
            NewLine(&currentPosition, lineBeginingX);
            animationOutput++;
            continue;
        }

        if (*animationOutput != ' ')
        {
            setColor(animation->color);
            cout << *animationOutput;
        }
        else
        {
            // skip if the animation at this position is empty
            PrintObject(&currentPosition);
        }
        currentPosition.x++;
        animationOutput++;
    }
}

/*
  AddAnimation
  Add animation to ongoingAnimations list. Requires valid pointer.
*/
void AddAnimation(Animation* animation)
{
    if (!animation) return;
    if (animationCount < 0 || animationCount >= (int)(sizeof(ongoingAnimations) / sizeof(ongoingAnimations[0]))) return;

    ongoingAnimations[animationCount] = *animation;
    currentTime[animationCount] = 0;

    if (!ongoingAnimations[animationCount].collion)
    {
        PrintAnimation(&ongoingAnimations[animationCount]);
    }
    animationCount++;
}

/*
  GenerateAttackPosition
  Generate initial animation position and provide pointer to ascii art.
  Validates animationPosition and animationOutput pointer.
*/
void GenerateAttackPosition(AttackDirections attackDirection, Vector2* animationPosition, const char** animationOutput)
{
    if (!animationPosition || !animationOutput) return;
    if (!ValidateVectorForBoard(animationPosition)) return;

    switch (attackDirection)
    {
    case AttackDirections::up:
        AddVectorsDirectly(animationPosition, &vector_up);
        AddVectorsDirectly(animationPosition, &vector_left);
        *animationOutput = attackUp;
        break;
    case AttackDirections::down:
        AddVectorsDirectly(animationPosition, &vector_down);
        AddVectorsDirectly(animationPosition, &vector_left);
        *animationOutput = attackDown;
        break;
    case AttackDirections::left:
        AddVectorsDirectly(animationPosition, &vector_up);
        AddVectorsDirectly(animationPosition, &vector_left);
        *animationOutput = attackLeft;
        break;
    case AttackDirections::right:
        AddVectorsDirectly(animationPosition, &vector_up);
        AddVectorsDirectly(animationPosition, &vector_right);
        *animationOutput = attackRight;
        break;

    default:
        *animationOutput = nullptr;
        break;
    }
}

// collision for attack animations
void AttackCollisionFunction(int object)
{
    if (isObjectEnemy(object))
    {
        DamageEnemy(TranslateObjectToEnemy(object), playerDMG);
    }
}

/*
  BuildCollision
  Create a collision-enabled copy of animation. Validates inputs.
*/
void BuildCollision(Animation* animation, void (*collisionFunction)(int), int duration)
{
    if (!animation || !collisionFunction) return;

    Animation newAnimation = *animation;
    newAnimation.collion = true;
    newAnimation.collisionFunction = collisionFunction;
    newAnimation.duration = duration;

    AddAnimation(&newAnimation);
}

const Color attackColor = COLOR_WHITE;

/*
  tryAttacking
  Player attack entry point. Validates playerObject pointer.
*/
void tryAttacking(AttackDirections attack, Player* playerObject)
{
    if (!playerObject || !playerObject->position) return;

    if (attackTimer < timeBetweenAttacks)
    {
        return;
    }
    attackTimer = 0;

    const char* animationOutput = nullptr;
    Vector2 animationPosition = *playerObject->position;

    GenerateAttackPosition(attack, &animationPosition, &animationOutput);

    Animation ann = CreateAnimationValue(attackColor,animationPosition, (char*)animationOutput, attackDuration);
    AddAnimation(&ann);

    // make the attacks collide
    BuildCollision(&ann, AttackCollisionFunction, attackCollisionDuration);
}

/*
  ExecuteAnimationCollision
  Call collision function for animation. Validates parameters.
*/
void ExecuteAnimationCollision(Animation* animation, int colidedObject)
{
    if (!animation || !animation->collisionFunction) return;
    animation->collisionFunction(colidedObject);
}

/*
  CheckCollision
  Walk animation output and invoke collision function where appropriate.
*/
void CheckCollision(Animation* currentAnimation)
{
    if (!currentAnimation) return;
    if (!currentAnimation->output) return;
    if (!currentAnimation->collisionFunction) return;

    Vector2 currentPosition = currentAnimation->position;
    const char* animationOutput = currentAnimation->output;
    int lineBeginingX = currentPosition.x;

    while (*animationOutput)
    {
        if (*animationOutput == '\n')
        {
            NewLine(&currentPosition, lineBeginingX);
            animationOutput++;
            continue;
        }

        if (*animationOutput != ' ')
        {
            // start collision function
            ExecuteAnimationCollision(currentAnimation, BoardValue(&currentPosition));
        }
        currentPosition.x++;
        animationOutput++;
    }
}

/*
  AnimationCollisionCheck
  Check collisions for all collision-enabled animations.
*/
void AnimationCollisionCheck()
{
    for (int i = 0; i < animationCount; i++)
    {
        Animation* currentAnimation = ongoingAnimations + i;

        if (currentAnimation->collion)
        {
            // start collision check
            CheckCollision(currentAnimation);
        }
    }
}

/*
  AnimatinStep
  Increment timers and remove expired animations safely.
*/
void AnimatinStep(int delta)
{
    attackTimer += delta;
    for (int i = 0; i < animationCount; i++)
    {
        currentTime[i] += delta;
        Animation* currentAnimation = ongoingAnimations + i;

        if (currentTime[i] > currentAnimation->duration)
        {
            RemoveAnimation(i);
            i--; // account for shifting array after removal
        }
    }
}

//======================================================================================

int sign(int number)
{
    int signBit = number & (1 << 31);
    return signBit ? -1 : 1;
}

int Abs(int number)
{
    return number * sign(number);
}

/*
  CheckForVerticalSpace
  Returns number of free vertical tiles in direction indicated by iterations' sign.
  Validates startingPosition pointer.
*/
int CheckForVerticalSpace(Vector2* startingPosition, int iterations)
{
    if (!startingPosition) return 0;

    int x = startingPosition->x;
    int y = startingPosition->y;

    int direction = sign(iterations);
    iterations *= direction;

    for (int i = 1; i <= iterations + 1; i++)
    {
        if (!isEmpty(x, y + (i * direction)))
        {
            // available spaces until obstacle
            return i - 1;
        }
    }
    return iterations + 1;
}

/*
  MoveObject
  Move a board object from one tile to another and update the console.
  Validates moveFrom and moveTo.
*/
void MoveObject(Vector2* moveFrom, Vector2* moveTo)
{
    if (!moveFrom || !moveTo) return;
    if (!ValidateVectorForBoard(moveFrom) || !ValidateVectorForBoard(moveTo)) return;
    if (AreEqual(moveFrom, moveTo)) return;

    unsigned int objectToMove = board[moveFrom->x][moveFrom->y];

    SetCursorPosition(moveFrom);
    PrintObject(air);

    SetCursorPosition(moveTo);
    PrintObject(objectToMove);

    board[moveFrom->x][moveFrom->y] = air;
    board[moveTo->x][moveTo->y] = (unsigned char)objectToMove;
}

/*
 * MoveEnemy
 * Move an enemy position by direction vector; validates pointers.
 * Returns true if moved, false otherwise.
 */
bool MoveEnemy(Vector2* direction, Vector2* position)
{
    if (!direction || !position) return false;
    if (AreEqual(direction, &vector_zero)) return true;

    Vector2* enemyMove = AddVectors(position, direction);
    if (!enemyMove) return false;

    // there is nothing under the enemy move
    bool canMove = isEmpty(enemyMove->x, enemyMove->y);
    if (canMove)
    {
        MoveObject(position, enemyMove);
        *position = *enemyMove;
        delete enemyMove;
        return true;
    }

    delete enemyMove;
    return false;
}

/*
 * MovePlayer
 * Move the player by direction if possible. Validates parameters.
 * Returns 0 on success, 1 on failure.
 */
int MovePlayer(Vector2* direction, Vector2* playerPosition)
{
    if (!direction || !playerPosition) return 1;
    if (AreEqual(direction, &vector_zero)) return 1;

    Vector2* playerMove = AddVectors(playerPosition, direction);
    if (!playerMove) return 1;

    if (isEmptyOrEnemy(playerMove->x, playerMove->y))
    {
        unsigned char boardValue = isEnemy(playerMove);
        if (boardValue)
        {
            bool canGo = CollideWithEnemy(boardValue - enemyIndexOffset);
            DamagePlayer(1);
            if (!canGo)
            {
                delete playerMove;
                return 1;
            }
        }

        MoveObject(playerPosition, playerMove);

        *playerPosition = *playerMove;
        delete playerMove;
        return 0;
    }
    else
    {
        delete playerMove;
        return 1;
    }
}

/*
 * GravityStep
 * Apply gravity/momentum to an object. Validates pointers.
 */
void GravityStep(Vector2* gravityPull, Vector2* momentum, Vector2* position, int& jumpsLeft)
{
    if (!gravityPull || !momentum || !position) return;

    AddVectorsDirectly(momentum, gravityPull);

    int freePosibleSpaces = CheckForVerticalSpace(position, momentum->y);

    int scalarMomentum = momentum->y;

    if (freePosibleSpaces == 0)
    {
        ZeroVector(momentum);
        if (sign(scalarMomentum) == 1)
        {
            jumpsLeft = maxJumps;
        }
        return;
    }

    if (scalarMomentum == 0) return;

    if (freePosibleSpaces > Abs(scalarMomentum))
    {
        momentum->y = scalarMomentum;
    }
    else
    {
        if (sign(scalarMomentum) == 1)
        {
            jumpsLeft = maxJumps;
        }
        momentum->y = freePosibleSpaces * sign(scalarMomentum);
    }
}

/*
 * PlayerJump
 * Apply jump force to player if jumps remain. Validates playerObject pointer.
 */
void PlayerJump(Player* playerObject)
{
    if (!playerObject) return;
    if (!(playerObject->jumpsLeft)) return;

    playerObject->verticalMomentum.y = jumpHeight;
    playerObject->jumpsLeft--;
}

//=====================================================

void AdjustVectorToSize(Vector2* v, int Size)
{
    if (!v) return;
    for (int i = 0; i < Size / 2; i++)
    {
        AddVectorsDirectly(v, &vector_up);
        AddVectorsDirectly(v, &vector_left);
    }
}

bool IsEmpty_Special(Vector2 position, int Size)
{
    Vector2 current = position;

    int xDirection = 1;
    int yDirection = 1;

    AdjustVectorToSize(&current, Size);

    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
        {
            if (!isEmpty(&current)) return false;

            current.x += xDirection;
        }

        xDirection *= -1;
        current.x += xDirection;
        current.y += yDirection;
    }

    return true;
}

void ValidateSpecialPosition(Vector2* position, int Size)
{
    if (!position) return;

    int halfSize = Size / 2;

    if (position->x < 1 + halfSize)
    {
        position->x = 1 + halfSize;
    }
    else if (position->x > width - 2 - halfSize)
    {
        position->x = width - 2 - halfSize;
    }

    if (position->y < 1 + halfSize)
    {
        position->y = 1 + halfSize;
    }
    else if (position->y > height - 2 - halfSize)
    {
        position->y = height - 2 - halfSize;
    }
}

/*
 * MoveSpecial
 * Move a larger-than-one tile enemy (square). Validates inputs.
 */
void MoveSpecial(Enemy* e, Vector2* newPosition, int Size)
{
    if (!e || !newPosition) return;

    ValidateSpecialPosition(newPosition, Size);

    Vector2 moveFrom = e->position;
    Vector2 moveTo = *newPosition;

    e->position = moveTo;

    AdjustVectorToSize(&moveFrom, Size);
    AdjustVectorToSize(&moveTo, Size);

    int xDirection = 1;
    int yDirection = 1;

    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
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
    return (number - integerPart) > 0 ? integerPart + 1 : integerPart;
}

void DamagePlayer(int DMG)
{
    player.HP -= DMG;
    PrintHeader();
    if(player.HP<=0)
    {
        lost = true;
    }
}

const Enemy WalkerBlueprint =
{
    'E',
    EnemyTypes::Walker,
    1,
    1,
    Color::COLOR_GREEN,
    vector_zero,
    nullptr
};

const Enemy CrawlerBlueprint =
{
    'C',
    EnemyTypes::Crawler,
    1,
    1,
    Color::COLOR_YELLOW,
    vector_zero,
    nullptr
};

const Enemy FlierBlueprint =
{
    'F',
    EnemyTypes::Flier,
    1,
    1,
    Color::COLOR_BLUE,
    vector_zero,
    nullptr
};

const Enemy JumperBlueprint =
{
    'J',
    EnemyTypes::Jumper,
    1,
    1,
    Color::COLOR_MAGENTA,
    vector_zero,
    nullptr
};

const Enemy CrusherBlueprint =
{
    'B',
    EnemyTypes::Crusher,
    10,
    3,
    Color::COLOR_CYAN,
    vector_zero,
    nullptr
};

const Enemy ShooterBlueprint =
{
    'B',
    EnemyTypes::Shooter,
    10,
    3,
    Color::COLOR_CYAN,
    vector_zero,
    nullptr
};

void PutSpecialEnemyOnBoard(Enemy* enemyToAdd, int enemySize, int index)
{
    if (!enemyToAdd || !board) return;

    int offset = enemySize / 2;

    for (int i = 0; i < enemySize; i++)
    {
        for (int j = 0; j < enemySize; j++)
        {
            int bx = enemyToAdd->position.x + i - offset;
            int by = enemyToAdd->position.y + j - offset;
            if (bx >= 0 && bx < width && by >= 0 && by < height && board[bx])
            {
                board[bx][by] = (unsigned char)index;
            }
        }
    }
}

void DrawSpecialEnemy(Enemy* enemyToAdd, int enemySize, int index)
{
    PutSpecialEnemyOnBoard(enemyToAdd, enemySize, index);
    //PrintFullBoard();
}

void AddSpecialEnemy(Enemy* enemyToAdd, int enemySize)
{
    if (!enemiesAlive || currentEnemiesAlive >= maxEnemiesAllowed) return;
    if (!enemyToAdd) return;

    enemiesAlive[currentEnemiesAlive] = *enemyToAdd;
    DrawSpecialEnemy(enemyToAdd, enemySize, currentEnemiesAlive + enemyIndexOffset);
    currentEnemiesAlive++;
}

void AddEnemy(Enemy* enemyToAdd)
{
    if (!enemiesAlive || currentEnemiesAlive >= maxEnemiesAllowed || !enemyToAdd) return;

    enemiesAlive[currentEnemiesAlive] = *enemyToAdd;

    if (ValidateVectorForBoard(&enemyToAdd->position))
    {
        board[enemyToAdd->position.x][enemyToAdd->position.y] = (unsigned char)(currentEnemiesAlive + enemyIndexOffset);
    }
    currentEnemiesAlive++;
}

bool DecreaseBoardValueForEnemy(Enemy* enemy)
{
    if (!enemy) return false;

    int Size = enemy->Size;
    Vector2 current = enemy->position;

    int xDirection = 1;
    int yDirection = 1;

    AdjustVectorToSize(&current, Size);

    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
        {
            DecreaseValueByOne(&current);
            current.x += xDirection;
        }

        xDirection *= -1;
        current.x += xDirection;
        current.y += yDirection;
    }

    return true;
}

void RemoveEnemyFromBoard(Enemy* enemy)
{
    if (!enemy) return;

    int Size = enemy->Size;
    Vector2 current = enemy->position;

    int xDirection = 1;
    int yDirection = 1;

    AdjustVectorToSize(&current, Size);

    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
        {
            RemoveObject(&current);
            current.x += xDirection;
        }

        xDirection *= -1;
        current.x += xDirection;
        current.y += yDirection;
    }
}

/*
 * RemoveEnemy
 * Remove enemy at a given index. Validates index and enemiesAlive.
 */
void RemoveEnemy(int enemyIndex)
{
    if (!enemiesAlive) return;
    if (enemyIndex < 0 || enemyIndex >= currentEnemiesAlive) return;

    Enemy* enemyToRemove = enemiesAlive + enemyIndex;

    if (enemyToRemove->type == Shooter)
    {
        CleanShooter(enemyToRemove);
    }
    delete[] enemyToRemove->extraInfo;

    RemoveEnemyFromBoard(enemyToRemove);

    for (int i = enemyIndex + 1; i < currentEnemiesAlive; i++)
    {
        DecreaseBoardValueForEnemy(enemiesAlive + i);
        enemiesAlive[i - 1] = enemiesAlive[i];
    }

    currentEnemiesAlive--;
}

bool CollideWithEnemy(int EnemyIndex)
{
    if (EnemyIndex < 0 || EnemyIndex >= currentEnemiesAlive) return false;

    if (isBoss(EnemyIndex))
    {
        return false;
    }

    RemoveEnemy(EnemyIndex);
    return true;
}

void FindTheClosestFloor(Vector2* position)
{
    if (!position) return;
    while (!canStand(*position))
    {
        position->y++;
    }
}

Vector2 GetRandomEmptyVector()
{
    Vector2 v;
    if (!board) return { 1,1 };
    do
    {
        v = {
            GenerateRandom(1, width - 2),
            GenerateRandom(1, height - 2)
        };
    } while (!isEmpty(v.x, v.y));
    return v;
}

Vector2 GetRandomEmptyVector(int Size)
{
    Vector2 v;
    if (!board) return { 1,1 };
    do
    {
        v = {
            GenerateRandom(1, width - 2),
            GenerateRandom(1, height - 2)
        };
    } while (!IsEmpty_Special(v, Size));
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
    walker.extraInfo = new int[1] { walkerDeffaultDirection }; // was: new int(walkerDeffaultDirection)
    AddEnemy(&walker);
}

bool WalkerStep(Enemy* enemy)
{
    if (!enemy) return false;
    Vector2* position = &enemy->position;
    if (!enemy->extraInfo) return false;
    int direction = *enemy->extraInfo;

    Vector2 desiredPosition = *position;
    desiredPosition.x += direction;

    if (AreEqual(&desiredPosition, player.position))
    {
        return true;
    }

    if (canStand(desiredPosition))
    {
        MoveObject(position, &desiredPosition);
        *position = desiredPosition;

        return false;
    }

    // change direction
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
    crawler.extraInfo = new int[crawlerInfoSize] {crawlerDirectionXDefault, crawlerDirectionYDefault};
    AddEnemy(&crawler);
}

void CrawlerGetNewDirection(Enemy* enemy)
{
    if (!enemy || !enemy->extraInfo) return;

    Vector2 position = enemy->position;
    Vector2 direction =
    {
        enemy->extraInfo[crawlerDirectionXIndex],
        enemy->extraInfo[crawlerDirectionYIndex]
    };

    if (direction.x)
    {
        Vector2 testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_up);
        if (canCrawl(testPosition))
        {
            enemy->extraInfo[crawlerDirectionXIndex] = vector_up.x;
            enemy->extraInfo[crawlerDirectionYIndex] = vector_up.y;
            return;
        }

        testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_down);
        if (canCrawl(testPosition))
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
        if (canStandorHang(testPosition))
        {
            enemy->extraInfo[crawlerDirectionXIndex] = vector_left.x;
            enemy->extraInfo[crawlerDirectionYIndex] = vector_left.y;
            return;
        }

        testPosition = position;
        AddVectorsDirectly(&testPosition, &vector_right);
        if (canStandorHang(testPosition))
        {
            enemy->extraInfo[crawlerDirectionXIndex] = vector_right.x;
            enemy->extraInfo[crawlerDirectionYIndex] = vector_right.y;
            return;
        }
    }

    enemy->extraInfo[crawlerDirectionXIndex] *= -1;
    enemy->extraInfo[crawlerDirectionYIndex] *= -1;
}

bool CrawerStep(Enemy* enemy)
{
    if (!enemy || !enemy->extraInfo) return false;

    Vector2* position = &enemy->position;
    Vector2 direction =
    {
        enemy->extraInfo[crawlerDirectionXIndex],
        enemy->extraInfo[crawlerDirectionYIndex]
    };

    Vector2 desiredPosition = direction;

    AddVectorsDirectly(&desiredPosition, position);

    if (AreEqual(&desiredPosition, player.position))
    {
        return true;
    }

    // change direction
    if (direction.x)
    {
        if (canStandorHang(desiredPosition))
        {
            MoveObject(position, &desiredPosition);
            *position = desiredPosition;

            return false;
        }
    }
    else
    {
        if (canCrawl(desiredPosition))
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
    // Ensure the flier has a non-zero horizontal direction so it can move.
    flier.extraInfo = new int[flierInfoSize] { 1 }; // set default direction to 1
    AddEnemy(&flier);
}

const int flyPercentage = 10;

bool Fly(Vector2* position)
{
    if (!position) return false;

    // change y position - flyPercentage% chance
    int flyNumber = GenerateRandom(1, 100);

    if (flyPercentage >= flyNumber)
    {
        Vector2 desiredPosition = vector_zero;

        desiredPosition.y = player.position ? player.position->y - position->y : 0;
        if (!desiredPosition.y) return false;

        desiredPosition.y = sign(desiredPosition.y);

        AddVectorsDirectly(&desiredPosition, position);

        if (AreEqual(&desiredPosition, player.position))
        {
            return true;
        }

        if (isEmpty(desiredPosition.x, desiredPosition.y))
        {
            MoveObject(position, &desiredPosition);
            *position = desiredPosition;
            return false;
        }
    }
    return false;
}

bool FlierStep(Enemy* enemy)
{
    if (!enemy || !enemy->extraInfo) return false;

    Vector2* position = &enemy->position;
    int direction = *enemy->extraInfo;

    // If direction somehow is zero, default to 1 to ensure movement.
    if (direction == 0)
    {
        direction = 1;
        *enemy->extraInfo = direction;
    }

    Vector2 desiredPosition = *position;
    desiredPosition.x += direction;

    if (AreEqual(&desiredPosition, player.position))
    {
        return true;
    }

    if (isEmpty(desiredPosition.x, desiredPosition.y))
    {
        MoveObject(position, &desiredPosition);
        *position = desiredPosition;
        return Fly(position);
    }

    // change direction when blocked
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

    jumper.extraInfo = new int[jumperInfoSize] {JumperDirectionDefault, JumperVelocityXDefault, JumperVelocityYDefault, JumperCanJumpDefault};

    AddEnemy(&jumper);
}

void TryToJump(Enemy* jumper, int& canJump, Vector2& velocity)
{
    if (!jumper) return;

    Vector2* position = &jumper->position;
    int distaceToPlayer = RoughDistance(position, player.position);

    if (canJump && distaceToPlayer <= maximumDistaceToJump && player.position && player.position->y < position->y)
    {
        velocity.y = jumpForceEnemyJumper;
        canJump = false;
    }
}

bool JumperStep(Enemy* enemy)
{
    if (!enemy || !enemy->extraInfo) return false;

    Vector2* position = &enemy->position;
    int direction = enemy->extraInfo[JumperDirectionIndex];
    Vector2 velocity = {
        enemy->extraInfo[JumperVelocityXIndex],
        enemy->extraInfo[JumperVelocityYIndex]
    };
    int canJump = enemy->extraInfo[JumperCanJumpIndex];

    Vector2 desiredPosition = *position;
    desiredPosition.x += direction;

    if (AreEqual(&desiredPosition, player.position))
    {
        return true;
    }

    if (isEmpty(desiredPosition.x, desiredPosition.y))
    {
        MoveObject(position, &desiredPosition);
        *position = desiredPosition;
    }
    else
    {
        // change direction
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

const int crusherBehaviourCount = 3;

const int bossSize = 3;
const int crusherExpandedSize = 7;

const int bossOngoingIndex = 0;
const int bossBehaviourIndex = 1;
const int bossTimerIndex = 2;
const int bossChosenPositionXIndex = 3;
const int bossChosenPositionYIndex = 4;
const int bossBoardIndex = 5;
// at least 2 sec
const int moveTimerCrusher = 7;
const int expandTimerCrusher = 1;
const int shrinkTimerCrusher = 7;

const int moveWarningTimerCrusher = 3;


const char* CrusherMoveWarningOutput = "!!!\n!!!\n!!!";


void SpawnCrusher()
{
    Enemy boss = CrusherBlueprint;

    boss.position = GetRandomEmptyVector(bossSize);
    boss.extraInfo = new int[6] {0, -1, 0, 0, 0, currentEnemiesAlive};

    AddSpecialEnemy(&boss, bossSize);
    cout << currentEnemiesAlive;
}

// crusher extra info:
/*
{
isOngoing,
behaviour,
timer,
chosenPositionX,
chosenPositionY
}
*/

bool DidSpecialEnemyHitPlayer(Enemy* enemy)
{
    if (!enemy || !player.position) return false;

    int Size = enemy->Size;
    Vector2 current = enemy->position;

    int xDirection = 1;
    int yDirection = 1;

    AdjustVectorToSize(&current, Size);

    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
        {
            if (AreEqual(&current, player.position))
            {
                return true;
            }
            current.x += xDirection;
        }

        xDirection *= -1;
        current.x += xDirection;
        current.y += yDirection;
    }

    return false;
}

enum CrusherBehaviour
{
    SquareMovePhase = 0,
    SquareExpandPhase = 1,
    SquareShrinkPhase = 2
};

const Animation CrusherMoveWarning =
{
    false,
    COLOR_RED,
    200,
    nullptr,
    nullptr,
    vector_zero
};

void CrusherChooseNewSquareMovePosition(Enemy* boss)
{
    if (!boss || !player.position) return;

    Vector2 position = *player.position;

    ValidateSpecialPosition(&position, crusherExpandedSize);

    boss->extraInfo[bossChosenPositionXIndex] = position.x;
    boss->extraInfo[bossChosenPositionYIndex] = position.y;
}

void MovePlayerToSafety()
{
    if (!player.position) return;

    if (BoardValue(player.position) == player_index)
    {
        board[player.position->x][player.position->y] = air;
    }

    Vector2 newPosition = GetRandomEmptyVector();

    *player.position = newPosition;

    VisualizePlayer();
}

void CrusherChooseNewBehaviour(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return;

    boss->extraInfo[bossOngoingIndex] = true;
    int behaviour = boss->extraInfo[bossBehaviourIndex];
    behaviour++;
    behaviour = behaviour % crusherBehaviourCount;

    switch (behaviour)
    {
    case SquareMovePhase:
        CrusherChooseNewSquareMovePosition(boss);
        break;
    }
    boss->extraInfo[bossBehaviourIndex] = behaviour;
}

bool MoveCrusher(Enemy* boss)
{
    if (!boss) return false;

    Vector2 desiredPosition =
    {
        boss->extraInfo[bossChosenPositionXIndex],
        boss->extraInfo[bossChosenPositionYIndex]
    };

    MoveSpecial(boss, &desiredPosition, bossSize);
    bool hit = DidSpecialEnemyHitPlayer(boss);

    if (hit)
    {
        MovePlayerToSafety();
    }
    return hit;
}

void AddAnimationWarningCrusher(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return;

    Animation warning = CrusherMoveWarning;

    Vector2 warningPosition =
    {
        boss->extraInfo[bossChosenPositionXIndex],
        boss->extraInfo[bossChosenPositionYIndex]
    };

    AdjustVectorToSize(&warningPosition, bossSize);

    warning.output = (char*)CrusherMoveWarningOutput;
    warning.position = warningPosition;

    AddAnimation(&warning);
}

bool SquareMovePhaseCrusher(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return false;

    int timer = boss->extraInfo[bossTimerIndex];
    if (timer >= moveTimerCrusher)
    {
        boss->extraInfo[bossOngoingIndex] = false;
        boss->extraInfo[bossTimerIndex] = 0;
        return MoveCrusher(boss);
    }
    else
    {
        boss->extraInfo[bossTimerIndex]++;

        if (timer % moveWarningTimerCrusher == 1)
        {
            AddAnimationWarningCrusher(boss);
        }

        return false;
    }
}

void ExpandSpecial(Enemy* enemy, int wantedSize, int index)
{
    DrawSpecialEnemy(enemy, wantedSize, index);
}

bool ExpandCrusher(Enemy* boss)
{
    if (!boss) return false;

    boss->Size = crusherExpandedSize;
    int index = boss->extraInfo[bossBoardIndex] + enemyIndexOffset;

    bool hit = DidSpecialEnemyHitPlayer(boss);

    ExpandSpecial(boss, crusherExpandedSize, index);

    if (hit)
    {
        MovePlayerToSafety();
    }
    return hit;
}

void ShrinkCrusher(Enemy* boss)
{
    if (!boss) return;

    boss->Size = bossSize;
    int index = boss->extraInfo[bossBoardIndex] + enemyIndexOffset;
    PutSpecialEnemyOnBoard(boss, crusherExpandedSize, air);
    ExpandSpecial(boss, bossSize, index);
}

bool SquareExpandPhaseCrusher(Enemy* boss)
{
    if (!boss) return false;

    int timer = boss->extraInfo[bossTimerIndex];

    if (timer >= expandTimerCrusher)
    {
        boss->extraInfo[bossOngoingIndex] = false;
        boss->extraInfo[bossTimerIndex] = 0;

        return ExpandCrusher(boss);
    }
    else
    {
        boss->extraInfo[bossTimerIndex]++;
        return false;
    }
}

bool SquareShrinkPhaseCrusher(Enemy* boss)
{
    if (!boss) return false;

    int timer = boss->extraInfo[bossTimerIndex];

    if (timer >= shrinkTimerCrusher)
    {
        ShrinkCrusher(boss);
        boss->extraInfo[bossOngoingIndex] = false;
        boss->extraInfo[bossTimerIndex] = 0;
        return false;
    }
    else
    {
        boss->extraInfo[bossTimerIndex]++;
        return false;
    }
}

bool CrusherStep(Enemy* enemy)
{
    if (!enemy || !enemy->extraInfo) return false;

    bool isOngoing = enemy->extraInfo[bossOngoingIndex];

    if (isOngoing)
    {
        int behaviour = enemy->extraInfo[bossBehaviourIndex];

        switch (behaviour)
        {
        case SquareMovePhase:
            return SquareMovePhaseCrusher(enemy);
        case SquareExpandPhase:
            return SquareExpandPhaseCrusher(enemy);
        case SquareShrinkPhase:
            return SquareShrinkPhaseCrusher(enemy);
            break;
        }
    }
    else
    {
        CrusherChooseNewBehaviour(enemy);
    }
    return false;
}
//====================================================

void SpawnShooter()
{
    Enemy boss = ShooterBlueprint;

    boss.position = GetRandomEmptyVector(bossSize);
    int strSize = sizeof(char*);

    boss.extraInfo = new int[7 + strSize] {0, -1, 0, 0, 0, currentEnemiesAlive, 0};

    AddSpecialEnemy(&boss, bossSize);
    cout << currentEnemiesAlive;
}


enum ShooterBehaviour
{
    MovePhase = 0,
    ShootDiagonalPhase = 1,
    ShooterWaitForCleaning1 = 2,
    ShootLinePhase = 3,
    ShooterWaitForCleaning2 = 4
};
const int shooterBehaviourCount = 5;
const int shooterPointersEncoded = 6;
const int moveTimerShooter = 7;
const int shootDiagonalTimerShooter = 7;
const int ShooterWarningDuration = 150;

const int intSize = 32;
const unsigned int maxInt = 0xffffffff;

const char warningLetter = '!';
const char laserLetter = 'L';

const int laserDuration = 300;

/*
 * DecodePointerFromShooter
 * Reconstruct an Animation* previously encoded into the shooter's extraInfo array.
 * Returns nullptr on invalid inputs.
 */
Animation* DecodePointerFromShooter(Enemy* enemy, int index)
{
    int ptrSize = sizeof(Animation*) / 4;

    //for old PCs
    if (ptrSize < 1)
    {
        ptrSize = 1;
    }

    int* arr = enemy->extraInfo + shooterPointersEncoded;
    arr++;
    arr += index * ptrSize;
    long long pointerRaw = 0;

    for (int i = 0; i < ptrSize; i++)
    {
        pointerRaw += *arr;
        arr++;
        if (i != ptrSize - 1)pointerRaw <<= intSize;
        //cout<<cur<<" "<<ptrSize<<" ";
    }

    return (Animation*)pointerRaw;
}

/*
  EncodePointerIntoShooter
  Store an Animation* value into the shooter's extraInfo array.
*/
void EncodePointerIntoShooter(Animation* pointer, Enemy* enemy)
{
    if (!enemy || !enemy->extraInfo) return;

    int ptrSize = sizeof(Animation*) / 4;
    if (ptrSize < 1) ptrSize = 1;

    int* arr = enemy->extraInfo + shooterPointersEncoded;
    int pointersCount = *arr;

    // beginning of array
    arr++;
    arr += pointersCount * ptrSize;

    long long pointerRaw = (long long)(pointer);

    for (int i = 0; i < ptrSize; i++)
    {
        *arr = (int)(pointerRaw & maxInt);
        arr++;
        pointerRaw >>= intSize;
    }

    // increase count
    enemy->extraInfo[shooterPointersEncoded]++;
}

void ShooterChooseNewMovePosition(Enemy* boss)
{
    if (!boss) return;
    Vector2 position = GetRandomEmptyVector(bossSize);

    boss->extraInfo[bossChosenPositionXIndex] = position.x;
    boss->extraInfo[bossChosenPositionYIndex] = position.y;
}

void CleanShooter(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return;

    boss->extraInfo[shooterPointersEncoded] = 0;
    for (int i = 0; i < 4; i++)
    {
        Animation* animation = DecodePointerFromShooter(boss, i);

        if (animation != nullptr)
        {
            if (animation->output != nullptr)
            {
                delete[] animation->output;
            }
            delete animation;
        }
    }
}

void ShooterChooseNewBehaviour(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return;
    boss->extraInfo[bossOngoingIndex] = true;
    int behaviour = boss->extraInfo[bossBehaviourIndex];
    behaviour++;
    behaviour = behaviour % shooterBehaviourCount;

    switch (behaviour)
    {
    case MovePhase:
        ShooterChooseNewMovePosition(boss);
        break;
    case ShootDiagonalPhase:
        AddDiagonalShootingAnimation(boss, warningLetter);
        break;
    case ShootLinePhase:
        AddLineShootingAnimations(boss, warningLetter);
        break;
    }
    boss->extraInfo[bossBehaviourIndex] = behaviour;
}

bool MoveShooter(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return false;

    Vector2 desiredPosition =
    {
        boss->extraInfo[bossChosenPositionXIndex],
        boss->extraInfo[bossChosenPositionYIndex]
    };

    MoveSpecial(boss, &desiredPosition, bossSize);
    bool hit = DidSpecialEnemyHitPlayer(boss);

    if (hit)
    {
        MovePlayerToSafety();
    }
    return hit;
}

void AddAnimationWarningShooter(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return;

    Animation warning = CrusherMoveWarning;

    Vector2 warningPosition =
    {
        boss->extraInfo[bossChosenPositionXIndex],
        boss->extraInfo[bossChosenPositionYIndex]
    };

    AdjustVectorToSize(&warningPosition, bossSize);

    warning.output = (char*)CrusherMoveWarningOutput;
    warning.position = warningPosition;

    AddAnimation(&warning);
}

void ChangeLetter(char oldLetter, char newLetter, char* str)
{
    if (!str) return;
    while (*str)
    {
        if (*str == oldLetter)
        {
            *str = newLetter;
        }

        str++;
    }
}

bool MovePhaseShooter(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return false;
    int timer = boss->extraInfo[bossTimerIndex];
    if (timer >= moveTimerShooter)
    {
        boss->extraInfo[bossOngoingIndex] = false;
        boss->extraInfo[bossTimerIndex] = 0;

        return MoveShooter(boss);
    }
    else
    {
        boss->extraInfo[bossTimerIndex]++;

        if (timer % moveWarningTimerCrusher == 1)
        {
            AddAnimationWarningCrusher(boss);
        }

        return false;
    }
}

void GenerateOut_main(char* str, int lines, char letter)
{
    if (!str) return;
    for (int line = 0; line < lines; line++)
    {
        if (line > 0)
        {
            str++;
        }

        for (int i = 0; i < line; i++)
        {
            *str = ' ';
            str++;
        }
        *str = letter;
        str++;
        *str = '\n';
    }
    *str = '\0';
}

void GenerateOut_secondary(char* str, int lines, char letter)
{
    if (!str) return;
    for (int line = 0; line < lines; line++)
    {
        if (line > 0)
        {
            str++;
        }

        for (int i = lines; i > line; i--)
        {
            *str = ' ';
            str++;
        }

        *str = letter;
        str++;
        *str = '\n';
    }
    *str = '\0';
}

const Color WarningColor = COLOR_RED;
//!
//  !
//    !
//      !
//.......
void GenerateAttackMainDiagonal(int distance1, int distance2, char letter, Vector2* position, Enemy* shooter)
{
    if (!position || !shooter) return;

    int diagonalDistance = minimum(distance1, distance2);
    if (diagonalDistance != 0)
    {
        int diagonalSize = 0;
        if (shootingStringSizes) diagonalSize = shootingStringSizes[diagonalDistance];
        if (diagonalSize <= 0) diagonalSize = diagonalDistance + 2;

        char* output = new char[diagonalSize];
        if (!output)
        {
            EncodePointerIntoShooter(nullptr, shooter);
            return;
        }

        GenerateOut_main(output, diagonalDistance, letter);
        Animation* newAnimation = CreateAnimation(WarningColor, *position, output, ShooterWarningDuration);

        EncodePointerIntoShooter(newAnimation, shooter);
    }
    else
    {
        EncodePointerIntoShooter(nullptr, shooter);
    }
}

//.......
//      !
//    !
//  !
//!
void GenerateAttackSecondaryDiagonal(int distance1, int distance2, char letter, Vector2* position, Enemy* shooter)
{
    if (!position || !shooter) return;

    int diagonalDistance = minimum(distance1, distance2);

    if (diagonalDistance != 0)
    {
        int diagonalSize = 0;
        if (shootingStringSizes) diagonalSize = shootingStringSizes[diagonalDistance];
        if (diagonalSize <= 0) diagonalSize = diagonalDistance + 2;

        char* output = new char[diagonalSize];
        if (!output)
        {
            EncodePointerIntoShooter(nullptr, shooter);
            return;
        }

        GenerateOut_secondary(output, diagonalDistance, letter);

        Animation* newAnimation = CreateAnimation(WarningColor, *position, output, ShooterWarningDuration);

        EncodePointerIntoShooter(newAnimation, shooter);
    }
    else
    {
        EncodePointerIntoShooter(nullptr, shooter);
    }
}

void Add_right_down_ShootingAnimation(Enemy* enemy, int distanceDown, int distanceRight, char letter)
{
    if (!enemy) return;
    Vector2 position = enemy->position;
    position.x += 2;
    position.y += 2;

    GenerateAttackMainDiagonal(distanceDown, distanceRight, letter, &position, enemy);
}

void Add_left_up_ShootingAnimation(Enemy* enemy, int distanceLeft, int distanceUp, char letter)
{
    if (!enemy) return;
    Vector2 position = enemy->position;

    int Range = 2 * minimum(distanceLeft, distanceUp) + bossSize;
    AdjustVectorToSize(&position, Range);

    GenerateAttackMainDiagonal(distanceLeft, distanceUp, letter, &position, enemy);
}

void Add_left_down_ShootingAnimation(Enemy* enemy, int distance1, int distance2, char letter)
{
    if (!enemy) return;
    Vector2 position = enemy->position;

    position.y += (1 + bossSize / 2);
    position.x -= (1 + bossSize / 2 + minimum(distance1, distance2));

    GenerateAttackSecondaryDiagonal(distance1, distance2, letter, &position, enemy);
}

void Add_right_up_ShootingAnimation(Enemy* enemy, int distance1, int distance2, char letter)
{
    if (!enemy) return;
    Vector2 position = enemy->position;

    position.x += (bossSize / 2);
    position.y -= (bossSize / 2 + minimum(distance1, distance2));

    GenerateAttackSecondaryDiagonal(distance1, distance2, letter, &position, enemy);
}

void AddDiagonalShootingAnimation(Enemy* enemy, char letter)
{
    if (!enemy) return;
    Vector2 bossPosition = enemy->position;

    int distanceUp, distanceDown, distanceLeft, distanceRight;
    distanceDown = height - bossPosition.y - 3;
    distanceRight = width - bossPosition.x - 3;
    distanceUp = bossPosition.y - 2;
    distanceLeft = bossPosition.x - 2;

    Add_right_down_ShootingAnimation(enemy, distanceDown, distanceRight, letter);
    Add_left_up_ShootingAnimation(enemy, distanceLeft, distanceUp, letter);
    Add_left_down_ShootingAnimation(enemy, distanceDown, distanceLeft, letter);
    Add_right_up_ShootingAnimation(enemy, distanceUp, distanceRight, letter);
}

void GenerateOut_vertical(char* str, int lines, char letter)
{
    if (!str) return;
    for (int line = 0; line < lines; line++)
    {
        *str = letter;
        str++;
        *str = '\n';
        str++;
    }
    str--;
    *str = '\0';
}

void GenerateOut_horizontal(char* str, int lineSize, char letter)
{
    if (!str) return;
    for (int i = 0; i < lineSize; i++)
    {
        *str = letter;
        str++;
    }
    *str = '\0';
}

void GenerateAttackHorizontal(int distance, char letter, Vector2* position, Enemy* shooter)
{
    if (!position || !shooter) return;

    if (distance != 0)
    {
        int Size = distance + 1;

        char* output = new char[Size];
        if (!output)
        {
            EncodePointerIntoShooter(nullptr, shooter);
            return;
        }

        GenerateOut_horizontal(output, distance, letter);

        Animation* newAnimation = CreateAnimation(WarningColor, *position, output, ShooterWarningDuration);

        EncodePointerIntoShooter(newAnimation, shooter);
    }
    else
    {
        EncodePointerIntoShooter(nullptr, shooter);
    }
}

void GenerateAttackVertical(int distance, char letter, Vector2* position, Enemy* shooter)
{
    if (!position || !shooter) return;

    if (distance > 0) // was: distance != 0
    {
        int Size = distance * 2;
        char* output = new char[Size];
        if (!output)
        {
            EncodePointerIntoShooter(nullptr, shooter);
            return;
        }

        GenerateOut_vertical(output, distance, letter);
        Animation* newAnimation = CreateAnimation(WarningColor, *position, output, ShooterWarningDuration);

        EncodePointerIntoShooter(newAnimation, shooter);
    }
    else
    {
        EncodePointerIntoShooter(nullptr, shooter);
    }
}

void Add_right_ShootingAnimation(Enemy* enemy, int distance, char letter)
{
    if (!enemy) return;
    Vector2 position = enemy->position;
    position.x += 1 + bossSize / 2;

    GenerateAttackHorizontal(distance, letter, &position, enemy);
}

void Add_left_ShootingAnimation(Enemy* enemy, int distance, char letter)
{
    if (!enemy) return;
    Vector2 position = enemy->position;
    position.x -= bossSize / 2 + distance;

    GenerateAttackHorizontal(distance, letter, &position, enemy);
}

void Add_up_ShootingAnimation(Enemy* enemy, int distance, char letter)
{
    if (!enemy) return;
    Vector2 position = enemy->position;

    position.y -= (1 + bossSize / 2 + distance);

    GenerateAttackVertical(distance, letter, &position, enemy);
}

void Add_down_ShootingAnimation(Enemy* enemy, int distance, char letter)
{
    if (!enemy) return;
    Vector2 position = enemy->position;

    position.y += (1 + bossSize / 2);

    GenerateAttackVertical(distance, letter, &position, enemy);
}

void AddLineShootingAnimations(Enemy* enemy, char letter)
{
    if (!enemy) return;
    Vector2 bossPosition = enemy->position;

    int distanceUp, distanceDown, distanceLeft, distanceRight;
    distanceDown = height - bossPosition.y - 3;
    distanceRight = width - bossPosition.x - 3;
    distanceUp = bossPosition.y - 3;
    distanceLeft = bossPosition.x - 2;

    Add_right_ShootingAnimation(enemy, distanceRight, letter);
    Add_left_ShootingAnimation(enemy, distanceLeft, letter);
    Add_up_ShootingAnimation(enemy, distanceUp, letter);
    Add_down_ShootingAnimation(enemy, distanceDown, letter);
}

void StartAnimations(Enemy* enemy)
{
    if (!enemy) return;

    for (int i = 0; i < 4; i++)
    {
        Animation* animation = DecodePointerFromShooter(enemy, i);

        if (animation != nullptr)
        {
            AddAnimation(animation);
        }
    }
}

void LaserHit(int boardValue)
{
    if (boardValue == player_index)
    {
        DamagePlayer(1);
        MovePlayerToSafety();
    }
}

const int laserCollisinDuration = 600;

void ShooterShoot(Enemy* enemy)
{
    if (!enemy) return;

    for (int i = 0; i < 4; i++)
    {
        Animation* animation = DecodePointerFromShooter(enemy, i);

        if (animation == nullptr)
        {
            continue;
        }
        animation->duration = laserDuration;

        if (animation->output) ChangeLetter(warningLetter, laserLetter, animation->output);

        AddAnimation(animation);
        BuildCollision(animation, LaserHit, laserCollisinDuration);
    }
}

bool ShootPhaseShooter(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return false;

    int timer = boss->extraInfo[bossTimerIndex];
    if (timer >= shootDiagonalTimerShooter)
    {
        boss->extraInfo[bossOngoingIndex] = false;
        boss->extraInfo[bossTimerIndex] = 0;

        ShooterShoot(boss);
        return false;
    }
    else
    {
        boss->extraInfo[bossTimerIndex]++;

        if (timer % moveWarningTimerCrusher == 1)
        {
            StartAnimations(boss);
        }

        return false;
    }
}

const int WaitTimerShooter = 12;

bool WaitShooter(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return false;
    int timer = boss->extraInfo[bossTimerIndex];
    if (timer >= WaitTimerShooter)
    {
        boss->extraInfo[bossOngoingIndex] = false;
        boss->extraInfo[bossTimerIndex] = 0;
        CleanShooter(boss);

        boss->extraInfo[shooterPointersEncoded] = 0;
        return false;
    }
    else
    {
        boss->extraInfo[bossTimerIndex]++;
        return false;
    }
}

bool ShootLinePhaseShooter(Enemy* boss)
{
    if (!boss || !boss->extraInfo) return false;
    int timer = boss->extraInfo[bossTimerIndex];
    if (timer >= shootDiagonalTimerShooter)
    {
        boss->extraInfo[bossOngoingIndex] = false;
        boss->extraInfo[bossTimerIndex] = 0;

        ShooterShoot(boss);
        return false;
    }
    else
    {
        boss->extraInfo[bossTimerIndex]++;

        if (timer % moveWarningTimerCrusher == 1)
        {
            StartAnimations(boss);
        }

        return false;
    }
}

bool ShooterStep(Enemy* enemy)
{
    if (!enemy || !enemy->extraInfo) return false;

    bool isOngoing = enemy->extraInfo[bossOngoingIndex];

    if (isOngoing)
    {
        int behaviour = enemy->extraInfo[bossBehaviourIndex];

        switch (behaviour)
        {
        case MovePhase:
            return MovePhaseShooter(enemy);
        case ShootDiagonalPhase:
            return ShootPhaseShooter(enemy);
            break;
        case ShooterWaitForCleaning1:
            return WaitShooter(enemy);
            break;
        case ShootLinePhase:
            return ShootLinePhaseShooter(enemy);
            break;
        case ShooterWaitForCleaning2:
            return WaitShooter(enemy);
            break;
        }
    }
    else
    {
        ShooterChooseNewBehaviour(enemy);
    }
    return false;
}
//====================================================

void SpawnEnemy()
{
    int randomEnemy = GenerateRandom(0, enemyTypeCount - 1);
    EnemyTypes enemyType = (EnemyTypes)randomEnemy;

    switch (enemyType)
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
    default:
        break;
    }
}

void SpawnBoss()
{
    SpawnShooter();
}

void EnemiesStep()
{
    if (!enemiesAlive) return;

    for (int i = 0; i < currentEnemiesAlive; i++)
    {
        EnemyTypes currentType = enemiesAlive[i].type;
        bool hit = false;

        switch (currentType)
        {
        case EnemyTypes::Walker:
            hit = WalkerStep(enemiesAlive + i);
            break;
        case Crawler:
            hit = CrawerStep(enemiesAlive + i);
            break;
        case Flier:
            hit = FlierStep(enemiesAlive + i);
            break;
        case Jumper:
            hit = JumperStep(enemiesAlive + i);
            break;
        case Crusher:
            hit = CrusherStep(enemiesAlive + i);
            break;
        case Shooter:
            hit = ShooterStep(enemiesAlive + i);
            break;
        default:
            break;
        }

        if (hit)
        {
            DamagePlayer(1);
            if (!isBoss(i))
            {
                RemoveEnemy(i);
                i--; // adjust index after removal
            }
        }
    }
}

void SpawnWave(int numOfEnemies)
{
    for (int i = 0; i < numOfEnemies; i++)
    {
        SpawnEnemy();
    }
}

void SpawnFirstWave()
{
    SpawnWalker();
    SpawnFlier();
    SpawnCrawler();
    SpawnJumper();
    SpawnWalker();
}

/*
 * DamageEnemy
 * Reduce HP of enemy at index; validate inputs.
 */
void DamageEnemy(int enemyIndex, int amount)
{
    if (!enemiesAlive) return;
    if (enemyIndex < 0 || enemyIndex >= currentEnemiesAlive) return;

    Enemy* e = enemiesAlive + enemyIndex;
    e->HP -= amount;

    if (e->HP <= 0)
    {
        RemoveEnemy(enemyIndex);
    }
}

//======================================================================================

void AddPlayer()
{
    player.HP = 5;

    Vector2* playerPosition = new Vector2;
    if (!playerPosition) return;

    player.position = playerPosition;
    player.jumpsLeft = maxJumps;
    player.verticalMomentum = vector_zero;

    int X = (int)(width / 2) + 1;
    int Y = (int)(height / 2) + 1;

    playerPosition->x = X;
    playerPosition->y = Y;

    if (board && ValidateVectorForBoard(playerPosition))
    {
        board[X][Y] = player_index;
    }
}


const Color headerColor = COLOR_WHITE;
void PrintHeader()
{
    setColor(headerColor);
    Vector2 v = { 0,-1 };

    SetCursorPosition(&v);

    cout << "HP: ";
    for(int i = 0; i<player.HP; i++)
    {
        cout<<'o';
        if(i<player.HP-1)
        {
            cout<<'-';
        }
    }

    cout<<"  left/right: A/D    jump: W    attack:j/l/k/i";
    cout<<"  ";
    cout << endl;
}

void PrintFullBoard()
{
    if (!board) return;
    PrintHeader();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (board[x]) PrintObject(board[x][y]);
            else cout << ' ';
        }
        cout << endl;
    }
}

char GetInput()
{
    // TODO: improve input handling
    if (_kbhit())
    {
        return _getch();
    }

    return 0;
}

float GetDeltaTime()
{
    clock_t current_time = clock();

    // time between the last frame and the new one
    float delta = (float)(current_time - lastFrameTime);

    lastFrameTime = current_time;

    return delta;
}

void GenerateShootingSize()
{
    if (shootingStringSizes) delete[] shootingStringSizes;
    shootingStringSizes = new short[width];
    if (!shootingStringSizes) return;

    shootingStringSizes[0] = 0;

    for (int i = 1; i < width; i++)
    {
        shootingStringSizes[i] = i + 2 + shootingStringSizes[i - 1];
    }
}

void GameSetup()
{
    // allocate board columns
    if (board) // free existing (defensive)
    {
        for (int i = 0; i < width; i++)
        {
            delete[] board[i];
        }
        delete[] board;
        board = nullptr;
    }

    board = new unsigned char* [width];
    if (!board) return;

    for (int i = 0; i < width; i++)
    {
        board[i] = new unsigned char[height];
        if (!board[i])
        {
            // on allocation failure, cleanup and abort setup
            for (int j = 0; j < i; j++) delete[] board[j];
            delete[] board;
            board = nullptr;
            return;
        }
    }
    GenerateMap();
    AddPlayer();

    if (!enemiesAlive) enemiesAlive = new Enemy[maxEnemiesAllowed];

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

    GenerateShootingSize();
}

void InputManager(char input)
{
    if (input == 0) return;

    if (input == 'a')
    {
        MovePlayer(&vector_left, player.position);
    }
    else if (input == 'd')
    {
        MovePlayer(&vector_right, player.position);
    }
    else if (input == 'w')
    {
        PlayerJump(&player);
    }
    else if (input == 'i')
    {
        tryAttacking(AttackDirections::up, &player);
    }
    else if (input == 'k')
    {
        tryAttacking(AttackDirections::down, &player);
    }
    else if (input == 'l')
    {
        tryAttacking(AttackDirections::right, &player);
    }
    else if (input == 'j')
    {
        tryAttacking(AttackDirections::left, &player);
    }
}

/*
 * GameLoop
 * Core game loop. Performs input, physics, AI and animations.
 */
void GameLoop()
{
    PrintFullBoard();
    float timer = 0;
    timer += GetDeltaTime();

    while (true)
    {
        char input = GetInput();
        InputManager(input);

        GravityStep(&vector_down, &player.verticalMomentum, player.position, player.jumpsLeft);
        MovePlayer(&player.verticalMomentum, player.position);
        if(lost)return;

        EnemiesStep();
        AnimationCollisionCheck();
        if(lost)return;

        if (currentEnemiesAlive == 0) break;

        while (timer <= frameDelay)
        {
            float delta = GetDeltaTime();
            timer += delta;

            AnimatinStep((int)delta);
        }
        timer = 0;
    }
}

const Color winColor = COLOR_GREEN;
void WIN()
{
    setColor(winColor);
    for (int i = 0; i < height * 2; i++)
    {
        cout << "\n";
    }
    cout << " _    _ _____ _   _\n| |  | |_   _| \\ | |\n| |  | | | | |  \\| |\n| |/\\| | | | | . ` |\n\\  /\\  /_| |_| |\\  |\n \\/  \\/ \\___/\\_| \\_/";
    for (int i = 0; i < 10; i++)
    {
        cout << "\n";
    }
    getch();
}

const Color loseColor = COLOR_RED;
void GAMEOVER()
{
    setColor(loseColor);
    for (int i = 0; i < height * 2; i++)
    {
        cout << "\n";
    }
    cout << " _____   ___  ___  ___ _____   _____  _   _ ___________\n|  __ \\ / _ \\ |  \\/  ||  ___| |  _  || | | |  ___| ___ \\\n| |  \\// /_\\ \\| .  . || |__   | | | || | | | |__ | |_/ /\n| | __ |  _  || |\\/| ||  __|  | | | || | | |  __||    /\n| |_\\ \\| | | || |  | || |___  \\ \\_/ /\\ \\_/ / |___| |\\ \\\n \\____/\\_| |_/\\_|  |_/\\____/   \\___/  \\___/\\____/\\_| \\_|";
    for (int i = 0; i < 10; i++)
    {
        cout << "\n";
    }
    getch();
}

void GenerateSeed()
{
    srand((unsigned int)time(NULL));
}

int main()
{
    GenerateSeed();
    GameSetup();
    //PrintFullBoard();
    int curWaveNumber = initialWaveNumber;

    SpawnFirstWave();
    GameLoop();
    curWaveNumber += GenerateRandom(3, 5);

    for (int i = 0; i < numberOfWaves - 1; i++)
    {
        if(lost)
        {
            GAMEOVER();
            return 0;
        }
        SpawnWave(curWaveNumber);

        GameLoop();

        curWaveNumber += GenerateRandom(3, 5);
        if(lost)
        {
            GAMEOVER();
            return 0;
        }
    }
    SpawnBoss();
    GameLoop();
    if(lost)
    {
        GAMEOVER();
        return 0;
    }
    WIN();

    return 0;
}
