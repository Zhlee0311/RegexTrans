#include "dfa.hpp"
#include "mdfa.hpp"
#include "share.hpp"
#include "state_dfa.hpp"
#include "state_mdfa.hpp"
#include <deque>
#include <algorithm>
#include <iostream>

MDFA::MDFA(StateMDFA *param1, std::set<StateMDFA *> param2)
{
    initialState = param1;
    acceptStates = param2;
}

MDFA::~MDFA()
{
    acceptStates.clear();
}

MDFA *MDFA::build(DFA *dfa)
{
    StateDFA *dfa_init = dfa->getInitialState();            // 此dfa的初态
    std::set<StateDFA *> dfa_accs = dfa->getAcceptStates(); // 此dfa的终态

    std::set<StateDFA *> setAcc;  // 初始的接受状态的集合
    std::set<StateDFA *> setNAcc; // 初始的非接受状态的集合

    std::map<int, StateDFA *> statesDFA = StateDFA::getStates(); // 所有的dfa状态（dfa状态已确定，可以直接使用拷贝）
    // auto statesMDFA = StateMDFA::getStates(); // 所有的mdfa状态

    for (const auto &[id, state] : statesDFA)
    {
        if (dfa_accs.count(state))
        {
            setAcc.insert(state);
        }
        else
        {
            setNAcc.insert(state);
        }
    }

    /*
    //用于验证初始的接受状态和非接受状态
    std::cout << "setAcc: ";
    for (const auto &state : setAcc)
    {
        std::cout << state->getId() << " ";
    }
    std::cout << "\nsetNAcc: ";
    for (const auto &state : setNAcc)
    {
        std::cout << state->getId() << " ";
    }
    */

    std::deque<std::set<StateDFA *>> allSet;
    if (!setNAcc.empty())
    {
        allSet.push_back(setNAcc);
    }
    if (!setAcc.empty())
    {
        allSet.push_back(setAcc);
    }

    /**
     * @brief 寻找一个dfa状态在某个符号条件下的转移状态Id
     * @param dfa_state 当前的dfa状态
     * @param ch 符号条件
     * @return int 转移状态的Id
     * @attention 当Id为-1时代表不存在此转移状态，逻辑上即这个状态在ch条件下指向自己
     */
    auto search = [](StateDFA *dfa_state, char ch) -> int
    {
        auto edges = dfa_state->getEdges();
        for (const auto &edge : edges)
        {
            if (edge.first == ch)
            {
                return edge.second; // 找到了目标状态的Id
            }
        }
        return -1; // 没找到则返回-1
    }; // 寻找一个dfa状态在某个符号条件下的转移状态Id，若无对应的转移状态则返回-1

    /**
     * @brief 判断两个 dfa_state 在”逻辑“上是否属于同一个集合
     * @param target  当前状态的转移结果
     * @param real 当前集合中第一个状态的转移结果
     * @param curSet 当前处理的集合
     * @attention 当Id为-1时代表不存在此转移状态，逻辑上即这个状态在ch条件下指向自己
     */
    auto check = [&allSet, &statesDFA](int target, int real, std::set<StateDFA *> curSet) -> bool
    {
        // 需要考虑target和real不全大于0的情况
        if (target == -1)
        {
            if ((real != -1 && curSet.count(statesDFA[real])) || real == -1)
            {
                return true;
            }
            return false;
        }
        else if (real == -1)
        {
            auto st_0 = statesDFA[target];
            if (curSet.count(st_0))
            {
                return true;
            }
            return false;
        }

        auto st_1 = statesDFA[target];
        auto st_2 = statesDFA[real];
        for (const auto &curSet : allSet)
        {
            if (curSet.count(st_1) && curSet.count(st_2))
            {
                return true;
            }
        } // 首先遍历当前未处理完的dfa状态的集合，检查其中是否存在两个Id代表的dfa状态
        for (const auto &state : StateMDFA::getStates())
        {
            auto ent = state.second->getEntity();
            if (ent.count(st_1) && ent.count(st_2))
            {
                return true;
            }
        } // 其次遍历所有mdfa的状态，其中的实体ent即为一个dfa状态的集合，检查过程与上类似
        return false;
    }; // 判断两个 dfa_state 是否属于同一个集合（若Id为-1则代表不存在此状态）

    std::unordered_map<char, int> firstTarget;   // 当前处理的集合中，第一个状态面对不同符号的转换情况
    std::set<StateDFA *> trashSet;               // 当前处理的集合的某一个状态位于首部时，移除状态的方案
    std::vector<std::set<StateDFA *>> trashSets; // 当前处理的集合中，所有移除状态的方案

    while (!allSet.empty())
    {
        trashSets.clear();
        std::set<StateDFA *> &curSet = allSet.front(); // 当前处理的集合

        // 考虑不同集合位于首部的情况
        for (auto &first : curSet)
        {
            firstTarget.clear();
            trashSet.clear();

            for (const auto &ch : Share::alphabet)
            {
                int target = search(first, ch); // 当前处理的集合的第一个状态在面对 "ch" 时转移到的状态的Id
                firstTarget[ch] = target;       // 不管结果如何都插入，-1也插入，代表没有对应的转换状态
            } // 构建：当前处理的集合中，第一个状态面对不同符号的转换情况

            for (auto it = curSet.begin(); it != curSet.end();)
            {
                bool moved = false;
                for (const auto &ch : Share::alphabet)
                {
                    auto target = search(*it, ch);
                    auto real = firstTarget[ch];

                    /*
                    //用于验证不同Id的DFA状态的转移情况
                    if ((*it)->getId() == 2)
                    {
                        std::cout << "target: " << target << " real: " << real << std::endl;
                    }
                    */

                    // 若当前的状态存在以此ch为条件的转移，且与第一个状态的转移结果在逻辑上不属于同一个集合，则将其移出当前集合
                    if (!check(target, real, curSet))
                    {
                        trashSet.insert(*it);
                        moved = true; // 逻辑删除
                        it++;
                        break;
                    }
                }
                if (!moved)
                {
                    it++;
                }
            }
            trashSets.push_back(trashSet);
        }

        // 从当前集合中移除状态，选择移除集合数量最少的方案
        int index = 0;
        for (int i = 0; i < trashSets.size(); i++)
        {
            if (trashSets[i].empty())
            {
                index = i;
                break;
            }
            else if (trashSets[i].size() < trashSets[index].size())
            {
                index = i;
            }
        }

        for (auto &state : trashSets[index])
        {
            curSet.erase(state);
        }

        new StateMDFA(curSet);
        allSet.pop_front();

        if (!trashSets[index].empty())
        {
            allSet.push_back(trashSets[index]);
        }
    }

    StateMDFA *param1 = nullptr;
    std::set<StateMDFA *> param2;

    /*
    用于验证生成的MDFA状态是否正确
    for (const auto &[id, state] : StateMDFA::getStates())
    {
        std::cout << id << ": ";
        auto ent = state->getEntity();
        for (const auto &dfaState : ent)
        {
            std::cout << dfaState->getId() << " ";
        }
        std::cout << std::endl;
    }
    */

    for (auto &mdfa_state_1 : StateMDFA::getStates())
    {
        auto curState = mdfa_state_1.second; // 当前的MDFA状态
        auto curEnt = curState->getEntity(); // 当前MDFA状态的实体（一个DFA状态的集合）

        std::unordered_map<char, std::unordered_set<int>> hasLinked;

        // 需要遍历当前MDFA状态实体中的每一个DFA状态，根据他们的边建立连接关系
        for (const auto &dfaState : curEnt)
        {
            for (const auto &edge : dfaState->getEdges())
            {
                for (auto &mdfa_state_2 : StateMDFA::getStates())
                {
                    auto ent = mdfa_state_2.second->getEntity();
                    if (ent.count(statesDFA[edge.second]) && !hasLinked[edge.first].count(mdfa_state_2.first))
                    {
                        curState->link(edge.first, mdfa_state_2.second);
                        hasLinked[edge.first].insert(mdfa_state_2.first);
                        break;
                    }
                }
            }
        }

        if (curEnt.count(dfa_init))
        {
            param1 = curState;
        }
        if (std::any_of(curEnt.begin(), curEnt.end(), [&](StateDFA *indiv)
                        { return dfa_accs.count(indiv); }))
        {
            param2.insert(curState);
        }
    }

    auto result = new MDFA(param1, param2);

    return result;
}

StateMDFA *MDFA::getInitialState()
{
    return initialState;
}

std::set<StateMDFA *> MDFA::getAcceptStates()
{
    return acceptStates;
}