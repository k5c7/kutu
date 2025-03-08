#include "computer.h"
#include <algorithm>
#include <stdexcept>


Computer::Computer(QPlainTextEdit* console) :
    m_current_line{0},
    m_memory{std::vector<double>(100, 0)},
    m_console{console}
{

}

void Computer::get_code(const std::vector<std::string>& code)
{
    m_code = code;
}

void Computer::start()
{
    spdlog::info("Computer started");
    clean();
    scan_labels();
    const size_t max_line = m_code.size();

    while(m_current_line < max_line)
    {
        const bool is_ok = execute_line(m_code[m_current_line]);
        if(!is_ok)
        {
            spdlog::warn("Execution stopped due to syntax error at line {}", m_current_line);
            stop();
            return;
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
        return process_cmd_move(types, tokens);
    }
    else if(tokens[0] == "PRINT")
    {
        return process_cmd_print(types, tokens);
    }
    else if(tokens[0] == "PRINTNL")
    {
        return process_cmd_print_newline(types, tokens);
    }
    else if((tokens[0] == "ADD") || (tokens[0] == "SUB") ||
            (tokens[0] == "MUL") || (tokens[0] == "DIV"))
    {
        return process_cmd_math(types, tokens);
    }

    else if((tokens[0] == "JMP") || (tokens[0] == "JMPE") || (tokens[0] == "JMPNE") ||
            (tokens[0] == "JMPB") || (tokens[0] == "JMPBE") || (tokens[0] == "JMPS") ||
            (tokens[0] == "JMPSE"))
    {
        return process_cmd_jump(types, tokens);
    }
    else if(tokens[0] == "NOP")
    {
        return process_cmd_nope();
    }
    else
    {
        spdlog::error("Command is not defined {}", tokens[0]);
        throw std::invalid_argument("Command is not defined");
    }
    return false;
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
        spdlog::warn("Second token must be address instead of {}", types[1]);
        return false;
    }

    const auto [ref_1, is_ok_1] = get_memory_ref(tokens[1]);
    if(!is_ok_1)
    {
        return false;
    }

    if((types[2] == lang::type::address))
    {
        const auto [ref_2, is_ok_2] = get_memory_ref(tokens[2]);

        if(is_ok_2)
        {
            ref_1 = ref_2;
            return true;
        }
    }
    else if((types[2] == lang::type::number))
    {
        const auto number = get_number(tokens[2]);
        ref_1 = number;
        return true;
    }

    return false;
}

bool Computer::process_cmd_jump(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    if(tokens[0] == "JMP")
    {
        return process_cmd_jump_absolute(types, tokens);
    }
    else
    {
        return process_cmd_jump_conditional(types, tokens);
    }
}

bool Computer::process_cmd_jump_absolute(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    // JMP _LABEL
    if(tokens.size() != 2)
    {
        spdlog::warn("Number of tokens must be 2 for absolute jump, it has {}", tokens.size());
        return false;
    }
    else if(types[1] != lang::type::label)
    {
        spdlog::warn("Second token must be label, it is {}", types[1]);
        return false;
    }

    const std::string label = tokens[1].substr(1);

    if(!m_labels.contains(label))
    {
        spdlog::warn("{} is not found in labels", label);
        return false;
    }

    m_current_line = m_labels.at(label) - 1;
    return true;
}

bool Computer::process_cmd_jump_conditional(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    if(tokens.size() != 4)
    {
        spdlog::warn("Number of tokens must be 4 for absolute jump, it has {}", tokens.size());
        return false;
    }
    else if(types[3] != lang::type::label)
    {
        spdlog::warn("Fourth token must be label, it is {}", types[1]);
        return false;
    }

    const std::string label = tokens[3].substr(1);

    if(!m_labels.contains(label))
    {
        spdlog::warn("{} is not found in labels", label);
        return false;
    }

    if(types[1] == lang::type::address && types[2] == lang::type::address)
    {
        const auto [num1, is_ok_1] = get_memory_ref(tokens[1]);
        const auto [num2, is_ok_2] = get_memory_ref(tokens[2]);
        if(is_ok_1 && is_ok_2)
        {
            return process_condition(tokens[0], num1, num2, label);
        }
    }
    else if(types[1] == lang::type::address && types[2] == lang::type::number)
    {
        const auto [num1, is_ok_1] = get_memory_ref(tokens[1]);
        const auto num2 = get_number(tokens[2]);
        if(is_ok_1)
        {
            return process_condition(tokens[0], num1, num2, label);
        }
    }
    else if(types[1] == lang::type::number && types[2] == lang::type::address)
    {
        const auto num1 = get_number(tokens[1]);
        const auto [num2, is_ok_2] = get_memory_ref(tokens[2]);
        if(is_ok_2)
        {
            return process_condition(tokens[0], num1, num2, label);
        }
    }
    else if(types[1] == lang::type::number && types[2] == lang::type::number)
    {
        const auto num1 = get_number(tokens[1]);
        const auto num2 = get_number(tokens[2]);
        return process_condition(tokens[0], num1, num2, label);
    }

    return false;
}

bool Computer::process_cmd_math(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    // FIXME: Only add implemented!
    if((types.size() != 4))
    {
        spdlog::warn("Number of tokens must be 4 for math operations, it has {}", types.size());
        return false;
    }

    if(types[1] != lang::type::address)
    {
        spdlog::warn("Second token must be address instead of {}", types[1]);
        return false;
    }

    const auto [ref_1, is_ok_1] = get_memory_ref(tokens[1]);
    if(!is_ok_1)
    {
        return false;
    }

    if((types[2] == lang::type::address) && (types[3] == lang::type::address))
    {
        const auto [ref_2, is_ok_2] = get_memory_ref(tokens[2]);
        const auto [ref_3, is_ok_3] = get_memory_ref(tokens[3]);
        if(is_ok_2 && is_ok_3)
        {
            return process_math(tokens[0], ref_1, ref_2, ref_3);
        }
    }
    else if((types[2] == lang::type::address) && (types[3] == lang::type::number))
    {
        const auto [ref_2, is_ok_2] = get_memory_ref(tokens[2]);
        const auto number = get_number(tokens[3]);
        if(is_ok_2)
        {
            return process_math(tokens[0], ref_1, ref_2, number);
        }
    }
    else if((types[2] == lang::type::number) && (types[3] == lang::type::address))
    {
        const auto [ref_3, is_ok_3] = get_memory_ref(tokens[3]);
        const auto number = get_number(tokens[2]);
        if(is_ok_3)
        {
            return process_math(tokens[0], ref_1, ref_3, number);
        }
    }
    else if((types[2] == lang::type::number) && (types[3] == lang::type::number))
    {
        const auto number_2 = get_number(tokens[2]);
        const auto number_3 = get_number(tokens[3]);
        return process_math(tokens[0], ref_1, number_2, number_3);
    }

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
            print_to_console(QString::number(ref));
            spdlog::debug("{}", ref);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(types[1] == lang::type::number)
    {
        print_to_console(QString::number(get_number(tokens[1])));
        spdlog::debug("{}", get_number(tokens[1]));
        return true;
    }
    else if (types[1] == lang::type::string)
    {
        const auto text = tokens[1].substr(1, tokens[1].size() - 2);
        print_to_console(QString::fromStdString(text));
        spdlog::debug("{}", text);
        return true;
    }
    else
    {
        spdlog::warn("Invalid argument for print: {}", types[1]);
        return false;
    }

}

bool Computer::process_cmd_print_newline(const std::vector<lang::type>& types, const std::vector<std::string>& tokens)
{
    if(types.size() != 1)
    {
        spdlog::warn("Println has {} argument instead of 0", types.size() - 1);
        return false;
    }
    m_console->appendPlainText("");
    return true;
}

bool Computer::process_cmd_nope()
{
    // FIXME: This function doesn't need types and tokens :)
    return true;
}

bool Computer::process_math(const std::string& op_str, double& ref, double num1, double num2)
{
    if(op_str == "ADD")
    {
        ref = num1 + num2;
    }
    else if(op_str == "SUB")
    {
        ref = num1 - num2;
    }
    else if(op_str == "MUL")
    {
        ref = num1 * num2;
    }
    else if(op_str == "DIV")
    {
        ref = num1 / num2;
    }
    else
    {
        spdlog::warn("Math operation as {} is not defined", op_str);
        return false;
    }

    return true;
}

bool Computer::process_condition(const std::string& jump_str, double num1, double num2, const std::string& label)
{
    if(((jump_str == "JMPE") && (num1 == num2))       ||
            ((jump_str == "JMPNE") && (num1 != num2)) ||
            ((jump_str == "JMPB") && (num1 > num2))   ||
            ((jump_str == "JMPBE") && (num1 >= num2)) ||
            ((jump_str == "JMPS") && (num1 < num2))   ||
            ((jump_str == "JMPSE") && (num1 <= num2)))
    {
        m_current_line = m_labels[label] - 1;

    }

    return true;
}

void Computer::print_to_console(const std::string& text)
{
    m_console->moveCursor (QTextCursor::End);
    m_console->insertPlainText(QString::fromStdString(text));
    m_console->moveCursor (QTextCursor::End);
}

void Computer::print_to_console(const QString& text)
{
    m_console->moveCursor (QTextCursor::End);
    m_console->insertPlainText(text);
    m_console->moveCursor (QTextCursor::End);
}

void Computer::clean()
{
    m_labels.clear();
    std::fill(m_memory.begin(), m_memory.end(), 0);
    spdlog::debug("Memory and labels are deleted");
}

double Computer::get_number(const std::string& str)
{
    return stod(str);
}

std::pair<double&, bool> Computer::get_memory_ref(const std::string& str)
{
    const size_t number = static_cast<size_t>(get_number(str.substr(1)));
    if(number >= m_memory.size())
    {
        spdlog::warn("number is big or equal than memory size {} > {}", number, m_memory.size() - 1);
        return{m_memory.at(0), false};
    }

    return {m_memory.at(number), true};
}


