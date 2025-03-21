#include <gtest/gtest.h>
#include "computer.h"

TEST(computerTest, MathOperations)
{
    std::vector<std::string> code {"ADD $0 1 2",
                                   "SUB $1 -5 -3",
                                   "MUL $2 $0 $1",
                                   "DIV $3 15 5"};

    std::vector<double> expected{3, -2, -6, 3};

    Computer computer(nullptr);
    computer.get_code(code);
    computer.start();

    for (size_t idx = 0; idx < expected.size(); idx++)
    {
        const double calculated = computer.get_memory(idx);
        EXPECT_EQ(calculated, expected[idx]);
    }
}

