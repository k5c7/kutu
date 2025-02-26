#ifndef COMPUTER_H
#define COMPUTER_H
#include "parser.h"

#include <map>

class Computer
{
public:
    Computer();
    void get_code(const std::vector<std::string>& code);
    void start();
    bool execute_line(const std::string& line);
    void stop();

private:
    size_t m_current_line;                  // just like instruction pointer
    std::vector<uint32_t> m_memory;         // for now it holds unsigned integer
    std::vector<std::string> m_code;        // input code
    std::vector<size_t> m_debug_line;       // in which lines computer should stop
    std::map<std::string, size_t> m_labels; // [label-line] map


    void scan_labels();
    bool process(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);
    bool process_command(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);
    bool process_comment_and_label(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);

    bool process_cmd_move(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);
    bool process_cmd_jump(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);
    bool process_cmd_jump_absolute(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);
    bool process_cmd_jump_conditional(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);
    bool process_cmd_math(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);
    bool process_cmd_print(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);
    bool process_cmd_nope(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);

    bool process_math(const std::string& op_str, uint32_t& ref, uint32_t num1, uint32_t num2);
    bool process_condition(const std::string& jump_str, int num1, int num2, const std::string& label);


    static uint32_t get_number(const std::string& str);
    std::pair<uint32_t&, bool> get_memory_ref(const std::string& str);
};

#endif // COMPUTER_H
