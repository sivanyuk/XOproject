#ifndef GBOARD_H
#define GBOARD_H

#include <stdint.h>
#include <memory>
#include <stdlib.h>
#include <time.h>
#include <random>
#include "xoscene.h"

#ifndef board_size
#define board_size XOScene::cells   //define Board size parameter
#endif

enum class StatusCell : uint16_t   //uint32_t  //ststus of one cell in one direction
{
    none = 0,
    s1c,    //1 closed
    s1o,    //1 opened
    s2c,
    s2o,
    s3c,
    s3o,
    s4,
};

struct WeightName
{
    StatusCell status;
    const char * name;
};

struct Cell_info
{
    Player  pl; // :2 ;  //player or none
    uint16_t weight_player; // : 15;   //weight of the cell of the current player
    uint16_t weight_opponent;   // :15 ;  //weight of the cell of the opponent
};

class Gboard
{
public:
    Gboard();
    Gboard(Player player);
    void init();   //clear the board
    static uint32_t getRnd(uint32_t start = 0, uint32_t end = -1);    //get random number
    void set_move(Player who, int posX, int posY);  //set a move for the player
    void set_player(Player who);    //set the main player
    Player get_player();
    Player get_opponet();
    Player check_cell(int x, int y);  //check board cell

    uint32_t calc_one_cell_weight(Player who, int cX, int cY, StatusCell* status_arr = nullptr) const;  //calculate the weight of a cell

    Cell_info get_cell_info(int posX, int posY);

    const WeightName weight_name[(int)(StatusCell::s4)+1] = {  //table of
        StatusCell::none, "none",
        StatusCell::s1c, "s1c", // 1 closed
        StatusCell::s1o, "s1o", // 1 opened
        StatusCell::s2c, "s2c",   // 1 closed
        StatusCell::s2o, "s2o",
        StatusCell::s3c, "s2c",
        StatusCell::s3o, "s3o",     //3 opened cells - high priority
        StatusCell::s4, "s4"      //4 in line - maximum priority
    };


private:

    struct MaxValue
    {
        uint16_t posX; //:6;
        uint16_t posY;   // :6;
        int16_t weightPlayer;   // : 10; //current weight maxV_player - for player, if maxV_opponent -  opponent
        int16_t weightOpponent; //  : 10; //current weight of opponent. maxV_player - for opponent, if maxV_opponent -  player
    };

    union board_
    {
        uint64_t line[board_size * board_size];	//linear board_size
        Cell_info cell[board_size][board_size];		//square board_size
    };

    struct BoardCoord   //board coordinates
    {
        int x;
        int y;
    };

    struct DirBoard //direction in the board
    {   int x;
        int y;
    };
    struct CellWeight
    {
        StatusCell status;
        int weight;
    };

    static const int val_arr_size = 8;   //size of MaxValue arrays
    Player the_player;  //the player for whom plays the computer
    Player the_opponent;  //the player for whom plays the computer

    MaxValue maxV_player[val_arr_size];    //array of maximum value for player
    MaxValue maxV_opponent[val_arr_size];    //array of maximum value for opponent
    MaxValue maxV_summ[val_arr_size];  //difference between player and opponent

    const DirBoard dir_arr[4] = {0, 1, 1, 1, 1, 0, 1, -1}; //4 direction to find the weight

    const DirBoard dir_arround[8] = {0, 1, 1, 1, 1, 0, 1, -1,  0, -1, -1, -1, -1, 0, -1, 1}; //8 direction to find weight arround
    const CellWeight weight[(int)(StatusCell::s4)+1] = {  //table of
        StatusCell::none, 0,
        StatusCell::s1c, 1, // 1 closed
        StatusCell::s1o, 5, // 1 opened
        StatusCell::s2c, 4,   // 1 closed
        StatusCell::s2o, 10,
        StatusCell::s3c, 10,
        StatusCell::s3o, 40,     //3 opened cells - high priority
        StatusCell::s4, 200      //4 in line - maximum priority
    };



    int minPosX; //minimum position (coordinates) of the player or opponent for X axis
    int minPosY; //minimum position (coordinates) of the player or opponent for Y axis
    int maxPosX; //maximum position (coordinates) of the player or opponent for X axis
    int maxPosY; //maximum position (coordinates) of the player or opponent for Y axis

    std::unique_ptr<board_> board;
    inline bool chk_boundaries(int pos) const;    //check boundaries
    StatusCell calc_dir_weight(Player who, int cX, int cY, int dirX, int dirY) const;   //calc weight in one direction
    void calc_weight_arround(Player player, int cX, int cY, StatusCell* status_arr = nullptr);   //calculate weight arround in one way (one direction). Player - the current computer
    BoardCoord find_max_wights();    //find maximum for player and opponent
    inline int find_start_coorinates(int);  //find minmum cell in the board for start search
    inline int find_end_coordinates(int);  //find maximum cell in the board for end search
    BoardCoord gen_rnd_rez(MaxValue * maxVarr, int arr_lenth);  //get a random number if there are few results with the same weight
    int recalc_pl_weight(int player_weight);    //recalculate player weight for calculation sum of player and opponents weight
    void find_max_summ(MaxValue & maxVarr, MaxValue * maxV_summ, int & adr_Vsum, int & max_Vsum ); //function for calculation
};




#endif // GBOARD_H
