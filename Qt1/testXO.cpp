/*
Created by Sergey Ivanyuk 2022
GitHub: https://github.com/sivanyuk
*/


#include    "mainwi.h"
#include "common_h.h"
#include <string>
#include <exception>
#include <cstdlib>
#include <stdexcept>
#include <QException>
#include <cmath>

void MainW::drawTestLIne(const XOTestLine *testline, const char* name)
{
    scene->clear();
    scene->drawCells();
    gboard -> init();
    gboard -> set_player(Player::O);
    //pl_move = Player::X;  //who should move now
    int i =0 ;
    int x, y, dirX, dirY;
    int repeate;\
    std::stringstream ss;
    Player pl, opponent;
    while (true)
    {
        x = testline[i].x;
        if (x < 0)
        {

            break;   //do antill positiove coordinats
        }
        y = testline[i].y;
        pl = testline[i].pl;
        repeate = testline[i].repeat;
        dirX = testline[i].dirX;
        dirY = testline[i].dirY;

        if (abs(dirX) >1)
        {
            ss << "dirX out of range ( " << dirX << "). Array's Line: " << std::to_string(i) << ".\nTest = "<< name << ", test line: " << i << ".\n File: " << __FILE__  << ", line " << __LINE__ ;
            throw XOEx(ss.str());
        }
        if (abs(dirY) >1)
        {
            ss << "dirX out of range ( " << dirY << "). Array's Line: " << std::to_string(i) << ". \nTest = "<< name << ", test line: " << i << ".\nFile: " << __FILE__  << ", line " << __LINE__ ;
            throw XOEx(ss.str());
        }
        while (repeate >=0)
        {
           if (!Gboard::chk_boundaries(x) || !Gboard::chk_boundaries(y))
           {
               ss << "x or y dirX is out of range. x = " << x <<  ",y = " << y << ".\n Test = ";    //<< name << ", test line: " << i << ".\nFile: " << __FILE__  << ", line " << __LINE__ ;
               throw XOEx(ss.str());
           }
           scene-> draw_player(pl, x, y );   //draw O
           gboard -> set_move(pl, x, y);   //make a move
           x += dirX;
           y += dirY;
           repeate --;
        }
        i++;
    }

}


void MainW::on_testBut_pressed()
{
    try
    {
        drawTestLIne(test1, "test1");
    }
    catch (XOEx & e)
    {
        showErrDialog((char*)e.what());
    }
}
