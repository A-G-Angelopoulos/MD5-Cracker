//============================================================================
// Name        : Permutations.cpp
// Author      : Aristeidis Angelopoulos
// Version     : 1.0
// Copyright   : 
// Description : Calculate all permutations for N places
//============================================================================

#ifndef PERMUTATIONS_H_
#define PERMUTATIONS_H_
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <vector>
using namespace std;

//Characters to use.
static char Letters[] ="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*(){}[]=+-_:;\"\'`<,>.?/|\\~";
extern vector<string> Words;
long long CountPerm=0;
string CurrentWord = "";
string PreviousWord = "";

//Recursive function to get all possible permutations of specific Characters for N spaces.
void Recursion(int Depth) {
    Depth--;
    //Loops for all characters
    for (unsigned int i=0; i<strlen(Letters); i++) {
        CurrentWord = CurrentWord + Letters[i];
        if (Depth > 0) {
            //Calls itself
            Recursion(Depth);
        } else {
            CountPerm++;
            //Put words into Vector
            Words.push_back(CurrentWord);
            if (CountPerm % 100000 == 0){
                cout << CountPerm << " Words Calculated" << endl;
            }
        }
        CurrentWord = PreviousWord;
    }
}

#endif /* PERMUTATIONS_H_ */
