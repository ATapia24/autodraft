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
	int iter;
	pthread_t thread;
        vector<Player> pg;
        vector<Player> sg;
        vector<Player> sf;
        vector<Player> pf;
        vector<Player> cr;
        vector<Player> gd;
        vector<Player> fd;
        vector<Player> ul;
};

struct Lineup{
	Player pg;
	Player sg;
	Player sf;
	Player pf;
	Player cr;
	Player gd;
	Player fd;
	Player ul;
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
void loadThreads(vector<taskData>&, vector<Player>);
void printLineup(Lineup);

//TASK - ALG
//DESC. Thread function (calcs)
int const N_THREADS = 8;
void *task(void *arg){
        
	taskData data = *(taskData*)arg;
	Lineup lineup;
	
	for(int pg=data.TMin; pg<data.TMax; pg++)
	{
		lineup.pg = data.pg[pg];
		
        	for(int sg=0; sg<data.sg.size(); sg++)
		{
			lineup.sg = data.sg[sg];
					
        		for(int sf=0; sf<data.sf.size(); sf++)
			{
				lineup.sf = data.sf[sf];
		
        			for(int pf=0; pf<data.pf.size(); pf++)
				{
					lineup.pf = data.pf[pf];
        		
					for(int cr=0; cr<data.cr.size(); cr++)
					{
						lineup.cr = data.cr[cr];
	
						for(int gd=0; gd<data.gd.size(); gd++)
						{
							//while gd player is not == pg or sg and in range of gd vector
							while(((data.gd[gd].id == data.pg[pg].id)
							|| (data.gd[gd].id == data.sg[sg].id))
							&& gd < (data.gd.size()-1))
								{
									gd++;
								}
							
							lineup.gd = data.gd[gd];
			
							for(int fd=0; fd<data.fd.size(); fd++)
							{
								//while gd playe ros != sf or pf and in range of gd vector
								while(((data.fd[fd].id == data.sf[sf].id)
                                                        	|| (data.fd[fd].id == data.pf[pf].id))
                                                        	&& fd < (data.fd.size()-1))
                                                                {
                                                                        fd++;
                                                                }
								
								lineup.fd = data.fd[fd];
				
								for(int ul=0; (ul<data.ul.size()); ul++)
								{	
									//while ul player is not in lineup and ul is in range
									/*while(
									((data.ul[ul].id == data.pg[pg].id)
                                                                	|| (data.ul[ul].id == data.sg[sg].id)
                                                                	|| (data.ul[ul].id == data.sf[sf].id)
                                                                	|| (data.ul[ul].id == data.pf[pf].id)
                                                                	|| (data.ul[ul].id == data.cr[cr].id))
                                                                	&& ul < (data.ul.size()-1)
									)
                                                           		{
                                                                        	ul++;
                                                                	}*/

									lineup.ul = data.ul[ul];
									
									//CENTER
								}	
							}
						}
					}
				}
			}
		}
			if(data.TNumber == N_THREADS)
			{
				cout << pg+1 <<  '/' << data.TMax << endl;
			}
	}

	cout << "Thread " << data.TNumber << " done." << endl; 
}
	
int main(int argc, char *argv[]){

	cout << "Auto Drafter: Beta" << endl;	
	
	vector<Player> players;
	vector<taskData> threads;

	//update data
	if(argc >= 2){
		downloadData();
	}
	
	//load players
	loadPlayers(players);
	
	//load threads
	loadThreads(threads, players);
}

//PRINT LINEUP
//DESC. Print lineup
void printLineup(Lineup lineup)
{
	cout << "-----------------------------------------\n";
	cout << "PG: " << lineup.pg.name << '\n';
	cout << "SG: " << lineup.sg.name << '\n';
	cout << "SF: " << lineup.sf.name << '\n';
	cout << "PF: " << lineup.pf.name << '\n';
	cout << "CR: " << lineup.cr.name << '\n';
	cout << "GD: " << lineup.gd.name << '\n';
	cout << "FD: " << lineup.fd.name << '\n';
	cout << "UL: " << lineup.ul.name << '\n';
}

//LOAD THREADS
//DESC. load/setup pthreads
void loadThreads(vector<taskData> &threads, vector<Player> players)
{
	for(int i=0; i<N_THREADS; i++)
        {
		//set thread vector size or segfault (if seg set on threads initilize)
		threads.reserve(N_THREADS);
                taskData tempT;
               	
		//set thread number
		tempT.TNumber = i + 1;
                
		//load task data
		loadTaskData(tempT, players);
                
		//add to vectot
		threads.push_back(tempT);
               
		 //start threads
		threads[i].iter = pthread_create(&threads[i].thread, NULL, task, &threads[i]);
        }

	//wait for all threads to finish to continue
        for(int i=0; i<N_THREADS; i++)
        {
                pthread_join(threads[i].thread,NULL);
        }
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

	//give last thread the remainer	
        if((pg.size() % N_THREADS) > 0)
        {
		data.TMax += (pg.size() % N_THREADS);
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

