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
    if (data.games.find(selected) == data.games.end()){
        cout << "Sorry I don't know that one." << endl;
        return 0;
    }
    else{
        cout << "Oh yea, I know that game!" << endl;
        found = true;
    }

    vector<pair<Node, double>> recommendation_list;
    double q_time = 0, s_time = 0; // Will store time to completed sorts
    if (found){
        recommendation_list = data.familiarity_list(selected, q_time, s_time);
        cout << "I got some recommendations for you" << endl << endl;
    }

    bool accepted = false;
    int pass = 1;
    while(!accepted && pass < recommendation_list.size()){
        cout << "I recommend you try \"" << recommendation_list[pass].first.name << "\"" << endl;
        cout << "Type y if you like the recommendation or any other character if you want a new one." << endl;
        string response;
        cin >> response;
        if(response != "y")
            pass++;
        else
            break;
    }
    cout << endl;
    cout << "It took quick sort " << q_time << " ms and shell sort " << s_time << " ms to find your recommendations" << endl;
    cout << "Thank you for using out app!" << endl;
}