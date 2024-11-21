#ifndef __STATE_DFA_HPP
#define __STATE_DFA_HPP

#include <vector>
#include <map>
#include <set>
#include <nlohmann/json.hpp>

class StateNFA;
class DFA;

/**
 * @brief DFA的状态，即NFA的状态集合
 */
class StateDFA
{
private:
    int Id;
    std::vector<std::pair<char, int>> edges;
    std::set<StateNFA *> entity;

private:
    static int nextId;
    static std::map<int, StateDFA *> states; // 统一管理所有StateDFA的映射

public:
    static std::map<int, StateDFA *> getStates();
    static void clearStates();
    static void showStates(DFA * dfa);
    static nlohmann::json toJSON(DFA * dfa);

public:
    static std::set<StateNFA *> move(StateDFA *dfa_state, char a); // dfa状态的外拓
    static StateDFA *search(std::set<StateNFA *> ent);             // 搜寻是否已存在代表此实体的dfa状态

public:
    StateDFA(std::set<StateNFA *> ent);

    ~StateDFA();

    void link(char condition, StateDFA *target);

    // std::set<StateNFA *> epsilonClosure(); // dfa状态的epsilon_closure

    std::set<StateNFA *> charClosure(char a); // dfa状态的char_closure

    int getId();

    std::vector<std::pair<char, int>> getEdges();

    std::set<StateNFA *> getEntity();
};

#endif