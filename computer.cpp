#include "computer.h"
#include <stdexcept>

Computer::Computer() :
    m_current_line{0},
    m_memory{std::vector<uint32_t>(100, 0)},
    m_code{},
    m_debug_line{},
    m_labels{}
{

}

void Computer::get_code(const std::vector<std::string>& code)
{
    m_code = code;
}

void Computer::start()
{
    spdlog::info("Computer started");
    scan_labels();
    const size_t max_line = m_code.size();

    while(m_current_line < max_line)
    {
        const bool is_ok = execute_line(m_code[m_current_line]);
        if(!is_ok)
        {
            spdlog::warn("Execution stopped due to syntax error at line {}", m_current_line);
            stop();
        }
    }

    spdlog::info("Execution stopped in a normal way");
    stop();
}

bool Computer::execute_line(const std::string& line)
{
    const auto [types, tokens, sanity_check] = Parser::parse_line(line);
    if(!sanity_check)
    {
        return false;
    }

    return process(types, tokens);
}

void Computer::stop()
{
    spdlog::info("Computer stopped");
    m_current_line = 0;
    m_debug_line.clear();
    m_code.clear();
    std::fill(m_memory.begin(), m_memory.end(), 0);
}

void Computer::scan_labels()
{
    const size_t code_size = m_code.size();

    for (size_t idx = 0; idx < code_size; idx++)
    {
        const auto [types, tokens, sanity_check] = Parser::parse_line(m_code[idx]);

        if(sanity_check && (types[0] == lang::type::label) && (types.size() == 1))
        {
            const std::string label = m_code[idx].substr(1);

            if(m_labels.contains(label))
            {
                spdlog::warn("You can't use same label ({}) more than once, overwriting {} with {} ",
                             label, m_labels[label], idx);
            }

            m_labels[label] = idx;
        }
    }
}

bool Computer::process(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    bool is_process_ok = false;

    if(types[0] == lang::type::command)
    {
        is_process_ok = process_command(types, tokens);
    }
    else if((types[0] == lang::type::comment) || (types[0] == lang::type::label))
    {
        is_process_ok = true;
    }
    else
    {
        is_process_ok = false;
        spdlog::warn("Error occured in process in line {}", m_current_line);
    }

    m_current_line++;
    return is_process_ok;
}

bool Computer::process_command(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    if(tokens[0] == "MOV")
    {
        process_cmd_move(types, tokens);
    }
    else if(tokens[0] == "PRINT")
    {
        process_cmd_print(types, tokens);
    }
    else if((tokens[0] == "ADD") || (tokens[0] == "SUB") ||
            (tokens[0] == "MUL") || (tokens[0] == "DIV"))
    {
        process_cmd_math(types, tokens);
    }
    else if((tokens[0] == "JMP") || (tokens[0] == "JE") || (tokens[0] == "JNE") ||
            (tokens[0] == "JB") || (tokens[0] == "JBE") || (tokens[0] == "JS") ||
            (tokens[0] == "JSE"))
    {
        process_cmd_jump(types, tokens);
    }
    else if(tokens[0] == "NOP")
    {
        process_cmd_nope(types, tokens);
    }
    else
    {
        spdlog::error("Command is not defined {}", tokens[0]);
        throw std::invalid_argument("Command is not defined");
    }
}

bool Computer::process_cmd_move(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    if((types.size() != 3))
    {
        spdlog::warn("Number of tokens must be 3 for MOV, it has {}", types.size());
        return false;
    }

    if(types[1] != lang::type::address)
    {
        spdlog::warn("Second token must be address but it is {}", types[1]);
        return false;
    }

    if(!(types[2] == lang::type::address) && !(types[2] == lang::type::number))
    {
        spdlog::warn("Third token must be address or number but it is {}", types[2]);
        return false;
    }

    const auto [ref, is_ok] = get_memory_ref(tokens[1]);
    if(is_ok)
    {
        if(types[2] == lang::type::number)
        {
            ref = get_number(tokens[2]);
            return true;
        }
        else
        {
            const auto [ref_2, is_ok_2] = get_memory_ref(tokens[2]);
            if(is_ok_2)
            {
                ref = ref_2;
                return true;
            }
            else
            {
                spdlog::warn("Memory reference couldn't be fetched for {}", tokens[2]);
                return false;
            }
        }
    }
    else
    {
        spdlog::warn("Memory reference couldn't be fetched for {}", tokens[1]);
        return false;
    }
}

bool Computer::process_cmd_jump(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    return false;
}

bool Computer::process_cmd_math(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    return false;
}

bool Computer::process_cmd_print(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    if(types.size() != 2)
    {
        spdlog::warn("Print has {} argument instead of 1", types.size() - 1);
        return false;
    }

    if(types[1] == lang::type::address)
    {
        const auto [ref, is_ok] = get_memory_ref(tokens[1]);
        if(is_ok)
        {
            spdlog::info("{}", ref); // TODO: Change spdlog to console
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(types[1] == lang::type::number)
    {
        spdlog::info("{}", get_number(tokens[1]));
        return true;
    }
    else if (types[1] == lang::type::string)
    {
        spdlog::info("{}", tokens[1].substr(1, tokens[1].size() - 2));
        return true;
    }
    else
    {
        spdlog::warn("Invalid argument for print: {}", types[1]);
        return false;
    }

}

bool Computer::process_cmd_nope(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    // FIXME: This function doesn't need types and tokens :)
    return true;
}

uint32_t Computer::get_number(const std::string& str)
{
    return stoi(str);
}

std::pair<uint32_t&, bool> Computer::get_memory_ref(const std::string& str)
{
    const uint32_t number = get_number(str.substr(1));
    if(number >= m_memory.size())
    {
        spdlog::warn("number is big or equal than memory size {} >= {}", number, m_memory.size());
        return{m_memory.at(0), false};
    }

    return {m_memory.at(number), true};
}


