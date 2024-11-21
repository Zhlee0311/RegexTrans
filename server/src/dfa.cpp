#include "dfa.hpp"
#include "nfa.hpp"
#include "state_nfa.hpp"
#include "state_dfa.hpp"
#include "share.hpp"

/**
 * @brief DFA构造函数
 */
DFA::DFA(StateDFA *param1, std::set<StateDFA *> param2)
{
    initialState = param1;
    acceptStates = param2;
}

DFA::~DFA()
{
    acceptStates.clear(); // initialState不需要释放，在StateDFA中由clearState统一管理
}

DFA *DFA::build(NFA *nfa)
{

    std::set<StateNFA *> ent;

    for (const auto &nfa_initial : nfa->getInitialStates())
    {
        std::set<StateNFA *> epsilon_closure = nfa_initial->epsilonClosure();
        ent.insert(epsilon_closure.begin(), epsilon_closure.end());
    } // 对输入的nfa的所有初态求epsilon_closure，从而确定dfa的初态

    auto param1 = new StateDFA(ent);

    auto dfa_states = StateDFA::getStates(); // 注意这里是拷贝，不是引用

    for (int i = 0; i < dfa_states.size(); i++)
    {
        auto dfa_state = dfa_states[i];        // 当前处理的dfa状态
        for (const auto &ch : Share::alphabet) // 遍历字母表
        {
            auto trans = StateDFA::move(dfa_state, ch); // dfa状态经过外拓后的集合
            if (trans.empty())
            {
                continue; // 若外拓集合为空，证明此时的dfa状态由此字母没有出路
            }

            auto target = StateDFA::search(trans); // 寻找外拓后的集合是否存在对应的dfa状态 "target"

            if (target == nullptr) // 若不存在则根据外拓集合建立新状态
            {
                auto dfa_state_new = new StateDFA(trans);                  // 建立新状态
                dfa_states.emplace(dfa_state_new->getId(), dfa_state_new); // 在拷贝中插入这个新状态
                dfa_state->link(ch, dfa_state_new);                        // 用字母'ch'连接 "dfa_state" 和 "dfa_state_new"
            }
            else
            {
                dfa_state->link(ch, target); // 若存在则直接连接 "dfa_state" 和 "target"
            }
        }
    }

    std::set<StateDFA *> param2;

    auto nfa_accs = nfa->getAcceptStates(); // 输入 "nfa" 的所有终态

    // 遍历所有dfa_state
    for (const auto &dfa_state : StateDFA::getStates())
    {
        // 检查其中的nfa_state是否包含输入 "nfa" 的终态
        for (const auto &nfa_state : dfa_state.second->getEntity())
        {
            if (nfa_accs.find(nfa_state) != nfa_accs.end())
            {
                param2.insert(dfa_state.second);
            }
        }
    }

    auto res = new DFA(param1, param2);

    return res;
}

StateDFA *DFA::getInitialState()
{
    return initialState;
}

std::set<StateDFA *> DFA::getAcceptStates()
{
    return acceptStates;
}