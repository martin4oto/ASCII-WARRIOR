using namespace std;

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

Enemy* enemiesAlive;






const char air = 0;
const char barrier = 1;
const char platform = 2;
const char player_index = 3;

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
        cout<<enemiesAlive[objectToPrint-5].visual;
    }
}
