#ifndef COMPUTER_H
#define COMPUTER_H
#include "parser.h"

#include <map>
#include <QPlainTextEdit>

class Computer
{
public:
    explicit Computer(QPlainTextEdit* console);
    void get_code(const std::vector<std::string>& code);
    void start();
    bool execute_line(const std::string& line);
    void stop();

private:
    size_t m_current_line;                  // just like instruction pointer
    std::vector<double> m_memory;         // for now it holds unsigned integer
    std::vector<std::string> m_code;        // input code
    std::vector<size_t> m_debug_line;       // in which lines computer should stop
    std::map<std::string, size_t> m_labels; // [label-line] map
    QPlainTextEdit* m_console;

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
    bool process_cmd_print_newline(const std::vector<lang::type>& types, const std::vector<std::string>& tokens);
    bool process_cmd_nope();
    bool process_cmd_clear();

    bool process_math(const std::string& op_str, double& ref, double num1, double num2);
    bool process_condition(const std::string& jump_str, double num1, double num2, const std::string& label);

    void print_to_console(const std::string& text);
    void print_to_console(const QString& text);


    void clean();

    static double get_number(const std::string& str);
    std::pair<double&, bool> get_memory_ref(const std::string& str);
};

#endif // COMPUTER_H
