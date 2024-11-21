#include "state_nfa.hpp"
#include "nfa.hpp"
#include <queue>
#include <iostream>

std::map<int, StateNFA *> StateNFA::getStates()
{
    return states;
}

void StateNFA::clearStates()
{
    for (auto state : states)
    {
        delete (state.second);
    }
    states.clear();
    nextId = 0;
}

void StateNFA::showStates(NFA *nfa)
{
    auto inits = nfa->getInitialStates();
    auto accs = nfa->getAcceptStates();

    std::cout << "============================" << std::endl;
    std::cout << "\033[32m" << "以下为NFA:" << "\033[0m" << std::endl;
    std::cout << "\033[33m" << "初态: " << "\033[0m" << std::endl;
    for (const auto &init : inits)
    {
        std::cout << "\033[35m" << "State" << init->getId() << " " << "\033[0m";
    }
    std::cout << std::endl;
    std::cout << "\033[31m" << "终态:" << "\033[0m" << std::endl;
    for (const auto &acc : accs)
    {
        std::cout << "\033[35m" << "State" << acc->getId() << " " << "\033[0m";
    }
    std::cout << "\n\033[36m" << "NFA的转换关系:" << "\033[0m" << std::endl;
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

nlohmann::json StateNFA::toJSON(NFA *nfa)
{
    nlohmann::json res;
    nlohmann::json inits;
    nlohmann::json accs;
    nlohmann::json states_json;
    for (const auto &init : nfa->getInitialStates())
    {
        inits.push_back(init->getId());
    }
    res["inits"] = inits;

    for (const auto &acc : nfa->getAcceptStates())
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

StateNFA::StateNFA()
{
    Id = nextId;
    nextId++;
    states.emplace(Id, this);
}

StateNFA::~StateNFA()
{
    edges.clear();
}

void StateNFA::link(char condition, StateNFA *target)
{
    edges.push_back(std::pair<char, int>(condition, target->getId()));
}

std::set<StateNFA *> StateNFA::epsilonClosure()
{
    std::set<StateNFA *> res;       // 最终返回的结果
    std::queue<StateNFA *> toCheck; // 待检查是否有epsilon边的状态队列， a + epsilon -> b ，则b属于a的epsilon_closure

    res.insert(this);
    toCheck.push(this);

    while (!toCheck.empty())
    {
        StateNFA *cur = toCheck.front();
        toCheck.pop();
        for (const auto &edge : cur->edges)
        {
            if (edge.first == '#')
            {
                StateNFA *nex = states[edge.second];
                if (res.find(nex) == res.end())
                {
                    res.insert(nex);
                    toCheck.push(nex);
                }
            }
        }
    }
    return res;
}

std::set<StateNFA *> StateNFA::charClosure(char a)
{
    std::set<StateNFA *> res;
    std::queue<StateNFA *> toCheck;

    toCheck.push(this);
    while (!toCheck.empty())
    {
        StateNFA *cur = toCheck.front();
        toCheck.pop();
        for (const auto &edge : cur->edges)
        {
            if (edge.first == a)
            {
                StateNFA *nex = states[edge.second];
                if (res.find(nex) == res.end())
                {
                    res.insert(nex);
                    toCheck.push(nex);
                }
            }
        }
    }
    return res;
}

int StateNFA::getId()
{
    return Id;
}

std::vector<std::pair<char, int>> StateNFA::getEdges()
{
    return edges;
}