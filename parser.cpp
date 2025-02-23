#include "parser.h"

Parser::Parser()
{

}

std::tuple<std::vector<lang::type>, std::vector<std::string>, bool> Parser::parse_line(const std::string& line)
{
    // TODO: Instead of only returning type and sanity
    // return type, token and sanity !
    const auto tokens = split(line);
    std::vector<lang::type> lang_types(tokens.size(), lang::type::undefined);

    for (size_t idx = 0; idx < tokens.size(); idx++)
    {
        lang_types[idx] = get_type(tokens[idx]);
    }

    const bool is_ok = sanity_check(lang_types);

    return {lang_types, tokens, is_ok};
}

std::vector<std::string> Parser::split(const std::string& line)
{
    std::vector<std::string> split_vec;

    size_t last = 0;
    size_t next = 0;
    while ((next = line.find(' ', last)) != std::string::npos)
    {
        split_vec.push_back(line.substr(last, next - last));
        last = next + 1;
    }

    split_vec.push_back(line.substr(last));

    return split_vec;
}

lang::type Parser::get_type(const std::string& item)
{
    for (const auto& cmd : s_commands)
    {
        if (item == cmd)
        {
            return lang::type::command;
        }
    }

    if (item.substr(0, 1) == "#")
    {
        return lang::type::comment;
    }

    if ((item.substr(0, 1) == "\"") && (item.substr(item.length() - 1) == "\""))
    {
        return lang::type::string;
    }

    if ((item.substr(0, 1) == "_"))
    {
        return lang::type::label;
    }

    if (is_number(item))
    {
        return lang::type::number;
    }

    if ((item.substr(0, 1) == "$") && is_number(item.substr(1)))
    {
        return lang::type::address;
    }

    return lang::type::undefined;

}

bool Parser::is_number(const std::string& str)
{
    // Ref: https://stackoverflow.com/a/4654718
    std::string::const_iterator it = str.begin();

    while (it != str.end() && std::isdigit(*it))
    {
        ++it;
    }

    return !str.empty() && it == str.end();
}

bool Parser::sanity_check(const std::vector<lang::type>& types)
{
    if (types.empty())
    {
        return true;
    }

    if (types[0] == lang::type::comment)
    {
        return true;
    }

    if (types.size() > 4)
    {
        return false;
    }

    for (const auto& item : types)
    {
        if (item == lang::type::undefined)
        {
            return false;
        }
    }

    return true;
}

const std::vector<std::string> Parser::s_commands = {"MOV", "JMP", "JMPE", "JMPNE", "JMPB", "JMPBE",
                                                     "JMPS", "JMPSE", "NOP", "PRINT", "ADD", "SUB",
                                                     "MUL", "DIV"
                                                    };
