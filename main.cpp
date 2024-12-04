#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "GameBase.h"


using namespace std;

int main(){
    GameBase data;
    /*auto iter = data.games.begin();
    cout << iter->second.name << endl;
    cout << iter->second.price << endl;
    cout << data.games.size() << endl;*/

    string selected;
    cout << "Enter the name of a game you like!" << endl;
    getline(cin, selected);

    bool found = false;
    if (data.games.find(selected) == data.games.end())
        cout << "Sorry I don't know that one." << endl;
    else{
        cout << "Oh yea, I know that game!" << endl;
        found = true;
    }

    if (found){
        vector<pair<Node, double>> recommendation_list = data.familiarity_list(selected);
        cout << "I got some recommendations for you" << endl;
    }
}