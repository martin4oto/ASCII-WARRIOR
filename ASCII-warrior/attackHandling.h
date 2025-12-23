#include <mapGeneration.h>
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
