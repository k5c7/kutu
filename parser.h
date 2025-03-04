#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <utility>
#include <spdlog/spdlog.h>
#include <cstdint>

namespace lang
{
enum type {command, label, comment, string, number, address, undefined};
}

class Parser
{
public:
    Parser();
    static std::tuple<std::vector<lang::type>, std::vector<std::string>, bool> parse_line(const std::string& line);

private:
    static std::vector<std::string> split(const std::string& line);
    static lang::type get_type(const std::string& item);
    static bool is_number(const std::string& str);
    static bool sanity_check(const std::vector<lang::type>& types);

    static const std::vector<std::string> s_commands;
    //    static const std::vector

};

#endif // PARSER_H
