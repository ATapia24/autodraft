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

struct taskData{
	int TNumber;
        int TMax;
        int TMin;
        vector<Player> pg;
        vector<Player> sg;
        vector<Player> sf;
        vector<Player> pf;
        vector<Player> cr;
        vector<Player> gd;
        vector<Player> fd;
        vector<Player> ul;
};

//functions
void downloadData();
void loadPlayers(vector<Player>&);
void printPlayer(Player);
void printPlayers(vector<Player>);
void getPosition(vector<Player>, vector<Player>&, string);
void sortFPPG(vector<Player>&, char);
void sortFPPD(vector<Player>&, char);
void sortSalary(vector<Player>&, char);
void loadTaskData(taskData&, vector<Player>);


void *task(void *arg){
        
	taskData data = *(taskData*)arg;

	
	for(int pg=data.TMin; pg<data.TMax; pg++)
	{
        	for(int s=0; s<data.sg.size(); s++)
		{
        		for(int sf=0; sf<data.sf.size(); sf++)
			{
        			for(int pf=0; pf<data.pf.size(); pf++)
				{
        		
					for(int cr=0; cr<data.cr.size(); cr++)
					{
						//cout << data.TNumber << '\n';
					}
				}
			}
		}
			cout << data.TMin << '/' << data.TMax << endl;
	}
}

int const N_THREADS = 2;
	
int main(int argc, char *argv[]){

	cout << "Auto Drafter: Beta" << endl;	
	vector<Player> players;

	//update data
	if(argc >= 2){
		downloadData();
	}
	
	//load players
	loadPlayers(players);
	
	taskData t1;
	taskData t2;

	t1.TNumber = 1;
	t2.TNumber = 2;
	
	loadTaskData(t1, players);
	loadTaskData(t2, players);

	pthread_t thread1, thread2;
	
 	int i1, i2;
	
	
	i1 = pthread_create( &thread1, NULL, task, &t1);
	i2 = pthread_create( &thread2, NULL, task, &t2);

	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
}

//LOAD TASK DATA
//DESC. load players into task data and calc min/max 
void loadTaskData(taskData &data, vector<Player> players)
{
	vector<Player> pg;
        vector<Player> sg;
        vector<Player> sf;
        vector<Player> pf;
        vector<Player> cr;
        vector<Player> gd;
        vector<Player> fd;

        getPosition(players, pg, "PG");
        getPosition(players, sg, "SG");
        getPosition(players, sf, "SF");
        getPosition(players, pf, "PF");
        getPosition(players, cr, "C");
        getPosition(players, gd, "GD");
        getPosition(players, fd, "FD");
	
	data.pg = pg;
	data.sg = sg;
	data.sf = sf;
	data.pf = pf;
	data.cr = cr;
	data.gd = gd;
	data.fd = fd;
	data.ul = players;

	//calc min/max
	int dataOffset = pg.size() /N_THREADS;
	data.TMin = dataOffset * (data.TNumber - 1);
	data.TMax = dataOffset * data.TNumber;

	
        if((pg.size() % N_THREADS) > 0)
        {
		data.TMax += (pg.size() % N_THREADS);
        }
}

//GET POSITION
//DESC. pull position out of player vector into another
void getPosition(vector<Player> allPlayers, vector<Player> &posPlayers, string pos)
{
	for(int i=0; i<allPlayers.size(); i++)
	{	
		if(pos == "GD")
		{
			if((allPlayers[i].position == "PG") || (allPlayers[i].position == "SG"))
                        {
                                posPlayers.push_back(allPlayers[i]);
                        }
	
		}else if(pos == "FD")
		{
			if((allPlayers[i].position == "SF") || (allPlayers[i].position == "PF"))
                        {
                                posPlayers.push_back(allPlayers[i]);
                        }
		}
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
	cout << "Pos     : " << player.position << '\n';
	//cout << "Team    : " << player.team << '\n';
	//cout << "Opp     : " << player.opponent << '\n';
	//cout << "FPPG    : " << player.fppg << '\n';
	//cout << "FPPD    : " << player.fppd << '\n';
	cout << "Salary  : " << player.salary << '\n';
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

