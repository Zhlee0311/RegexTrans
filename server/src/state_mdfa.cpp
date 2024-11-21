#include "state_mdfa.hpp"
#include "mdfa.hpp"
#include <iostream>

std::map<int, StateMDFA *> StateMDFA::getStates()
{
    return states;
}

void StateMDFA::clearStates()
{
    for (auto state : states)
    {
        delete state.second;
    }
    states.clear();
    nextId = 0;
}

void StateMDFA::showStates(MDFA *mdfa)
{
    std::cout << "\033[32m" << "以下为最小DFA:" << "\033[0m" << std::endl;
    std::cout << "\033[33m" << "初态: " << "\033[0m" << std::endl;
    std::cout << "\033[35m" << "State" << mdfa->getInitialState()->getId() << "\033[0m" << std::endl;
    std::cout << "\033[31m" << "终态:" << "\033[0m" << std::endl;
    for (const auto &acc : mdfa->getAcceptStates())
    {
        std::cout << "\033[35m" << "State" << acc->getId() << " " << "\033[0m";
    }
    std::cout << "\n\033[36m" << "最小DFA的转换关系:" << "\033[0m" << std::endl;
    for (const auto &state : states)
    {
        const auto &edges = state.second->getEdges();
        std::cout << "\033[35m" << "State" << state.first << ":" << "\033[0m" << std::endl;
        for (const auto &edge : edges)
        {
            if (edge.first == '#')
            {
                std::cout << " + " << "\033[34m" << "ε" << "\033[0m" << " ---> " << "\033[35m" << "State" << edge.second << "\033[0m" << std::endl;
            }
            else
            {
                std::cout << " + " << "\033[34m" << edge.first << "\033[0m" << " ---> " << "\033[35m" << "State" << edge.second << "\033[0m" << std::endl;
            }
        }
    }
    std::cout << "============================" << std::endl;
}

nlohmann::json StateMDFA::toJSON(MDFA *mdfa)
{
    nlohmann::json res;
    nlohmann::json inits;
    nlohmann::json accs;
    nlohmann::json states_json;

    inits.push_back(mdfa->getInitialState()->getId());
    res["inits"] = inits;

    for (const auto &acc : mdfa->getAcceptStates())
    {
        accs.push_back(acc->getId());
    }
    res["accs"] = accs;

    for (const auto &[id, state] : states)
    {
        nlohmann::json state_json;
        nlohmann::json edges;
        state_json["id"] = id;
        for (const auto &edge : state->getEdges())
        {
            if (edge.first == '#')
            {
                edges.push_back({{"character", "ε"}, {"target", edge.second}});
            }
            else
            {
                edges.push_back({{"character", std::string(1, edge.first)}, {"target", edge.second}});
            }
        }
        state_json["edges"] = edges;
        states_json.push_back(state_json);
    }
    res["states"] = states_json;
    return res;
}

StateMDFA::StateMDFA(std::set<StateDFA *> ent)
{
    entity = ent;
    Id = nextId;
    nextId++;
    states.emplace(Id, this);
}

StateMDFA::~StateMDFA()
{
    edges.clear();
    entity.clear();
}

void StateMDFA::link(char condition, StateMDFA *target)
{
    edges.push_back(std::pair<char, int>(condition, target->getId()));
}

int StateMDFA::getId()
{
    return Id;
}

std::vector<std::pair<char, int>> StateMDFA::getEdges()
{
    return edges;
}

std::set<StateDFA *> StateMDFA::getEntity()
{
    return entity;
}
