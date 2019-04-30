#define _XOPEN_SOURCE
#define NUM_THREADS 6
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <crypt.h>
#include <functional>
#include <algorithm>
#include <vector>
#include <sys/stat.h>
#include <pthread.h>
#include "register.h"
#include "login.h"
#include "permutations.h"
using namespace std;

//Global definitions
vector<string> Words,Usernames, Hashes, Salts;
long long Limits[NUM_THREADS][2];
long long Counter=0;

//Multithreaded function to crack passwords.
void *CheckPasswords(void *threadid) {
	long tid;
	tid = (long)threadid;
	ofstream Results;
	Results.open("Results.txt", ios_base::app);
	for (long long i = Limits[tid][0]; i <= Limits[tid][1]; i++) {
		Counter++;
		for (unsigned int j = 0; j < Usernames.size(); j++) {
			//Encrypts each word with the users salt and checks if the two hashes match
			if (crypt(Words[i].c_str(), Salts[j].c_str()) == Hashes[j]) {
				cout << "Found a Password!" << endl;
				//Print user-name and password into a file.
				Results << "Username:" << Usernames[j] << " ";
				Results << "Password:" << Words[i] << endl;
				Hashes.erase(Hashes.begin() + j);
				Salts.erase(Salts.begin() + j);
				Usernames.erase(Usernames.begin() + j);
			}
		}
		//Prints the counter every 1000 words tested.
		if (Counter % 1000 == 0 && i !=0) {
			cout << "Thread: " << tid << endl;
			cout << Counter << " Words Tried" << endl;
			cout << Usernames.size() << " Users Uncracked" << endl << endl;
		}
	}
	//Exits threads.
   	pthread_exit(NULL);
}

//Function to attack a Shadow file. (Questions 4 and 5)
void Attack() {
	ifstream ShadowFile, DictionaryFile;
	string line, data, Salt, Hash, Username,Algorithm, Shadow, Dictionary;
	vector<string> CrackedUsernames;
	ifstream Results;
	Results.open("Results.txt", ios_base::app);
	int Temp = 1;
	int Selection=0;
	bool Found = false;
	struct stat buffer;
		//Selects an attack type. Dictionary or Brute
		cout << "Select an action: " << endl;
		cout << "1.Dictionary Attack" << endl;
		cout << "2.Brute Force" << endl;
		cout << "Selection: ";
		cin >> Selection;
		cout << endl;
	while (Selection != 1 && Selection != 2) {
		cout << "Error. Wrong Selection. Try Again" << endl << endl;
		cout << "Selection: ";
		cin >> Selection;
		system("clear");
		cout << "Select an action: " << endl;
		cout << "1.Dictionary Attack" << endl;
		cout << "2.Brute Force" << endl;
		cout << "Selection: ";
		cin >> Selection;
		cout << endl;
	}
	cout << "Enter the name of the Shadow File." << endl;
	cout << "Name: ";
	cin >> Shadow;
	ShadowFile.open(Shadow.c_str());
	while (!ShadowFile.is_open()) {
		cout << "Shadow couldn't be opened. Try Again." << endl;
		cout << "Enter the name of the Shadow File." << endl;
		cout << "Name: ";
		cin >> Shadow;
		ShadowFile.open(Shadow.c_str());
	}
	//Dictionary Attack
	if (Selection == 1) {
		//Dictionary to use
		cout << "Input the dictionary file name to use." << endl;
		cout <<"Dictionary Name: " << endl;
		cin >> Dictionary;
		DictionaryFile.open(Dictionary.c_str());
		while (!DictionaryFile.is_open()) {
			cout << "Dictionary couldn't be opened. Try Again!" << endl;
			cout << "Input the dictionary file name to use." << endl;
			cout <<"Dictionary Name: " << endl;
			cin >> Dictionary;
			DictionaryFile.open(Dictionary.c_str());
		}
		//need to figure out a better way to get words a few at a time=============
		while (getline(DictionaryFile, line)) {
			//Removes caridge return characters (this took me a while to figure out)
			removeCharsFromString(line, "\r");
			//Puts word in vector
			Words.push_back(line);
		}
		//==========================================================================
	//Brute force attack. VERY SLOW
	} else if (Selection == 2) {
		int Depth = 0;
		cout << "What password length should i brute force?" << endl;
		cout << "Length: ";
		cin >> Depth;
		//Recursion function that creates words.
		Recursion(Depth, "", "", Depth);
		cout << "All possible words calculated" << endl << endl;
	}

	//Checks which users are already cracked.
	while (getline(Results, line)){
		stringstream Resultstream(line);
		getline(Resultstream, data, ':');
		getline(Resultstream, Username, ' ');
		CrackedUsernames.push_back(Username);
	}
	//Gets each user entry
	while (getline(ShadowFile, line)) {
		//For each user gets User-name, Salt and Hash
		stringstream linestream(line);
		getline(linestream, Username, ':');
		getline(linestream, Hash, ':');
		stringstream Saltstream(Hash);
		getline(Saltstream, data, '$');
		getline(Saltstream, Algorithm, '$');
		Algorithm = "$" + Algorithm + "$";
		getline(Saltstream, Salt, '$');
		Salt = Algorithm + Salt;
		for (unsigned int i=0; i < CrackedUsernames.size(); i++){
			if (Username == CrackedUsernames[i]){
				Found = true;
			}
		}
		//Removes users which have already been cracked.
		if (Found == false){
			//Puts data into vectors.
			Usernames.push_back(Username);
			Hashes.push_back(Hash);
			Salts.push_back(Salt);
		}
	}
	//Divide words into n threads
	long long ThreadSize[NUM_THREADS];
	int NormalSize = (Words.size()/NUM_THREADS);
	int WordsLeft = Words.size() - (NormalSize*NUM_THREADS);
	for (int i=0; i < NUM_THREADS; i++){
		ThreadSize[i]=NormalSize;
	}
	ThreadSize[NUM_THREADS-1] = ThreadSize[NUM_THREADS-1] + WordsLeft;
	long long PreviousEnd = 0;
	for (int i=0; i < NUM_THREADS; i++){
		Limits[i][0] = PreviousEnd;
		Limits[i][1] = PreviousEnd + (ThreadSize[i]-1);
		PreviousEnd = Limits[i][1] + 1;
	}
	cout << "====Words Per Thread====" << endl;
	for (int i=0; i< NUM_THREADS ;i++){
		cout << "Thread " << i << " limits: " << Limits[i][0] << "-" << Limits[i][1] << endl;
	}
	//Create threads and start cracking passwords
	try {
		clock_t tStart = clock();
		pthread_t threads[NUM_THREADS];
		long rc;
		for(int i=0; i < NUM_THREADS; i++ ){
			rc = pthread_create(&(threads[i]), NULL, CheckPasswords, (void *)(intptr_t)i);
			if (rc){
				cout << "Error:unable to create thread," << rc << endl;
				exit(-1);
			}
		}
		pthread_exit(NULL);
		//Prints the run time of the program.
		cout << Counter << " Total Words Tried" << endl;
		printf("Time Elapsed: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
	} catch (const exception& Error){
			cout << "An error has occurred: " << Error.what() << endl;
	}
}

//Main Function
int main() {
	string Command;
	cout << "Would you like to login, register or attack?" << endl;
	cout << "Command: ";
	cin >> Command;
	while (Command != "Register" && Command != "register" && Command != "Login"
			&& Command != "login" && Command != "attack" && Command != "Attack") {
		cout << "Wrong input. Try Again." << endl;
		cout << "Command: ";
		cin >> Command;
	}
	//Chooses what to do
	if (Command == "Register" || Command == "register") {
		Register();
	} else if (Command == "Login" || Command == "login") {
		Login();
	} else if (Command == "Attack" || Command == "attack") {
		Attack();
	}
	return 0;
}
