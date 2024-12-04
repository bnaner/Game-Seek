#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include "Node.h"

#ifndef GAME_SEEK_GAMEBASE_H
#define GAME_SEEK_GAMEBASE_H

#endif //GAME_SEEK_GAMEBASE_H

class GameBase{
    unordered_map<string, Node> import_games();
    vector<pair<Node, double>> familiarity_list_helper(string name);
    void quick_recommendation_list(vector<pair<Node, double>>& recommendation_list, int low, int high);
    int quick_helper(vector<pair<Node, double>>& recommendation_list, int low, int high);

public:
    GameBase();
    unordered_map<string, Node> games;
    vector<pair<Node, double>> familiarity_list(string name, double& q, double& s);
    void shell_recommendation_list(vector<pair<Node, double>>& recommendation_list);
};
