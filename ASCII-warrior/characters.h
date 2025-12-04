struct Vector2
{
    int x;
    int y;
};

struct Player
{
    int HP;
    char visual;
    Vector2 *position;
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
