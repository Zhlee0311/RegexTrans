#include "state_nfa.hpp"
#include "nfa.hpp"
#include "input.hpp"
#include "share.hpp"
#include <stack>

/*
std::string trans_postfix(std::string s);
std::string pre_process(std::string s);
*/

// extern std::unordered_map<char, int> precedence;

NFA::NFA(char character)
{

    StateNFA *initial = new StateNFA();
    StateNFA *accept = new StateNFA();
    initial->link(character, accept);

    initialStates.insert(initial);
    acceptStates.insert(accept);
}

NFA::NFA(StateNFA *initial, StateNFA *accept)
{
    initialStates.insert(initial);
    acceptStates.insert(accept);
}

NFA::NFA(std::set<StateNFA *> initialStates, std::set<StateNFA *> acceptStates)
{
    this->initialStates = initialStates;
    this->acceptStates = acceptStates;
}

NFA::~NFA()
{
    initialStates.clear();
    acceptStates.clear();
}

NFA *NFA::closure()
{
    StateNFA *initial_new = new StateNFA();
    StateNFA *accept_new = new StateNFA();

    initial_new->link('#', accept_new);

    for (StateNFA *initial : initialStates)
    {
        initial_new->link('#', initial);
    }

    for (StateNFA *accept : acceptStates)
    {
        accept->link('#', accept_new);
    }

    for (StateNFA *accept : acceptStates)
    {
        for (StateNFA *initial : initialStates)
        {
            accept->link('#', initial);
        }
    }
    auto res = new NFA(initial_new, accept_new);
    return res;
}

NFA *NFA::concat(NFA *next)
{
    for (StateNFA *rear : acceptStates)
    {
        for (StateNFA *front : next->initialStates)
        {
            rear->link('#', front);
        }
    }
    auto res = new NFA(initialStates, next->acceptStates);
    return res;
}

NFA *NFA::select(NFA *other)
{
    StateNFA *initial_new = new StateNFA();
    StateNFA *accept_new = new StateNFA();

    for (StateNFA *initial : initialStates)
    {
        initial_new->link('#', initial);
    }

    for (StateNFA *initial : other->initialStates)
    {
        initial_new->link('#', initial);
    }
    for (StateNFA *accept : acceptStates)
    {
        accept->link('#', accept_new);
    }
    for (StateNFA *accept : other->acceptStates)
    {
        accept->link('#', accept_new);
    }
    auto res = new NFA(initial_new, accept_new);
    return res;
}

std::set<StateNFA *> NFA::getInitialStates()
{
    return initialStates;
}

std::set<StateNFA *> NFA::getAcceptStates()
{
    return acceptStates;
}

NFA *NFA::build(std::string input)
{
    input = trans_postfix(pre_process(input)); // 对输入进行预处理
    std::stack<NFA *> tmps;                    // 用于存储NFA中间运算结果的栈

    for (char ch : input)
    {
        if (Share::precedence.find(ch) == Share::precedence.end())
        {
            NFA *tmp = new NFA(ch);
            tmps.push(tmp);
        }
        else if (ch == '*')
        {
            NFA *tmp = tmps.top()->closure();
            delete tmps.top();
            tmps.pop();
            tmps.push(tmp);
        }
        else if (ch == '@')
        {
            NFA *tmp1 = tmps.top();
            tmps.pop();
            NFA *tmp2 = tmps.top();
            tmps.pop();
            NFA *tmp = tmp2->concat(tmp1);
            delete tmp1;
            delete tmp2;
            tmps.push(tmp);
        }
        else if (ch == '|')
        {
            NFA *tmp1 = tmps.top();
            tmps.pop();
            NFA *tmp2 = tmps.top();
            tmps.pop();
            NFA *tmp = tmp2->select(tmp1);
            delete tmp1;
            delete tmp2;
            tmps.push(tmp);
        }
    }
    return tmps.top();
}
