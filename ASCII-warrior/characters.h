int enemyTypeCount = 1;

enum EnemyTypes
{
    Walker
};


struct Vector2
{
    int x;
    int y;
};

struct Animation
{
    int duration;
    Vector2 position;
    char *output;
};

struct Player
{
    int jumpsLeft;
    int HP;
    Vector2 *position;
    Vector2 verticalMomentum;
};

//extraInfo:
//(walker): extraInfo-walking direction
struct Enemy
{
    EnemyTypes type;
    int HP;
    Vector2 position;
    int extraInfo;
};


Animation *CreateAnimation(Vector2 position, char* out, int duration)
{
    Animation *finished = new Animation;

    *finished ={
        duration,
        position,
        out
    };

    return finished;
}

Animation CreateAnimationValue(Vector2 position, char* out, int duration)
{
    return {
        duration,
        position,
        out
    };
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
