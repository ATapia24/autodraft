#include <iostream>
#include <vector>

using namespace std;

vector<char> listA;
vector<char> listB;

int main(){

listA.push_back('1');
listA.push_back('2');
listA.push_back('3');
listA.push_back('4');
listA.push_back('5');

listB.push_back('a');
listB.push_back('b');
listB.push_back('c');
//listB.push_back('d');

cout << "Single" << endl;

for(int a=0; a<listA.size(); a++)
{
	for(int b=0; b<listB.size(); b++)
	{
		cout << listA[a] << listB[b];
	}
}

cout << endl << "Split" << endl;

int totalCores = 2;

int C1Min = 0;
int C1Max = listA.size() / totalCores;

int C2Min = listA.size() / totalCores;
int C2Max = C2Min * 2; 

if((listA.size() % 2) == 1)
{
	C2Max += 1;
}

//cout << "Core 1 - Min: " << C1Min << " Max: " << C1Max << endl;
//cout << "Core 2 - Min: " << C2Min << " Max: " << C2Max << endl;
 

for(int a=C1Min; a<C1Max; a++)
{
        for(int b=0; b<listB.size(); b++)
        {
                cout << listA[a] << listB[b];
        }
}

cout << "-" << endl;

for(int a=C2Min; a<C2Max; a++)
{
        for(int b=0; b<listB.size(); b++)
        {
                cout << listA[a] << listB[b];
        }
}

cout << endl;
return 0;
}
