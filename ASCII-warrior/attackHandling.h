#include <mapGeneration.h>

enum AttackDirections
{
    up,
    down,
    left,
    right
};

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

    animationCount--;
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
            currentPosition.y++;
            currentPosition.x--;
            SetCursorPosition(&currentPosition);
            animationOutput++;
            continue;
        }

        cout<<*animationOutput;
        currentPosition.x++;
        animationOutput++;
    }
}

void CheckForHits(AttackDirections, Vector2);

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

    ongoingAnimations[animationCount] = CreateAnimationValue(animationPosition, animationOutput,attackDuration);
    currentTime[animationCount] = 0;

    PrintAnimation(&ongoingAnimations[animationCount]);

    CheckForHits(attack,animationPosition);

    animationCount++;
}

void AnimatinStep(int delta)
{
    attackTimer+=delta;
    for(int i = 0; i<animationCount; i++)
    {
        currentTime[i]+=delta;

        if(currentTime[i] > ongoingAnimations[i].duration)
        {
            RemoveAnimation(i);
        }
    }
}

void RemoveEnemy(int);

void CheckForHits(AttackDirections direction, Vector2 position)
{
    Vector2 *direction_vector;

    switch(direction)
    {
    case AttackDirections::up:
    case AttackDirections::down:
        direction_vector = &vector_up;
        break;
    case AttackDirections::left:
    case AttackDirections::right:
        direction_vector = &vector_down;
        break;
    }

    for(int i = 0; i<3; i++)
    {
        unsigned char hit;

        if(hit = isEnemy(&position))
        {
            RemoveEnemy(hit-5);
        }

        AddVectorsDirectly(&position, direction_vector);
    }
}
