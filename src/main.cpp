#include "crow.h"
#include "GameBase.h"

int main() {
    crow::SimpleApp app;
    GameBase data;

    // Serve index.html at the root URL
    CROW_ROUTE(app, "/")([]() {
        auto page = crow::mustache::load_text("index.html");
        return page;
    });

    // Search endpoint remains the same
    CROW_ROUTE(app, "/search")
        .methods("GET"_method)
        ([&data](const crow::request& req) {
            auto query = req.url_params.get("query");
            if (!query) {
                return crow::response(400, "Query parameter missing");
            }

            // Search for matching games
            std::vector<Node> matches;
            for (const auto& [name, node] : data.games) {
                if (name.find(query) != std::string::npos) {
                    matches.push_back(node);
                }
            }

            // Prepare the JSON response
            crow::json::wvalue response;
            response["results"] = crow::json::wvalue::list(); // Initialize as a list

            auto& results = response["results"];
            size_t index = 0;

            for (const auto& game : matches) {
                crow::json::wvalue game_data;
                game_data["name"] = game.name;
                game_data["price"] = game.price;
                game_data["rating"] = game.rating;
                game_data["description"] = game.description;
                game_data["image"] = game.image;

                results[index++] = std::move(game_data); // Add game data to the results list
            }

            return crow::response(response);
        });

    // Start the Crow app
    app.port(8080).multithreaded().run();
    return 0;
}
