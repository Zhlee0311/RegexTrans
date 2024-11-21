#include "crow.h"
#include "crow/middlewares/cors.h"
#include "nfa.hpp"
#include "dfa.hpp"
#include "mdfa.hpp"
#include "share.hpp"
#include "state_nfa.hpp"
#include "state_dfa.hpp"
#include "state_mdfa.hpp"

std::unordered_map<char, int> Share::precedence = {
    {'*', 3}, // 闭包
    {'@', 2}, // 连接
    {'|', 1}, // 或
    {'(', 0},
    {')', -1}};

std::unordered_set<char> Share::alphabet;

int StateNFA::nextId = 0;
std::map<int, StateNFA *> StateNFA::states;

int StateDFA::nextId = 0;
std::map<int, StateDFA *> StateDFA::states;

int StateMDFA::nextId = 0;
std::map<int, StateMDFA *> StateMDFA::states;

int main()
{
    crow::App<crow::CORSHandler> app;
    auto &cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .origin("*")
        .headers("origin, x-requested-with, accept, access-control-allow-origin, authorization, content-type")
        .methods("POST"_method, "GET"_method, "PUT"_method, "DELETE"_method, "PATCH"_method, "OPTIONS"_method);

    /**
     * # @brief 通过输入的正则表达式构造NFA
     */
    CROW_ROUTE(app, "/nfa").methods(crow::HTTPMethod::POST)([&](const crow::request &req)
                                                            {
        crow::json::rvalue reqData = crow::json::load(req.body);

        if(!reqData){
            return crow::response(400);
        }

        std::string input = reqData["input"].s();
        NFA *nfa = NFA::build(input);
        nlohmann::json result = StateNFA::toJSON(nfa);
        
        delete nfa;
        StateNFA::clearStates();
        
        auto response = crow::response(result.dump());
        response.add_header("Content-Type", "application/json");
        return response; });

    /**
     * # @brief 通过输入的正则表达式构造DFA
     */
    CROW_ROUTE(app, "/dfa").methods(crow::HTTPMethod::POST)([&](const crow::request &req)
                                                            {
            crow::json::rvalue reqData = crow::json::load(req.body);
            if(!reqData){
                return crow::response(400);
            }

            std::string input = reqData["input"].s();
            NFA *nfa = NFA::build(input);
            DFA *dfa = DFA::build(nfa);
            nlohmann::json result = StateDFA::toJSON(dfa);

            delete nfa;
            delete dfa;
            StateNFA::clearStates();
            StateDFA::clearStates();

            auto response = crow::response(result.dump());
            response.add_header("Content-Type", "application/json");
        return response; });

    /**
     * @brief 通过输入的正则表达式构造MDFA
     */
    CROW_ROUTE(app, "/mdfa").methods(crow::HTTPMethod::POST)([&](const crow::request &req)
                                                             {
            crow::json::rvalue reqData = crow::json::load(req.body);
            if(!reqData){
                return crow::response(400);
            }
            
            std::string input = reqData["input"].s();
            NFA *nfa = NFA::build(input);
            DFA *dfa = DFA::build(nfa);
            MDFA *mdfa = MDFA::build(dfa);
            nlohmann::json result = StateMDFA::toJSON(mdfa);
            
            delete nfa;
            delete dfa;
            delete mdfa;

            StateMDFA::clearStates();
            StateDFA::clearStates();
            StateNFA::clearStates();
            
            auto response = crow::response(result.dump());
            response.add_header("Content-Type", "application/json");
        return response; });

    app.port(8080).multithreaded().run();
}