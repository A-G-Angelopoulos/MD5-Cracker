#ifndef REGISTER_H_
#define REGISTER_H_

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <crypt.h>
#include <algorithm>
#include "removeChars.h"
using namespace std;

//Function to Register into a shadow file. (Question 2)
void Register() {
	//Opens a shadow file.
	fstream Shadow;
	string line,SUsername;
	Shadow.open("shadow.txt");
	if (!Shadow.is_open()) {
		cout << "Error opening shadow file...";
	}
	string Username, Password, PasswordVerify;
	//Gets a User-name
	cout << "Enter your desired User-name: ";
	cin >> Username;
	//Checks if User-name already exists.
	while (getline(Shadow, line)) {
		stringstream linestream(line);
		getline(linestream, SUsername, ':');
		if (SUsername == Username){
			cout << "User-name already exists. Try again." << endl;
			cout << "Enter your desired User-name: ";
			cin >> Username;
			cout << endl;
			Shadow.close();
			Shadow.open("shadow.txt"); 
		}
	}
	Shadow.close();
	Shadow.open("shadow.txt", ios_base::app);
	//Gets a password.
	cout << "Enter a password: ";
	cin >> Password;
	cout << endl;
	//Verifies it.
	cout << "Enter Password again: ";
	cin >> PasswordVerify;
	cout << endl;
	//Checks if passwords match.
	while (Password != PasswordVerify) {
		cout << "Passwords do not match. Try again." << endl;
		cout << "Enter a password: ";
		cin >> Password;
		cout << endl;
		cout << "Enter Password again: ";
		cin >> PasswordVerify;
		cout << endl;
	}
	//Code to get the current time and put it into Salt variable
	time_t t = time(&t);
	//Gets local time
	struct tm *timeinfo = localtime(&t);
	char buffer[80];
	//Sets it as Seconds Minutes Hours and Date
	strftime(buffer, sizeof(buffer), "%S%M%I%d", timeinfo);
	string Time(buffer);
	removeCharsFromString(Time, ":/ -");
	//Puts $1$ in front of Salt
	string Salt = "$1$";
	//Adds the current time as a random salt value.
	Salt += Time;
	//Registers the user into the shadow file.
	Shadow << Username << ":" << crypt(Password.c_str(), Salt.c_str())
			<< ":17131:0:99999:7:::" << endl;
}

#endif /* REGISTER_H_ */
