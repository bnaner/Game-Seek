#include <iostream>
#include <unordered_set>

using namespace std;

#ifndef GAME_SEEK_NODE_H
#define GAME_SEEK_NODE_H

#endif //GAME_SEEK_NODE_H

class Node {
public:
    Node(){};
    string name;
    int age = 0; // Minimum age rating to play the game, not all games have this property properly filled however
    double recommendation_score = 0.0; // Formula TBD, (shared genres*2 / total genres between two games) + .1 per shared publisher +- .1 for rating
    double price = 0.0;
    double positive_rating = 0;
    double negative_rating = 0;
    double rating = 0; // Will be postive/total reviews
    string description;
    string image;
    string url = "https://store.steampowered.com/app/"; // appID will be appended
    unordered_set<string> tags;
    unordered_set<string> publishers;
    unordered_set<string> languages;
};