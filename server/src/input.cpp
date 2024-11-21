#include "share.hpp"
#include "input.hpp"
#include <stack>

std::string pre_process(std::string s)
{
    get_characters(s);
    auto check = [s](int i) -> bool
    {
        if (s[i] == '|' || s[i + 1] == '|' || s[i] == '(' || s[i + 1] == ')' || s[i + 1] == '*')
        {
            return false;
        }
        else
        {
            return true;
        }
    }; // 判断连接符是否应该插入

    std::string tmp; // 用一个空串进行插入操作
    if (s.size() >= 2)
    {
        for (int i = 0; i <= s.size() - 2; i++)
        {
            tmp += s[i];
            if (check(i))
            {
                tmp += '@'; // 使用‘@’表示连接符
            }
        }
        tmp += s.back();
        return tmp;
    }
    else
    {
        return s; // 若为空串或长度为1的串则直接返回
    }
}

std::string trans_postfix(std::string s)
{
    std::string res;
    std::stack<char> op_stack; // 存储操作符的栈
    for (int i = 0; i < s.size(); i++)
    {
        char cur = s[i]; // 当前处理的字符
        if (cur == ')')
        {
            while (op_stack.top() != '(')
            {
                res += op_stack.top();
                op_stack.pop();
            }
            op_stack.pop();
        }
        else if (cur == '(')
        {
            op_stack.push(cur);
        }
        else if (Share::precedence.find(cur) == Share::precedence.end())
        {
            res += cur;
        }
        else
        {
            while (!op_stack.empty() && Share::precedence.at(op_stack.top()) >= Share::precedence.at(cur))
            {
                res += op_stack.top();
                op_stack.pop();
            }
            op_stack.push(cur);
        }
    }
    while (!op_stack.empty())
    {
        res += op_stack.top();
        op_stack.pop();
    }
    return res;
}

void get_characters(std::string s)
{
    for (const auto &ch : s)
    {
        if (Share::precedence.find(ch) == Share::precedence.end())
        {
            Share::alphabet.insert(ch); // 若优先级表中未找到ch，则代表它是个字母，加入字母表
        }
    }
}