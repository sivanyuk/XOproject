/*
Created by Sergey Ivanyuk 2022
GitHub: https://github.com/sivanyuk
*/
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

enum class WeiCalcFl : uint32_t //flags for weight calculation
{
    f3c_player = 999,
    f3c_opponent = 1999,
    f2o_foreview = 2999,
    f3c_foreview = 3999,
};

enum class StatusCell : uint8_t   //uint32_t  //ststus of one cell in one direction
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

struct MaxWeightInfo     //information about the next computer's move calculation
{
    //MaxWeightInfo() : x{0}, y{0}, weightPlayer{0}, weightOpponent{0}, weight_sum{0} {};
    int16_t x; //:6;
    int16_t y;   // :6;
    int16_t weightPlayer;   // : 10; //current weight maxV_player - for player, if maxV_opponent -  opponent
    int16_t weightOpponent; //  : 10; //current weight of opponent. maxV_player - for opponent, if maxV_opponent -  player
    int16_t weight_sum; //result summ of player + opponent
};


struct WeightName
{
    StatusCell status;
    const char * name;
};

struct Cell_info
{
    StatusCell pl_stat_arr[4];  //array of status cell for player
    StatusCell op_stat_arr[4];  //for opponent
    Player  pl; // :2 ;  //player X, O or none
    uint16_t pl_weight; // : 15;   //weight of the cell of the current player
    uint16_t op_weight;   // :15 ;  //weight of the cell of the opponent
};

class Gboard
{
public:
    Gboard();
    Gboard(Player player);


    void init();   //clear the board
    static uint32_t getRnd(uint32_t start = 0, uint32_t end = -1);    //get random number
    bool set_move(Player who, int posX, int posY);  //set a move for the player with checking the win situation
    bool chek_win_situation(int posX, int posY);  //check the win situation after move into the cell. Output - true: thewin position was found
    void set_player(Player who);    //set the main player
    Player get_player();
    Player get_opponet();
    Player check_cell(int x, int y);  //check board cell

    uint32_t calc_one_cell_weight(Player who, int cX, int cY, StatusCell* status_arr = nullptr) const;  //calculate the weight of a cell

    Cell_info get_cell_info(int posX, int posY);

    WinInfo get_win_info() {return win_info;}   //get information abpout the winner

    MaxWeightInfo  find_max_weights();    //find maximum for player and opponent

    static bool chk_boundaries(int pos);    //check boundaries
    const WeightName weight_name[(int)(StatusCell::s4)+1] = {  //table of
        StatusCell::none, "none",
        StatusCell::s1c, "s1c", // 1 closed
        StatusCell::s1o, "s1o", // 1 opened
        StatusCell::s2c, "s2c",   // 1 closed
        StatusCell::s2o, "s2o",
        StatusCell::s3c, "s3c",
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

    union Board
    {
        uint32_t line[board_size * board_size * sizeof(Cell_info) /sizeof(uint32_t)];	//linear board_size
        Cell_info cell[board_size][board_size];		//square board_size
    };

    struct Coord   //board coordinates
    {
        int8_t x;
        int8_t y;
        void operator = (Coord & rhs) {x = rhs.x; y = rhs.y;}   //copy data
        bool operator == (Coord & rhs) {return x == rhs.x && y == rhs.y;}   //compare elements
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
    enum class Dist12: int //check if distance betwee point 1 or 2
    {
        yes = 0,    //distance 1 or 2 for x and y
        no,     //distance > 2 for one coordinate
        both_no //distance > 2 for both
    };

    static const int val_arr_size = 16;   //size of MaxValue arrays
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
        StatusCell::s1o, 6, // 1 opened
        StatusCell::s2c, 4,   // 1 closed
        StatusCell::s2o, 16,
        StatusCell::s3c, 12,
        StatusCell::s3o, 80,     //3 opened cells - high priority
        StatusCell::s4, 400      //4 in line - maximum priority
    };

    int minPosX; //minimum position (coordinates) of the player or opponent for X axis
    int minPosY; //minimum position (coordinates) of the player or opponent for Y axis
    int maxPosX; //maximum position (coordinates) of the player or opponent for X axis
    int maxPosY; //maximum position (coordinates) of the player or opponent for Y axis

    WinInfo win_info; //in
    bool fgame_on;    //game is going on

    //std::unique_ptr<Board>
    Board board;


    StatusCell calc_dir_weight(Player who, int cX, int cY, int dirX, int dirY) const;   //calc weight in one direction
    void calc_weight_arround(Player player, int cX, int cY, StatusCell* status_arr = nullptr);   //calculate weight arround in one way (one direction). Player - the current computer
    inline int find_start_coorinates(int);  //find minmum cell in the board for start search
    inline int find_end_coordinates(int);  //find maximum cell in the board for end search
    int recalc_pl_weight(int player_weight);    //recalculate player weight for calculation sum of player and opponents weight

    /*
     * fill array for maximum summary weights of player + opponent (maxV_summ_arr)
     * MaxValue - value from arrays player/opponent
     * adr_Vsum - length of maxV_summ_arr
     * max_Vsum - maximum value for the
     */
    void find_max_summ(MaxValue & maxVarr, MaxValue * maxV_summ_arr, int & adr_Vsum, int & max_Vsum ); //function for calculation

    void copy_max_wei(MaxWeightInfo & maxWinfo, MaxValue & maxV, uint32_t max_sum = 0);   //copy max weight

    void copy_max_wei(MaxWeightInfo &maxWinfo, int posX, int posY, Cell_info * cell_info,  uint32_t max_sum);   //copy max weight variant 2

    void copy_max_wei(MaxWeightInfo &maxWinfo, Coord & coord , Cell_info & cell_info, uint32_t max_sum);

    void  gen_rnd_rez(MaxWeightInfo &max_wei_inf,  MaxValue * maxVarr, int arr_lenth, uint32_t max_sum = 0);  //get a random number if there are few results with the same weight

    /*
     * check a direction for winning situation
    */
    bool chek_win_line(Player who, int cX, int cY, int dirX, int dirY); //chek for winning line (4 in line)

    bool is_game_on() {return fgame_on;};    //check the flag if the game is going on

    /*
     * check if here the cell with 3c ststus
     */
    bool chk_status_3c(MaxWeightInfo &max_wei_inf, MaxValue *maxVarr, int arr_length, bool fPlayer); //check for 2o, 3c status

    /*
    * chek if this cell has status of 3 closed or 2 opened
    * f Player  true for player, false for opponent
    * fChk_both: if both -to check situation for 2o and 2c, false check only 3c
    */
    bool check_2o3c(int x, int y, bool fPlayer, bool fChk_both = false);   //check if pressents 3 close situation

    /*
     * check situation when both players have 2*2o/3c configuration
     * if player or/and oppenent have 3o - situatio - fill max_wei_inf with result= true
     */
    bool find_both_3c(MaxWeightInfo &max_wei_inf, int startX, int startY, int endX, int endY);    //ckeck if both players have situation 2* 2o/3c situation

    /*
     * check winning fork in the move after current one
     * max_wei_inf - cordinates if situation will be found with result == true
     */
    bool check_foreview(MaxWeightInfo &max_wei_inf, int startX, int startY, int endX, int endY);    //check situation if wining can be in apear in a move after next  one

    Dist12 chk_dist (Coord & ref, Coord & target); //compatre for distance 1 or 2 for x and y between 2 points

    /*
     * check winning for 3 cell with 2o or 3c each
     * max_wei_inf - cordinates if situation will be found with result == true
     *  ref, target - ccordinates of 2 cells
     *  f3c - flag if cell with 3 point will be found
     *  fPl flage true for player, false - opponent
     *  result: true if found foreview fork with 3c
     */
    bool chk_foreview2(MaxWeightInfo &max_wei_inf, Coord & ref, Coord & target, bool fPl, bool &fFound); //,  bool & f3c);


    /*
     * find a free cell when move in the certain direction
     */
    bool go_near(Player who, Coord & coord, int dir_x, int dir_y);

    /*
     * wirte information if found foreview fork
     * result = true if here is foreview for with 3c line if it's
    */
    bool write_foreview_info(MaxWeightInfo &max_wei_inf, Coord& ref, Coord& targ, bool & fFound_local, bool &fFound, /*bool &f3c, */ bool f3c_local);
};

#endif // GBOARD_H

