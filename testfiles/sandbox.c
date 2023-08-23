#include <iostream>
#include <vector>
#include <string>

using namespace std;

int x = 7;
// int *P= &x;
int *P2 = &x;
// int P3 = &x;
int y = 6;
int *py = &y;


int main()
{

    cout << " x "<< x << " ";
    // cout << " x "<< x << " ";
    cout << " &x=p2 "<< P2 << " ";
    cout << " *p2 "<< *P2 << " ";
    cout << " py "<< py << " ";
    cout << " y "<< y << " ";
    cout << endl;
}
