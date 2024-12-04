//
// Created by layth on 12/3/2024.
//

#include "GameBase.h"

GameBase::GameBase() {
    games = import_games();
}

unordered_map<string, Node> GameBase::import_games() {
    unordered_map<string, Node> games;
    string filename = "../games.csv";
    ifstream input(filename);

    if(!input.is_open())
        cout << "Error, file not opened" << endl;

    string line;
    getline(input, line); // goes past the template row
    while(getline(input, line)){
        Node new_game;
        stringstream s(line);
        string temp;

        // Retrieves the game's ID
        getline(s,temp, ',');
        new_game.url += temp;

        // Retrieves the game's name
        getline(s, temp, '"');
        getline(s, temp, '"');
        new_game.name = temp;

        // Skips undesired properties;
        for(int i = 0; i < 4; i++){getline(s, temp, '"');}
        for(int i = 0; i < 2; i++){getline(s, temp, ',');}

        // Retrieves required age to play game
        getline(s, temp, ',');
        new_game.age = stoi(temp);

        // Retrieves price of the game
        getline(s, temp, ',');
        new_game.price = stod(temp);

        // Skip unwanted data
        getline(s, temp, ',');
        getline(s, temp, '"');

        // Retrieves Description
        getline(s, temp, '"');
        new_game.description = temp;

        // Skip
        getline(s, temp, '"');
        getline(s, temp, '"');

        // Retrieves Supported Languages
        stringstream languages(temp);
        getline(languages, temp, '\'');

        while(getline(languages, temp, '\'')){
            new_game.languages.insert(temp);
            getline(languages, temp, '\'');
        }

        // Skip
        for(int i = 0; i < 5; i++) { getline(s, temp, '"');}

        // Retrieve Image
        getline(s, temp, '"');
        new_game.image = temp;

        // Skip
        for(int i = 0; i < 8; i++) { getline(s, temp, '"');}
        for(int i = 0; i < 2; i++) { getline(s, temp, ',');}

        // Retrieves User Ratings
        getline(s, temp, ',');
        if(temp.empty() || !isdigit(temp[0])) {
            cout << new_game.name << endl;}
        new_game.positive_rating = stoi(temp);
        getline(s, temp, ',');
        if(temp.empty() || !isdigit(temp[0])) { continue;}
        new_game.negative_rating = stoi(temp);
        new_game.rating = (new_game.positive_rating) / (new_game.positive_rating + new_game.negative_rating + 1); // +1 is for 0 error

        //Skip
        for(int i = 0; i < 3; i++) { getline(s, temp, ',');}
        getline(s, temp, '"');

        // Retrieve warning notes
        string warnings;
        getline(s, warnings, '"');
        if(warnings.find("sexual") != string::npos || warnings.find("Sexual") != string::npos || warnings.find("18") != string::npos){
            continue; // Doesn't add game if it is inappropriate
        }

        // Skip
        for(int i = 0; i < 5; i++) { getline(s, temp, ',');}
        for(int i = 0; i < 3; i++) { getline(s, temp, '"');}

        // Get list of publishers
        getline(s, temp, '"');
        stringstream publishers(temp);
        while (getline(publishers, temp, ',')){
            new_game.publishers.insert(temp);
        }

        // Skips and Gets tags to describe the game
        for(int i = 0; i < 4; i++) { getline(s, temp, '"');}
        stringstream tags(temp);
        while (getline(tags, temp, ',')){
            new_game.tags.insert(temp);
        }
        for(int i = 0; i < 2; i++) { getline(s, temp, '"');}
        stringstream tags2(temp);
        while (getline(tags2, temp, ',')){
            new_game.tags.insert(temp);
        }

        // Games that are too mature or are not in english or rated poorly should not be added
        if(new_game.age < 18 && new_game.languages.find("English") != new_game.languages.end() && new_game.rating > 0.5 && new_game.positive_rating > 9)
            games.insert(make_pair(new_game.name, new_game));
    }
    input.close();
    return games;
}
