#ifndef __SHARE_HPP
#define __SHARE_HPP

#include <unordered_set>
#include <unordered_map>

/**
 * @brief 存储共享变量的类
 */
class Share
{
public:
    static std::unordered_map<char, int> precedence;
    static std::unordered_set<char> alphabet;
};


#endif