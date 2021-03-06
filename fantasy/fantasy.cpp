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
double realFppg;
double aFppg;
double fppd;
bool injured;
int lineupPos;
};

struct Team{
string name;
string abbr;
string city;
double oppg;
double oppgAdjust;
double defEf;
double defEfAdjust;
};

struct PlayerMatrix
{
	vector< vector<Player> > players;
	vector<string> position;
};

//functions
void downloadData();
void downloadAdjustData();
void teamInit(vector<Team>&);
void loadTeams(vector<Team>&);
void parseTeamOppgData(vector<Team>&);
void parseTeamDefEfData(vector<Team>&);
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
void adjustPlayers(vector<Player>&, vector<Team>);

class Lineup
{
	public:
		Lineup(PlayerMatrix _matrix);
		vector<int> lowFPPG(int, int);
		int salary();
		double fppg();
		double fppgReal();
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
	temp.position = "N/A";	
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
vector<int> Lineup::lowFPPG(int start, int end)
{	
	//get size of range
	int size = end - start + 1;
	
	//adjust size and end for when start==0
	if(!start)
	{
	//TEST#	size++;
		end++;
	}
	//vectors
	vector<int> fppgOrder(size);
	vector<Player> temp;
	
	//get players from range start-end
	for(int i=start; i<end; i++)
	{
		temp.push_back(players[i]);
	}
	
	//sort by fppg ascending
	sortFPPG(temp, 'a');
        

	//set player pos order
	for(int i=start; i<end; i++)
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

//FPPG REAL
//DESC. return real fppg sum
double Lineup::fppgReal()
{
	int sum=0;
	for(int i=0; i<8; i++)
	{
		sum += players[i].realFppg;
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
	
	cout << "----------------------------------\n";
	cout << "Salary   : " << salary() << endl;

	cout << "----------------------------------\n";
	cout << "Adjusted FPPG" << endl;
	cout << "FPPG     : " << fppg() << endl;
	cout << "AVG FPPG : " << (fppg()/8)<< endl;
	
	cout << "----------------------------------\n";
	cout << "True FPPG" << endl;
	cout << "FPPG     : " << fppgReal() << endl;
	cout << "AVG FPPG : " << (fppgReal()/8) << endl;
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
		//downloadAdjustData();
	}

	//players	
	vector<Player> players;
	PlayerMatrix matrix;
	
	//teams
	vector<Team> teams(30);
	teamInit(teams);
	parseTeamOppgData(teams);
	parseTeamDefEfData(teams);

	//load players and matrix
	loadPlayers(players);
	adjustPlayers(players, teams);
	loadMatrix(players, matrix);

	//generate lineup
	genLineup(matrix);
}

//ADJUST PLAYERS
//DESC. adjust player stats based on opponent
void adjustPlayers(vector<Player> &players, vector<Team> teams)
{
	for(int i=0; i<players.size(); i++)
	{
		for(int j=0; j<teams.size(); j++)
		{
			if(players[i].opponent == teams[j].abbr)
			{
				players[i].fppg += teams[j].oppgAdjust;
				players[i].fppg += teams[j].defEfAdjust;
				j = teams.size()-1;
			}
		}
	}

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

	vector<Team> temp(30);
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

			//TEAM CITY
			if(lineCount == teamNameOffset)
			{
				//add offset
				teamNameOffset += offset;
				
				string city;
				int endPos;
				
				//parse team name form line
				city = line.substr(44, 15);
				endPos = city.find('\"');
				city = city.substr(0, endPos);
				
				//FORMATING EXCEPTIONS
				if(city == "Okla City")
				{
					city = "Oklahoma City";
				}
				
				temp[teamIndex].city = city;
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
				temp[teamIndex].oppg = atof(oppg.c_str());
			}
			
			//++team index
			if(lineCount % offset == 0)
			{
				teamIndex++;
			}
		}
	}
	
	file.close();

	//set data
	for(int i=0; i<temp.size(); i++)
	{
		for(int j=0; j<teams.size(); j++)
		{
			if(teams[j].city == temp[i].city)
			{
				teams[j].oppg = temp[i].oppg;
			}
		}
	}

	//calc multiplier
	double avgOppg=0;
	for(int i=0; i<teams.size(); i++)
	{
		avgOppg += teams[i].oppg;
	}
	avgOppg = avgOppg/30;
	for(int i=0; i<teams.size(); i++)
	{
		teams[i].oppgAdjust = (teams[i].oppg-avgOppg)/2;
	}
}

//PARSE TEAM DATA
//DESC. parse team data into teams vector
void parseTeamDefEfData(vector<Team> &teams)
{
	ifstream file;
	string line;
	file.open("data/teamDefEf.html");
	bool startFlag=0;
	int lineCount=1;

	vector<Team> temp(30);
	int teamIndex=0;
	int offset = 10;
	int teamNameOffset=2;
	int teamDEFEFOffset=3;

	while(getline(file, line))
	{
		if((line.find(">1</td>") !=string::npos) && !startFlag)
		{
			startFlag = 1;
		}
		else if(startFlag && (lineCount < 300))
		{
			lineCount++;

			//TEAM CITY
			if(lineCount == teamNameOffset)
			{
				//add offset
				teamNameOffset += offset;
				
				string city;
				int endPos;
				
				//parse team name form line
				city = line.substr(44, 15);
				endPos = city.find('\"');
				city = city.substr(0, endPos);
				
				//FORMATING EXCEPTIONS
				if(city == "Okla City")
				{
					city = "Oklahoma City";
				}
				
				temp[teamIndex].city = city;
			}

			//OPPG
			if(lineCount == teamDEFEFOffset)
			{
				teamDEFEFOffset += offset;

				string defEf;
				int endPos;
				
				//parse team oppg from line
				defEf = line.substr(38, 10);
				endPos = defEf.find('\"');
				defEf= defEf.substr(0, endPos);
				
				//set oppg change string to c string then double using atof 
				temp[teamIndex].defEf = atof(defEf.c_str());
			}
			
			//++team index
			if(lineCount % offset == 0)
			{
				teamIndex++;
			}
		}
	}
	
	file.close();

	//set data
	for(int i=0; i<temp.size(); i++)
	{
		for(int j=0; j<teams.size(); j++)
		{
			if(teams[j].city == temp[i].city)
			{
				teams[j].defEf = temp[i].defEf;
			}
		}
	}

	//calc multiplier
	double avgDefEf=0;
	for(int i=0; i<teams.size(); i++)
	{
		avgDefEf += teams[i].defEf;
	}
	avgDefEf = avgDefEf/30;
	for(int i=0; i<teams.size(); i++)
	{
		teams[i].defEfAdjust = (teams[i].defEf-avgDefEf) * 100;
	}
}

//GENERATE LINEUP
//DESC, GENERATE LINEUP
void genLineup(PlayerMatrix matrix)
{
	Lineup lineup(matrix), highLineup(matrix);
	
	int size = 8;
	int start=0;
	int end=7;

	//player pos index
	vector<int> index(size);
	
	vector<int> fppgOrder(size), lastOrder(size);
	int positionIndex = 0;
	int currentPosition = 0;
	int positionOffset;
	int currentPos = 0;
	int salary = 200;
	bool posMaxed=0, lineupMaxed=0;
	int  count =0;
	while(!lineupMaxed && count < 500)
	{
		//get fppg ordered vector for spots 0-4
		fppgOrder = lineup.lowFPPG(start,end);
		
		if(lastOrder != fppgOrder)
		{
			currentPos=0;
			lastOrder=fppgOrder;
		}
		
		//set player fppg[currentPos] = position, positionIndex = index of position list
		positionOffset = lineup.setPlayer(fppgOrder[currentPos], positionIndex);
		
		//if at the end of current lisy set pos maxed
		if(positionOffset == -1){
			posMaxed=1;
		}

		//if there is an overshoot revert change and set pos maxed
		if(lineup.salary() > salary){
			posMaxed=1;
			lineup.setPlayer(fppgOrder[currentPos], positionIndex-1);
		}
			
		if(!posMaxed){
			//go on to next player in same list
			positionIndex += positionOffset;
		}else{
			currentPos++;	
		}

		//set lineup to maxed pos goes past size
		if(currentPos>(size-1)){
			lineupMaxed=1;	
		}
		

		//set lineup if valid
		if( (lineup.salary() <= salary) && (lineup.fppg() > highLineup.fppg()) )
		{
			highLineup = lineup;
		}

		count++;
	}
	
	highLineup.print();
		
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
	//sortSalary(players, 'a');
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
			
			//REAL FPPG
			tempPlayer.realFppg = tempPlayer.fppg;

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
			if(!tempPlayer.injured){
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
	system("wget https://www.teamrankings.com/nba/stat/defensive-efficiency -q -O data/teamDefEf.html");
}

//TEAM INIT
//DESC. intitialize teams
void teamInit(vector<Team> &teams)
{
	teams[0].name = "Hawks";
	teams[0].abbr = "ATL";
	teams[0].city = "Atlanta";

	teams[1].name = "Nets";
	teams[1].abbr = "BKN";
	teams[1].city = "Brooklyn";

	teams[2].name = "Celtics";
	teams[2].abbr = "BOS";
	teams[2].city = "Boston";

	teams[3].name = "Hornets";
	teams[3].abbr = "CHA";
	teams[3].city = "Charlotte";

	teams[4].name = "Bulls";
	teams[4].abbr = "CHI";
	teams[4].city = "Chicago";

	teams[5].name = "Mavericks";
	teams[5].abbr = "DAL";
	teams[5].city = "Dallas";

	teams[6].name = "Nuggets";
	teams[6].abbr = "DEN";
	teams[6].city = "Denver";

	teams[7].name = "Pistons";
	teams[7].abbr = "DET";
	teams[7].city = "Detroit";

	teams[8].name = "Warriors";
	teams[8].abbr = "GS";
	teams[8].city = "Golden State";

	teams[9].name = "Rockets";
	teams[9].abbr = "HOU";
	teams[9].city = "Houston";

	teams[10].name = "Pacers";
	teams[10].abbr = "IND";
	teams[10].city = "Indiana";

	teams[11].name = "Clippers";
	teams[11].abbr = "LAC";
	teams[11].city = "LA Clippers";

	teams[12].name = "Lakers";
	teams[12].abbr = "LAL";
	teams[12].city = "LA Lakers";

	teams[13].name = "Grizzlies";
	teams[13].abbr = "MEM";
	teams[13].city = "Memphis";

	teams[14].name = "Heat";
	teams[14].abbr = "MIA";
	teams[14].city = "Miami";

	teams[15].name = "Pelicans";
	teams[15].abbr = "NOP";
	teams[15].city = "New Orleans";

	teams[16].name = "Knicks";
	teams[16].abbr = "NYK";
	teams[16].city = "New York";

	teams[17].name = "Thunder";
	teams[17].abbr = "OKC";
	teams[17].city = "Oklahoma City";

	teams[18].name = "Magic";
	teams[18].abbr = "ORL";
	teams[18].city = "Orlando";

	teams[19].name = "Suns";
	teams[19].abbr = "PHO";
	teams[19].city = "Phoenix";

	teams[20].name = "Trail Blazers";
	teams[20].abbr = "POR";
	teams[20].city = "Portland";

	teams[21].name = "Kings";
	teams[21].abbr = "SAC";
	teams[21].city = "Sacramento";

	teams[22].name = "Spurs";
	teams[22].abbr = "SA";
	teams[22].city = "San Antonio";

	teams[23].name = "Raptors";
	teams[23].abbr = "TOR";
	teams[23].city = "Toronto";

	teams[24].name = "Jazz";
	teams[24].abbr = "UTA";
	teams[24].city = "Utah";

	teams[25].name = "Wizards";
	teams[25].abbr = "WAS";
	teams[25].city = "Washington";

	teams[26].name = "Bucks";
	teams[26].abbr = "MIL";
	teams[26].city = "Milwaukee";

	teams[27].name = "Timberwolves";
	teams[27].abbr = "MIN";
	teams[27].city = "Minnesota";

	teams[28].name = "76ers";
	teams[28].abbr = "PHI";
	teams[28].city = "Philadelphia";

	teams[29].name = "Cavaliers";
	teams[29].abbr = "CLE";
	teams[29].city = "Cleveland";
}








