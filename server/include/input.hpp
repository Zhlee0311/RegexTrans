#ifndef __INPUT_HPP
#define __INPUT_HPP

#include <iostream>
/**
 * @brief 在正规式需要连接的地方插入‘@’表示连接
 * @param string 待预处理的串
 * @return 处理后的串
 * @attention 同时调用get_characters构建字母表
 */
std::string pre_process(std::string s);

/**
 * @brief 中缀表达式转换为后缀表达式
 * @param string 中缀表达式
 * @return 后缀表达式
 */
std::string trans_postfix(std::string s);

/**
 * @brief 提取输入字符串中的字母表
 */
void get_characters(std::string s);

#endif