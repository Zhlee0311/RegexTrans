#ifndef __MDFA_HPP
#define __MDFA_HPP

#include <set>

class StateMDFA;
class DFA;

class MDFA
{
private:
    StateMDFA *initialState;
    std::set<StateMDFA *> acceptStates;

public:
    MDFA(StateMDFA *param1, std::set<StateMDFA *> param2);
    ~MDFA();

public:
    static MDFA *build(DFA *dfa);

public:
    StateMDFA *getInitialState();

    std::set<StateMDFA *> getAcceptStates();
};

#endif