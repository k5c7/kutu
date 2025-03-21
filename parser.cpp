#include "parser.h"
#include <regex>

Parser::Parser()
{

}

std::tuple<std::vector<lang::type>, std::vector<std::string>, bool> Parser::parse_line(const std::string& line)
{
    // FIXME: Not a proper solution
    if(line.empty())
    {
        return {{lang::type::command}, {"NOP"}, true};
    }

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
    // Regexp : [^\s"']+|"([^"]*)"|'([^']*)'
    // Ref: stackoverflow.com/a/366532
    const std::string pattern = std::string(R"lit([^\s"']+|"([^"]*)"|'([^']*)')lit");
    const std::regex re(pattern);

    auto words_begin = std::sregex_iterator(line.begin(), line.end(), re);
    auto words_end = std::sregex_iterator();

    std::vector<std::string> split_vec;

    for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    {
        split_vec.push_back(i->str());
    }

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
    try
    {
        double num = std::stod(str);
        return true;
    }
    catch(...)
    {
        return false;
    }
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

void Parser::combine_string_elements(std::vector<std::string>& line)
{
    // FIXME: It doesn't work when string has whitespace at the
    // end of the string as: PRINT "a "
    size_t start_idx = 0;
    size_t stop_idx = 0;

    for (size_t idx = 0; idx < line.size(); idx++)
    {
        if (line[idx].front() == '\"')
        {
            start_idx = idx;
        }
        else if (line[idx].back() == '\"')
        {
            stop_idx = idx;
        }
    }

    if ((start_idx != 0) && (stop_idx != 0))
    {
        std::vector<std::string> vec;

        for (size_t idx = 0; idx < start_idx; idx++)
        {
            vec.push_back(line[idx]);
        }

        std::string combined_string;
        for (size_t idx = start_idx; idx <= stop_idx; idx++)
        {
            combined_string += line[idx];

            if(idx != stop_idx)
            {
                combined_string += " ";
            }
        }
        if(line[stop_idx] == "\"")
        {
            combined_string += " ";
        }
        vec.push_back(combined_string);

        for (size_t idx = stop_idx + 1; idx < line.size(); idx++)
        {
            vec.push_back(line[idx]);
        }

        line = vec;
    }
}

const std::vector<std::string> Parser::s_commands = {"MOV", "JMP", "JMPE", "JMPNE", "JMPB", "JMPBE",
                                                     "JMPS", "JMPSE", "NOP", "PRINT", "PRINTNL",
                                                     "ADD", "SUB", "MUL", "DIV"
                                                    };
