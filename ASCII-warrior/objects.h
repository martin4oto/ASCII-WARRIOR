using namespace std;

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
}
