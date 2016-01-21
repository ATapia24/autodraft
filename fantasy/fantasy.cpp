#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <pthread.h>

using namespace std;

struct Player{
int id;
string name;
string position;
string team;
string opponent;
string game;
string time;
double salary;
double fppg;
double fppd;
bool injured;
};

struct PlayerMatrix
{
	vector< vector<Player> > players;
};

class Lineup
{
	public:
		Lineup(PlayerMatrix _matrix);
		void print();
	private:
		PlayerMatrix matrix;
		vector<Player> players;
};

Lineup::Lineup(PlayerMatrix _matrix)
{
	matrix = _matrix;
	
	for(int i=0; i<8; i++)
	{
		players[i] = matrix.players[i][0];
	}
}

void Lineup::print()
{
	cout << "----------------------------------\n";
	
	for(int i=0; i<8; i++)
	{
		cout << players[i].position << ": " << players[i].name << '\n';
	}	
}

//functions
void downloadData();
void loadPlayers(vector<Player>&);
void printPlayer(Player);
void printPlayers(vector<Player>);
void getPosition(vector<Player>, vector<Player>&, string);
void sortFPPG(vector<Player>&, char);
void sortFPPD(vector<Player>&, char);
void sortSalary(vector<Player>&, char);
void rmLowFPPD(vector<Player>&);
void genLineup(PlayerMatrix);
void loadMatrix(vector<Player>, PlayerMatrix&);
	
int main(int argc, char *argv[]){

	cout << "Auto Drafter: Beta" << endl;	
	
	//update data
	if(argc >= 2){
		downloadData();
	}
	
	vector<Player> players;
	PlayerMatrix matrix;

	//load players and matrix
	loadPlayers(players);
	loadMatrix(players, matrix);

	//generate lineup
	genLineup(matrix);
		
}

//GENERATE LINEUP
//DESC, GENERATE LINEUP
void genLineup(PlayerMatrix matrix)
{
	Lineup lineup(matrix);
	lineup.print();	
}
//LOAD MATRIX
//DESC. LOAD PLAYERS INTO PLAYER MATRIX
void loadMatrix(vector<Player> players, PlayerMatrix &matrix)
{
	vector<Player> PG;
	vector<Player> SG;
	vector<Player> SF;
	vector<Player> PF;
	vector<Player> C;
	vector<Player> GD;
	vector<Player> FD;
	vector<Player> UL;


	getPosition(players, PG, "PG");	
	getPosition(players, SG, "SG");	
	getPosition(players, SF, "SF");	
	getPosition(players, PF, "PF");	
	getPosition(players, C, "C");	
	getPosition(players, GD, "GD");	
	getPosition(players, FD, "FD");	
	UL = players;

	//FIX******
	matrix.players.reserve(8);
	matrix.players[0].reserve(PG.size());
	cout << "fsedf" << endl;
	cout << matrix.players.size() << " : " << matrix.players[1].size() << endl;
	matrix.players[1].reserve(SG.size());
	cout << "2" << endl;
	matrix.players[2].reserve(SF.size());
	matrix.players[3].reserve(PF.size());
	matrix.players[4].reserve(C.size());
	matrix.players[5].reserve(GD.size());
	matrix.players[6].reserve(FD.size());
	matrix.players[7].reserve(UL.size());

	matrix.players[0] = PG;
	matrix.players[1] = SG;
	matrix.players[2] = SF;
	matrix.players[3] = PF;
	matrix.players[4] = C;
	matrix.players[5] = GD;
	matrix.players[6] = FD;
	matrix.players[7] = UL;
}

//REMOVE LOW FPPD
//DESC. REMOVE PLAYERS WITH LOWER FPPD
void rmLowFPPD(vector<Player> &players)
{
	//sort by fppd decsending
	sortFPPD(players, 'd');

	for(int i=0; i<players.size(); i++)
	{
		//dont check cuurent player in index j=i+1
		for(int j=(i+1); j<players.size(); j++)
		{
	
			if(players[i].fppg > players[j].fppg)
			{
				//remove player and adjust j
				players.erase(players.begin() + j);
				j--;
			}
		}
		
	}
}

//GET POSITION
//DESC. pull position out of player vector into another
void getPosition(vector<Player> allPlayers, vector<Player> &posPlayers, string pos)
{
	//LOOP THROUGH LIST
	for(int i=0; i<allPlayers.size(); i++)
	{	
		//GUARD
		if(pos == "GD")
		{
			if((allPlayers[i].position == "PG") || (allPlayers[i].position == "SG"))
                        {
                                posPlayers.push_back(allPlayers[i]);
                        }
	
		//FORWARD
		}else if(pos == "FD")
		{
			if((allPlayers[i].position == "SF") || (allPlayers[i].position == "PF"))
                        {
                                posPlayers.push_back(allPlayers[i]);
                        }
		}
		//PASSED POS
		else{

			if(allPlayers[i].position == pos)
			{
				posPlayers.push_back(allPlayers[i]);
			}
		}
	}
}

//SORT BY FPPG
//DESC. Sort by fppg by char ascending or descending
void sortFPPG(vector<Player> &players, char order)
{
        for(int i=0; i<players.size(); i++)
        {
                for(int j=0; j<players.size(); j++)
                {
                        if(order == 'a')
                        {
                                if(players[i].fppg < players[j].fppg)
                                {
                                        Player temp = players[i];
                                        players[i] = players[j];
                                        players[j] = temp;
                                }
                        }else
                        {
                                if(players[i].fppg > players[j].fppg)
                                {
                                        Player temp = players[i];
                                        players[i] = players[j];
                                        players[j] = temp;
                                }
                        }
                }
        }
}

//SORT BY FPPD
//DESC. Sort by fppd by char ascending or descending
void sortFPPD(vector<Player> &players, char order)
{
	for(int i=0; i<players.size(); i++)
	{
		for(int j=0; j<players.size(); j++)
		{
			if(order == 'a')
			{
				if(players[i].fppd < players[j].fppd)
				{
					Player temp = players[i];
					players[i] = players[j];
					players[j] = temp;
				}
			}else
			{
				if(players[i].fppd > players[j].fppd)
                                {
                                        Player temp = players[i];
                                        players[i] = players[j];
                                        players[j] = temp;
                                }
			}
		}
	}
} 

//SORT BY SALARY
//DESC. Sort by fppg by char ascending or descending
void sortSalary(vector<Player> &players, char order)
{
        for(int i=0; i<players.size(); i++)
        {
                for(int j=0; j<players.size(); j++)
                {
                        if(order == 'a')
                        {
                                if(players[i].salary < players[j].salary)
                                {
                                        Player temp = players[i];
                                        players[i] = players[j];
                                        players[j] = temp;
                                }
                        }else
                        {
                                if(players[i].salary > players[j].salary)
                                {
                                        Player temp = players[i];
                                        players[i] = players[j];
                                        players[j] = temp;
                                }
                        }
                }
        }
}


//PRINT PLAYER
//DESC. print player
void printPlayer(Player player)
{	
	cout << "Name    : " << player.name << '\n';
	//cout << "Pos     : " << player.position << '\n';
	//cout << "Team    : " << player.team << '\n';
	//cout << "Opp     : " << player.opponent << '\n';
	cout << "FPPG    : " << player.fppg << '\n';
	cout << "FPPD    : " << player.fppd << '\n';
	//cout << "Salary  : " << player.salary << '\n';
	//cout << "Injured : " << player.injured << '\n';			
}

//PRINT PLAYERS
//DESC. print player list
void printPlayers(vector<Player> players)
{
	for(int i=0; i<players.size(); i++)
	{
		printPlayer(players[i]);
		cout << "-------------------------------\n";
	}
}


//LOAD PLAYERS
//DESC. Load players into player vector from playerList.cvs
void loadPlayers(vector<Player> &playerVector)
{
	string line;
 	ifstream myfile ("data/playerList.cvs");
	int lineNumber = 0;  	

	if (myfile.is_open())
 	{
		while ( getline (myfile,line) )
		{
			if(lineNumber != 0)
			{		

			Player tempPlayer;
			size_t startPos, endPos, replacePos;
			char token = ',';				
			
			//ID
			tempPlayer.id = atoi(line.substr(6, 4).c_str());

			//NAME
  			startPos = line.find_first_of(token);
			line[startPos] = '#';

			replacePos = line.find_first_of(token);
			line[replacePos] = ' ';
			
			endPos = line.find_first_of(token);
				
			tempPlayer.name = line.substr(startPos + 1, endPos - startPos - 1);

			//POSITION
			startPos = line.find_first_of(token);
			line[startPos] = '#';
				
			endPos = line.find_first_of(token);
				
			tempPlayer.position = line.substr(startPos + 1, endPos - startPos - 1);
			

			//TEAM
			startPos = line.find_first_of(token);
                        line[startPos] = '#';

                        endPos = line.find_first_of(token);

                        tempPlayer.team = line.substr(startPos + 1, endPos - startPos - 1);


			//OPPONENT
			startPos = line.find_first_of(token);
                        line[startPos] = '#';

                        endPos = line.find_first_of(token);

                        tempPlayer.opponent = line.substr(startPos + 1, endPos - startPos - 1);

				
			//GAME
                        startPos = line.find_first_of(token);
                        line[startPos] = '#';

                        endPos = line.find_first_of(token);

                        tempPlayer.game = line.substr(startPos + 1, endPos - startPos - 1);


			//TIME
                        startPos = line.find_first_of(token);
                        line[startPos] = '#';

                        endPos = line.find_first_of(token);

                        tempPlayer.time = line.substr(startPos + 1, endPos - startPos - 1);


			//SALARY
                        startPos = line.find_first_of(token);
                        line[startPos] = '#';
                        endPos = line.find_first_of(token);

                        tempPlayer.salary = atoi(line.substr(startPos + 1, endPos - startPos - 1).c_str());

				
			//FPPG
                        startPos = line.find_first_of(token);
                        line[startPos] = '#';

                        endPos = line.find_first_of(token);

                        tempPlayer.fppg = atof(line.substr(startPos + 1, endPos - startPos - 1).c_str());


			//INJURY STATUS
			startPos = line.find_first_of(token);
                        line[startPos] = '#';

                        endPos = line.find_first_of(token);

                        string injured = line.substr(startPos + 1, endPos - startPos - 1);
				
			if(injured[1] != ' ')
			{
				tempPlayer.injured = 1;
			
			}else {tempPlayer.injured = 0;}
			

			//CALC FPPD
			tempPlayer.fppd = tempPlayer.fppg / tempPlayer.salary;
			

			//add to vector
			playerVector.push_back(tempPlayer);
			
			}			

			lineNumber++;
		}

	
	myfile.close();
	}
	else cout << "Unable to open playerList.cvs"; 

}

void downloadData()
{
	//get contest id
	string contestLink, contestId;
	cout << "Enter contest link: ";
	cin >> contestLink;
	contestId = contestLink.substr(46, 6);

	cout << "Updating player data... \n";
	
	//download yahoo cvs file
	string cvsLink = "wget https://dfyql-ro.sports.yahoo.com/v2/export/contestPlayers?contestId=" + contestId + " -q -O data/playerList.cvs";
	system(cvsLink.c_str());
}

