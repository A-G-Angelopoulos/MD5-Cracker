#ifndef REMOVECHARS_H_
#define REMOVECHARS_H_
#include <string>
using namespace std;

//Function that removes specific characters from a string
void removeCharsFromString(string &str, const char* charsToRemove) {
    for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
        str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
}

#endif /* REMOVECHARS_H_ */
