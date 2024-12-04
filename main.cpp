#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "GameBase.h"


using namespace std;

int main(){
    GameBase data;
    auto iter = data.games.begin();
    cout << iter->second.name << endl;
    cout << iter->second.price << endl;
    cout << data.games.size();
}