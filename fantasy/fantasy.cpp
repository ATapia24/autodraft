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
double aFppg;
double fppd;
bool injured;
int lineupPos;
};

struct Team{
string name;
string nameAbbr;
string city;
double oppg;
};

struct PlayerMatrix
{
	vector< vector<Player> > players;
	vector<string> position;
};

//functions
void downloadData();
void downloadAdjustData();
void loadTeams(vector<Team>&);
void parseTeamOppgData(vector<Team>&);
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


class Lineup
{
	public:
		Lineup(PlayerMatrix _matrix);
		vector<int> lowFPPG();
		int salary();
		double fppg();
		void print();
		int setPlayer(int, int);
		vector<Player> getPlayers();
	private:
		PlayerMatrix matrix;
		vector<Player> players;
		vector<int> fppgOrder;
};

//CONSTRUCTOR ***NEED TO FIX BACKUP STARTING POS***
//DESC. set lineup to first element in matrix for each slot
Lineup::Lineup(PlayerMatrix _matrix)
{
	Player temp;
	temp.id = 0;
	temp.name = "Temp Player";
	temp.fppg = 0;
	temp.fppd = 0;	
	//set martx and players size
	matrix = _matrix;
	players.resize(8);
	
	//set lineup to first element int matrix
	for(int i=0; i<8; i++)
	{

		//players[i] = matrix.players[i][0];
		players[i] = temp;
		players[i].lineupPos = i;
	}
}
//GET PLAYER
//DESC. return players vector
vector<Player> Lineup::getPlayers()
{
	return players;
}
//LOW FPPG
//DESC. return low fppg in lineup
vector<int> Lineup::lowFPPG()
{	vector<int> fppgOrder;
	fppgOrder.resize(8);
	vector<Player> temp = players;
	
	//sort by fppg ascending
	sortFPPG(temp, 'a');
        
	//set player pos order
	for(int i=0; i<8; i++)
	{
		fppgOrder[i] = temp[i].lineupPos;
	}
	return fppgOrder;
}
//FPPG
//DESC. return fppg sum
double Lineup::fppg()
{
	int sum=0;
	for(int i=0; i<8; i++)
	{
		sum += players[i].fppg;
	}
	return sum;
}
//SALARY
//DESC. return salary sum
int Lineup::salary()
{
	int sum=0;
	for(int i=0; i<8; i++)
	{
		sum += players[i].salary;
	}
	return sum;
}
//PRINT
//DESC. print lineup
void Lineup::print()
{
	cout << "----------------------------------\n";
	
	for(int i=0; i<8; i++)
	{
		if(i==5){cout<<"Reserve - \n";}
		cout << matrix.position[i] << ": " << players[i].name << " : " << players[i].fppg << endl;
	}
	cout << "Salary   : " << salary() << endl; 
	cout << "FPPG     : " << fppg() << endl;
	cout << "AVG FPPG : " << (fppg()/8) << endl;
}

//SET PLAYER
//DESC. set player in lineup
int Lineup::setPlayer(int pos, int index)
{
	int offset=0;
	bool inLineup=1;

	while(inLineup)
	{
		bool flag=0;
	
		for(int i=0; i<8; i++)
		{
			if(matrix.players[pos].size() < (index+offset)+1)
			{
				return -1;	
			}

			if(players[i].name == matrix.players[pos][index + offset].name)
			{
					flag=1;
					offset++;
			}
		}

		if(!flag)
		{
			inLineup = 0;
		}	
	}
	
	players[pos] = matrix.players[pos][index + offset];
	players[pos].lineupPos = pos;
	
	return offset;
}

int main(int argc, char *argv[]){

	cout << "Auto Drafter: Beta" << endl;	
	
	//update data
	if(argc >= 2){
		downloadData();
	}
	if(argc >= 3){
		downloadAdjustData();
	}
	
	//players	
	vector<Player> players;
	PlayerMatrix matrix;
	
	//teams
	vector<Team> teams(30);
	//parseTeamOppgData(teams);
	
	//load players and matrix
	loadPlayers(players);
	loadMatrix(players, matrix);
	
	//generate lineup
	genLineup(matrix);
		
}

//PARSE TEAM DATA
//DESC. parse team data into teams vector
void parseTeamOppgData(vector<Team> &teams)
{
	ifstream file;
	string line;
	file.open("data/teamDef.html");
	bool startFlag=0;
	int lineCount=1;

	
	int teamIndex=0;
	int offset = 10;
	int teamNameOffset=2;
	int teamOPPGOffset=3;

	while(getline(file, line))
	{
		if((line.find(">1</td>") !=string::npos) && !startFlag)
		{
			startFlag = 1;
		}
		else if(startFlag && (lineCount < 300))
		{
			lineCount++;

			//TEAM NAME
			if(lineCount == teamNameOffset)
			{
				//add offset
				teamNameOffset += offset;
				
				string name;
				int endPos;
				
				//parse team name form line
				name = line.substr(44, 15);
				endPos = name.find('\"');
				name = name.substr(0, endPos);
				
				teams[teamIndex].name = name;
			}

			//OPPG
			if(lineCount == teamOPPGOffset)
			{
				teamOPPGOffset += offset;

				string oppg;
				int endPos;
				
				//parse team oppg from line
				oppg = line.substr(38, 10);
				endPos = oppg.find('\"');
				oppg = oppg.substr(0, endPos);
				
				//set oppg change string to c string then double using atof 
				teams[teamIndex].oppg = atof(oppg.c_str());
			}
			
			//++team index
			if(lineCount % offset == 0)
			{
				teamIndex++;
			}
		}
	}
	
	file.close();
	
}

//GENERATE LINEUP
//DESC, GENERATE LINEUP
void genLineup(PlayerMatrix matrix)
{
	Lineup lineup(matrix), highLineup(matrix);
	
	//player pos index
	vector<int> index;
	index.resize(8);

	vector<int> fppgOrder;
	fppgOrder.resize(8);

	int orderIndex = 0;
	int offset=0;

	for(int i=0; i<8; i++)
	{
		index[i] = 0;
	}
	
	bool maxed = 0;
	bool posMaxed = 0;
	while(!maxed)
	{
		while((lineup.salary()<=200) && !posMaxed)
		{
			//update fppg list order
			fppgOrder = lineup.lowFPPG();
			
			//++index for current orderindex
			index[(fppgOrder[orderIndex])]++;
			
			//set player
			offset = lineup.setPlayer(fppgOrder[orderIndex],index[(fppgOrder[orderIndex])]);
			
			if(offset == -1)
			{
				posMaxed = 1;
			}else
			{
				index[orderIndex] += offset;
			}	
			if(lineup.salary()<=200)
			{
				highLineup = lineup;
			}
		}
			
		posMaxed = 0;
		orderIndex++;
		if((orderIndex > 8)){maxed=1;}
		
	}

	lineup = highLineup;
	lineup.print();
	
}
//LOAD MATRIX
//DESC. LOAD PLAYERS INTO PLAYER MATRIX
void loadMatrix(vector<Player> players, PlayerMatrix &matrix)
{   
	//player vectors
	vector<Player> PG;
	vector<Player> SG;
	vector<Player> SF;
	vector<Player> PF;
	vector<Player> C;
	vector<Player> GD;
	vector<Player> FD;
	vector<Player> UL;

	//fill player vectors
	getPosition(players, PG, "PG");	
	getPosition(players, SG, "SG");	
	getPosition(players, SF, "SF");	
	getPosition(players, PF, "PF");	
	getPosition(players, C, "C");	
	getPosition(players, GD, "GD");	
	getPosition(players, FD, "FD");	
	UL = players;


	//set matrix sizes
	matrix.players.resize(8);
	matrix.players[0].resize(PG.size());
	matrix.players[1].resize(SG.size());
	matrix.players[2].resize(SF.size());
	matrix.players[3].resize(PF.size());
	matrix.players[4].resize(C.size());
	matrix.players[5].resize(GD.size());
	matrix.players[6].resize(FD.size());
	matrix.players[7].resize(UL.size());

	//set matrix
	matrix.players[0] = PG;
	matrix.players[1] = SG;
	matrix.players[2] = SF;
	matrix.players[3] = PF;
	matrix.players[4] = C;
	matrix.players[5] = GD;
	matrix.players[6] = FD;
	matrix.players[7] = UL;

	matrix.position.resize(8);
	matrix.position[0] = "PG";
	matrix.position[1] = "SG";
	matrix.position[2] = "SF";
	matrix.position[3] = "PF";
	matrix.position[4] = "C";
	matrix.position[5] = "GD";
	matrix.position[6] = "FD";
	matrix.position[7] = "UL";

	for(int i=0; i<8; i++)
	{
		rmLowFPPD(matrix.players[i]);
	}
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
			if(!tempPlayer.injured && tempPlayer.team != "BOS" && tempPlayer.team != "PHI"){
				playerVector.push_back(tempPlayer);
			}
			
			}			

			lineNumber++;
		}

	
	myfile.close();
	}
	else cout << "Unable to open playerList.cvs"; 

}

//DOWNLOAD DATA
//DESC. download contest data
void downloadData()
{
	//get contest id
	string contestLink, contestId;
	cout << "Enter contest Link or Id: ";
	cin >> contestLink;
	if(contestLink.length() == 7)
	{
		contestId = contestLink;
	}else{
		contestId = contestLink.substr(46, 7);
	}

	//download yahoo cvs file
	cout << "Updating player data...\n";
	string cvsLink = "wget https://dfyql-ro.sports.yahoo.com/v2/export/contestPlayers?contestId=" + contestId + " -q -O data/playerList.cvs";
	system(cvsLink.c_str());
}

//DOWNLOAD ADJUST DATA
//DESC. download adjust player data
void downloadAdjustData()
{
	cout << "Updating team defensive data...\n";
	system("wget https://www.teamrankings.com/nba/stat/opponent-points-per-game -q -O data/teamDef.html");
}








