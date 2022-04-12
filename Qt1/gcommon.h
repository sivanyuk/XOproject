#ifndef GCOMMON_H
#define GCOMMON_H

#include <stdint.h>

enum class Player : uint32_t
{
    None = 0,   //no player or move
    O,
    X,
};


struct WinInfo //coordinates of winsituation
{
    uint8_t x_start;    //start posion
    uint8_t y_start;
    uint8_t x_end;      //end position
    uint8_t y_end;
    Player pl_win;  //who won
};

#endif // GCOMMON_H

