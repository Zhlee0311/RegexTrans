#ifndef __NFA_HPP
#define __NFA_HPP

#include <iostream>
#include <set>

class StateNFA;

class NFA
{
private:
    std::set<StateNFA *> initialStates;
    std::set<StateNFA *> acceptStates;

public:
    NFA(char character);
    NFA(StateNFA *initial, StateNFA *accept);
    NFA(std::set<StateNFA *> initialStates, std::set<StateNFA *> acceptStates);
    ~NFA();

public:
    NFA *closure();

    NFA *concat(NFA *next);

    NFA *select(NFA *other);

    std::set<StateNFA *> getInitialStates();
    std::set<StateNFA *> getAcceptStates();

public:
    /**
     * @brief 根据输入的字符串构造NFA
     */
    static NFA *build(std::string input);
};

#endif