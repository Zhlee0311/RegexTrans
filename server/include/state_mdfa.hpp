#ifndef __STATE_MDFA_HPP
#define __STATE_MDFA_HPP

#include <vector>
#include <map>
#include <set>
#include <nlohmann/json.hpp>

class StateDFA;
class MDFA;

/**
 * @brief 最小DFA的状态,即DFA的状态集合
 */
class StateMDFA
{
private:
    int Id;
    std::vector<std::pair<char, int>> edges;
    std::set<StateDFA *> entity; // 实体是DFA状态的集合

private:
    static int nextId;
    static std::map<int, StateMDFA *> states; // 统一管理所有StateMDFA的映射

public:
    static std::map<int, StateMDFA *> getStates();
    static void clearStates();
    static void showStates(MDFA *mdfa);
    static nlohmann::json toJSON(MDFA *mdfa);

public:
    StateMDFA(std::set<StateDFA *> ent);

    ~StateMDFA();

    void link(char condition, StateMDFA *target);

    int getId();

    std::vector<std::pair<char, int>> getEdges();

    std::set<StateDFA *> getEntity();
};

#endif