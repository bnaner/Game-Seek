//
// Created by layth on 12/3/2024.
//

#include "GameBase.h"

GameBase::GameBase() {
    games = import_games();
}

// Goes through the games.csv file and maps each entry to a node, then adding them into the GameBase
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
        if(!new_game.name.empty() && new_game.age < 18 && new_game.languages.find("English") != new_game.languages.end() && new_game.rating > 0.5 && new_game.positive_rating > 9)
            games.insert(make_pair(new_game.name, new_game));
    }
    input.close();
    return games;
}

vector<pair<Node, double>> GameBase::familiarity_list(string name, double& q, double& s) {
    vector<pair<Node, double>> recommendation_list_Q = familiarity_list_helper(name);
    vector<pair<Node, double>> recommendation_list_S = recommendation_list_Q;

    auto q_start = chrono::high_resolution_clock::now();
    quick_recommendation_list(recommendation_list_Q, 0, recommendation_list_Q.size());
    auto q_stop = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> q_time = q_stop - q_start;
    q += q_time.count();

    auto s_start = chrono::high_resolution_clock::now();
    shell_recommendation_list(recommendation_list_S);
    auto s_stop = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> s_time = s_stop - s_start;
    s+= s_time.count();

    return recommendation_list_Q;
}

// Returns a list of games and a familiarity score, with higher scores being more similar to the inputted game
vector<pair<Node, double>> GameBase::familiarity_list_helper(string name){
    // Formula TBD, (shared genres*2 / total genres between two games) + .1 per shared publisher +- .1 for rating
    double TAGS_MULT = 5;
    double RATING_MULT = 1;
    //double POPULARITY_MULT = 0.5; // Incorporate if desired
    double PUBLISHER_MULT = 0.5;
    vector<pair<Node, double>> list;

    if(games.find(name) == games.end())
        return list;

    Node selected_game = games[name];
    for (auto iter = games.begin(); iter != games.end(); iter++){
        double score = 0;
        double tag_score = 0;
        double tag_matches = 0;

        for (auto tag = selected_game.tags.begin(); tag != selected_game.tags.end(); tag++){
            if(iter->second.tags.find(*tag) != iter->second.tags.end())
                tag_matches++;
        }
        tag_score = ((tag_matches*2) / (selected_game.tags.size() + iter->second.tags.size())) * TAGS_MULT;
        score += tag_score;

        for (auto publisher = selected_game.publishers.begin(); publisher != selected_game.publishers.end(); publisher++){
            if(iter->second.publishers.find(*publisher) != iter->second.publishers.end())
                score += PUBLISHER_MULT;
        }
        if(iter->second.rating > 0.79)
            score += RATING_MULT;

        list.push_back(make_pair(iter->second, score));
    }
    return list;
}

// Sorting lecture slides used as reference for both algorithms
void GameBase::quick_recommendation_list(vector<pair<Node, double>>& recommendation_list, int low, int high) {
    if (low < high){
        int pivot_index = quick_helper(recommendation_list, low, high); // Partially sorts the array based on pivot
        quick_recommendation_list(recommendation_list, low, pivot_index - 1); // Sorts subarray less than pivot
        quick_recommendation_list(recommendation_list, pivot_index+1, high); // Sorts subarray more than pivot
    }
}

int GameBase::quick_helper(vector<pair<Node, double>>& recommendation_list, int low, int high) {
    int pivot_index = low;
    int upward = low, downward = high;

    while (upward < downward){
        for(int i = upward; i < high; i++){
            if (recommendation_list[i].second < recommendation_list[pivot_index].second)
                break;
            upward++;
        }
        for(int i = downward; i > low; i--){
            if (recommendation_list[i].second > recommendation_list[pivot_index].second)
                break;
            downward--;
        }
        if(upward < downward){
            swap(recommendation_list[upward], recommendation_list[downward]);
        }
    }
    swap(recommendation_list[low], recommendation_list[downward]);
    return downward;
}


void GameBase::shell_recommendation_list(vector<pair<Node, double>>& recommendation_list) {
    for(int gap = recommendation_list.size() / 2; gap > 0; gap /= 2){
        for(int i = 0; i + gap < recommendation_list.size(); i++){
            if(recommendation_list[i].second < recommendation_list[i+gap].second){
                swap(recommendation_list[i], recommendation_list[i+gap]);
                int current = i;
                while (current - gap >= 0 && recommendation_list[current].second > recommendation_list[current-gap].second){
                    swap(recommendation_list[current], recommendation_list[current-gap]);
                    current -= gap;
                }
            }
        }
    }
}






