struct Vector2
{
    int x;
    int y;
};

struct Player
{
    char visual;
    int jumpsLeft;
    int HP;
    Vector2 *position;
    Vector2 verticalMomentum;
};

struct Enemy
{
    int HP;
    char visual;
};

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

bool AreVectorsEqual(Vector2 *v1, Vector2 *v2)
{
    return (v1->x==v2->x)&&(v1->y==v2->y);
}

void  ZeroVector(Vector2 *v)
{
    v->x = 0;
    v->y = 0;
}
