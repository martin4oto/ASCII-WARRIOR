int enemyTypeCount = 4;
int bossTypes = 1;
const int bossSize = 3;

enum EnemyTypes
{
    Walker,
    Crawler,
    Flier,
    Jumper,

    Boss
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
