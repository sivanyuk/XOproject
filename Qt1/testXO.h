/*
Created by Sergey Ivanyuk 2022
GitHub: https://github.com/sivanyuk
*/

#ifndef TESTXO_H
#define TESTXO_H

#include "gboard.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <exception>

//exception class
class XOEx : public std::exception {
    //const char* msg;
    std::string ss;
    XOEx();    // no default constructor
public:
    explicit XOEx(const std::string s) throw(): ss(s) {};
    const char* what() const throw() { return ss.c_str(); }
};


struct XOTestLine
{
const static int32_t startX = 10;
const static int32_t startY = 10;
    int32_t x; //start coordinates x, y
    int32_t y;
    Player pl;  //X or O
    int32_t repeat; //number of repeats (+ to existing)
    int32_t dirX;  //direction X -1, 0 or 1
    int32_t dirY;
    bool fOpponent;

};



const XOTestLine  test1[] =
{
    XOTestLine::startX,     XOTestLine::startY,     Player::O, 1, -1, 0, false, //line of O without X at the end
    XOTestLine::startX + 3, XOTestLine::startY+ 1,  Player::O, 1,  1, 1, false, //line of O without X at the end
    -1,     //-1 is the end of the array
};





#endif // TESTXO_H
