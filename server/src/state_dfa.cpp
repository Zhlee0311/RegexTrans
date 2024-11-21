#include "state_dfa.hpp"
#include "state_nfa.hpp"
#include "dfa.hpp"
#include <iostream>

std::map<int, StateDFA *> StateDFA::getStates()
{
    return states;
}

void StateDFA::clearStates()
{
    for (auto state : states)
    {
        delete state.second;
    }
    states.clear();
    nextId = 0;
}

void StateDFA::showStates(DFA *dfa)
{
    std::cout << "\033[32m" << "以下为DFA:" << "\033[0m" << std::endl;
    std::cout << "\033[33m" << "初态: " << "\033[0m" << std::endl;
    std::cout << "\033[35m" << "State" << dfa->getInitialState()->getId() << "\033[0m" << std::endl;
    std::cout << "\033[31m" << "终态:" << "\033[0m" << std::endl;
    for (const auto &acc : dfa->getAcceptStates())
    {
        std::cout << "\033[35m" << "State" << acc->getId() << " " << "\033[0m";
    }
    std::cout << "\n\033[36m" << "DFA的转换关系:" << "\033[0m" << std::endl;
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

nlohmann::json StateDFA::toJSON(DFA *dfa)
{
    nlohmann::json res;
    nlohmann::json inits;
    nlohmann::json accs;
    nlohmann::json states_json;

    inits.push_back(dfa->getInitialState()->getId());
    res["inits"] = inits;

    for (const auto &acc : dfa->getAcceptStates())
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

std::set<StateNFA *> StateDFA::move(StateDFA *dfa_state, char a)
{
    std::set<StateNFA *> char_closure = dfa_state->charClosure(a); // 对这个DFA状态进行符号闭包运算
    std::set<StateNFA *> res;
    for (const auto &nfa_state : char_closure)
    {
        auto epsilon_closure = nfa_state->epsilonClosure();
        res.insert(epsilon_closure.begin(), epsilon_closure.end());
    }
    return res;
}

StateDFA *StateDFA::search(std::set<StateNFA *> ent)
{
    for (const auto &pair : states)
    {
        StateDFA *state = pair.second;
        if (state->entity == ent)
        {
            return state; // 存在
        }
    }
    return nullptr; // 不存在
}

StateDFA::StateDFA(std::set<StateNFA *> ent)
{
    entity = ent;
    Id = nextId;
    nextId++;
    states.emplace(Id, this);
}

StateDFA::~StateDFA()
{
    edges.clear();
    entity.clear(); // 里面指针指向的内存并不需要释放，由StateNFA统一管理
}

void StateDFA::link(char condition, StateDFA *target)
{
    edges.push_back(std::pair<char, int>(condition, target->getId()));
}

/*
std::set<StateNFA *> StateDFA::epsilonClosure()
{
    std::set<StateNFA *> res;
    for (const auto &state : entity)
    {
        auto closure = state->epsilonClosure();
        res.insert(closure.begin(), closure.end());
    }
    return res;
} // dfa状态的epsilon_closure
*/

std::set<StateNFA *> StateDFA::charClosure(char a)
{
    std::set<StateNFA *> res;
    for (const auto &nfa_state : entity)
    {
        auto char_closure = nfa_state->charClosure(a); // 对单个NFA状态进行符号闭包运算
        res.insert(char_closure.begin(), char_closure.end());
    }
    return res;
} // dfa状态的char_closure

int StateDFA::getId()
{
    return Id;
}

std::vector<std::pair<char, int>> StateDFA::getEdges()
{
    return edges;
}

std::set<StateNFA *> StateDFA::getEntity()
{
    return std::set<StateNFA *>(entity);
}