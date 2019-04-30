#ifndef LOGIN_H_
#define LOGIN_H_
#include <fstream>
using namespace std;

//Login Function
void Login() {
    //Opens the Shadow file.
    fstream Shadow;
    bool Found = false;
    Shadow.open("shadow.txt");
    string Username, SUsername, Password, Salt, Hash, line, data;
    //Gets User-name and password.
    cout << "Enter your User-name: ";
    cin >> Username;
    cout << endl;
    cout << "Enter your Password: ";
    cin >> Password;
    while (getline(Shadow, line)) {
        stringstream linestream(line);
        getline(linestream, SUsername, ':');
        if (SUsername == Username) {
            Found = true;
            //Gets salt value.
            getline(linestream, Hash, ':');
            stringstream Hashstream(Hash);
            getline(Hashstream, data, '$');
            getline(Hashstream, data, '$');
            getline(Hashstream, Salt, '$');
            Salt = "$1$" + Salt;
            //Gets Hash value.
            if (crypt(Password.c_str(), Salt.c_str()) == Hash) {
                //If it is correct, enters the system.
                //UtopianSystem();
                break;
            } else {
                cout << "Access Forbidden" << endl;
            }
            break;
        }
    }
    if (Found == false) {
        cout << "Access Forbidden" << endl;
    }
}

#endif /* LOGIN_H_ */
