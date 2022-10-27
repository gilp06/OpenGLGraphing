//
// Created by GPowers on 10/24/2022.
//

#include "Parser.hpp"

#include "stack"
#include "sstream"
#include "cmath"

float Parser::RPNToFloat(const std::string &equation, float x_val) {
    std::stack<float> operands;
    std::stringstream stream(equation);
    std::string cur;
    while(stream >> cur)
    {
        try {
            float value = std::stof(cur);
            operands.push(value);
        }
        catch (std::exception&) {
            if(cur == "x")
            {
                operands.push(x_val);
                continue;
            }

            if(operands.empty())
            {
                continue;
            }

            if(cur == "sqrt") {
                float a = operands.top();
                operands.pop();
                operands.push(sqrtf(a));
                continue;
            }
            if(cur == "sin")
            {
                float a = operands.top();
                operands.pop();
                operands.push(sinf(a));
                continue;
            }
            if(cur == "cos")
            {
                float a = operands.top();
                operands.pop();
                operands.push(cosf(a));
                continue;
            }
            if(cur == "tan")
            {
                float a = operands.top();
                operands.pop();
                operands.push(tanf(a));
                continue;
            }



            if(operands.size() < 2) {
                continue;
            }

            float a = operands.top();
            operands.pop();
            float b = operands.top();
            operands.pop();

            if(cur == "+")
            {
                operands.push(a+b);
            }
            if(cur == "-")
            {
                operands.push(b-a);
            }
            if(cur == "*")
            {
                operands.push(a*b);
            }
            if(cur == "/")
            {
                operands.push(b/a);
            }
            if(cur == "^")
            {
                operands.push(powf(b, a));
            }
            else
            {
                continue;
            }
        }
    }
    if(operands.empty()) {
        return INT32_MIN;
    }
    return operands.top();
}
