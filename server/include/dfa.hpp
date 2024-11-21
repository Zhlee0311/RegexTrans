#ifndef __DFA_HPP
#define __DFA_HPP

#include <set>

class StateDFA;
class NFA;

class DFA
{
private:
    StateDFA *initialState;            // DFA只有一个初态
    std::set<StateDFA *> acceptStates; // DFA的终态集合

public:
    DFA(StateDFA *param1, std::set<StateDFA *> param2); // 构造函数，param1对应初态，param2对应终态集合
    ~DFA();

public:
    static DFA *build(NFA *nfa);

public:
    StateDFA *getInitialState();

    std::set<StateDFA *> getAcceptStates();
};

#endif