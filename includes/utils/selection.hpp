#pragma once

#include "inquirer.hpp"
#include "str_utils.hpp"

/**
 * @brief Show a selection message and await the user input.
 */
inline std::string selection(const std::string &q, const std::vector<std::string> &arr) {
    auto inquirer = alx::Inquirer("");
    auto token = randomString(8);
    inquirer.add_question({token, q, arr});
    inquirer.ask();
    auto ans = inquirer.answer(token);
    return ans;
}