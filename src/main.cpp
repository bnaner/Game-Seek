#include "crow.h"
#include "GameBase.h"
#include <chrono>

int main() {
    crow::SimpleApp app;
    GameBase data;

    // Serve index.html at the root URL
    CROW_ROUTE(app, "/")([](){
        auto page = crow::mustache::load_text("index.html");
        return page;
    });

    // Search endpoint
    CROW_ROUTE(app, "/search")
        .methods("GET"_method)
        ([&data](const crow::request& req) {
            auto query = req.url_params.get("query");
            if (!query) {
                return crow::response(400, "Query parameter missing");
            }

            std::string search_query(query);
            std::string lower_query = search_query;
            std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

            // Search for matching games (case-insensitive)
            std::vector<Node> matches;
            for (const auto& pair : data.games) {
                const std::string& name = pair.first;
                std::string lower_name = name;
                std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
                if (lower_name == lower_query) {
                    matches.push_back(pair.second);
                    break; // Exact match found
                }
            }

            // Prepare the JSON response
            crow::json::wvalue response;
            response["results"] = crow::json::wvalue::list();

            auto& results = response["results"];
            size_t index = 0;

            for (const auto& game : matches) {
                crow::json::wvalue game_data;
                game_data["name"] = game.name;
                game_data["price"] = game.price;
                game_data["rating"] = game.rating;
                game_data["description"] = game.description;
                game_data["image"] = game.image;

                results[index++] = std::move(game_data);
            }

            return crow::response(response);
        });

    // Recommendation endpoint
    CROW_ROUTE(app, "/recommend")
        .methods("GET"_method)
        ([&data](const crow::request& req) {
            auto game_name = req.url_params.get("game");
            std::cout << "Received /recommend request for game: " << (game_name ? game_name : "null") << std::endl;
            if (!game_name) {
                return crow::response(400, "Game parameter missing");
            }

            std::string selected(game_name);

            // Case-insensitive search for the game
            std::string lower_selected = selected;
            std::transform(lower_selected.begin(), lower_selected.end(), lower_selected.begin(), ::tolower);

            auto it = data.games.find(selected);
            if (it == data.games.end()) {
                // Try case-insensitive search
                for (const auto& pair : data.games) {
                    std::string lower_name = pair.first;
                    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
                    if (lower_name == lower_selected) {
                        it = data.games.find(pair.first);
                        selected = pair.first; // Update selected to the correct case
                        break;
                    }
                }
                if (it == data.games.end()) {
                    std::cout << "Game not found: " << selected << std::endl;
                    return crow::response(404, "Game not found");
                }
            }

            double q_time = 0, s_time = 0; // Will store time to completed sorts
            std::vector<std::pair<Node, double>> recommendation_list = data.familiarity_list(selected, q_time, s_time);

            // Remove the selected game from the recommendations
            recommendation_list.erase(
                std::remove_if(
                    recommendation_list.begin(),
                    recommendation_list.end(),
                    [&selected](const std::pair<Node, double>& rec) {
                        return rec.first.name == selected;
                    }
                ),
                recommendation_list.end()
            );

            // Check if the recommendation list is empty
            if (recommendation_list.empty()) {
                crow::json::wvalue response;
                response["recommendations"] = crow::json::wvalue::list();
                return crow::response(response);
            }

            // Prepare the JSON response
            crow::json::wvalue response;
            response["recommendations"] = crow::json::wvalue::list();
            response["quick_sort_time_ms"] = q_time;
            response["shell_sort_time_ms"] = s_time;

            auto& recommendations = response["recommendations"];
            size_t index = 0;

            for (size_t i = 0; i < recommendation_list.size() && index < 10; ++i) {
                const auto& game = recommendation_list[i].first;
                crow::json::wvalue game_data;
                game_data["name"] = game.name;
                game_data["price"] = game.price;
                game_data["rating"] = game.rating;
                game_data["description"] = game.description;
                game_data["image"] = game.image;
                game_data["score"] = recommendation_list[i].second; // Include the familiarity score

                recommendations[index++] = std::move(game_data);
            }

            return crow::response(response);
    });

    // Autocomplete endpoint
    CROW_ROUTE(app, "/autocomplete")
        .methods("GET"_method)
        ([&data](const crow::request& req) {
            auto query = req.url_params.get("query");
            if (!query || std::strlen(query) == 0) {
                return crow::response(400, "Query parameter missing or empty");
            }

            std::string query_str(query);
            std::transform(query_str.begin(), query_str.end(), query_str.begin(), ::tolower);

            std::vector<std::string> suggestions;
            size_t max_suggestions = 10;

            for (const auto& pair : data.games) {
                const std::string& name = pair.first;
                std::string lower_name = name;
                std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
                if (lower_name.find(query_str) == 0) { // Name starts with query
                    suggestions.push_back(name);
                    if (suggestions.size() >= max_suggestions) {
                        break;
                    }
                }
            }

            // Prepare the JSON response
            crow::json::wvalue response;
            response["suggestions"] = crow::json::wvalue::list();

            auto& res_suggestions = response["suggestions"];
            size_t index = 0;

            for (const auto& suggestion : suggestions) {
                res_suggestions[index++] = suggestion;
            }

            return crow::response(response);
    });



    // Start the Crow app
    app.port(18080).multithreaded().run();
    return 0;
}

