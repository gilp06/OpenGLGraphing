//
// Created by GPowers on 10/24/2022.
//

#ifndef OPENGL_PARSER_HPP
#define OPENGL_PARSER_HPP

#include "string"

class Parser {
public:
    static float RPNToFloat(const std::string& equation, float x_val);
};


#endif //OPENGL_PARSER_HPP
