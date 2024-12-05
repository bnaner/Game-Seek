#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "Node.h"

#ifndef GAME_SEEK_GAMEBASE_H
#define GAME_SEEK_GAMEBASE_H

#endif //GAME_SEEK_GAMEBASE_H

class GameBase{
    unordered_map<string, Node> import_games();

public:
    GameBase();
    unordered_map<string, Node> games;
};
