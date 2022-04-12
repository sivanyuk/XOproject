/*
Created by Sergey Ivanyuk 2022
GitHub: https://github.com/sivanyuk
*/

#include "common_h.h"
#include <cstdlib>
#include <cmath>

Gboard::Gboard()
{
    //win_.pl_win = Player::None;
    assert(weight[(int)StatusCell::s3c].weight <= weight[(int)StatusCell::s2o].weight);     //compare weight of 3c and 2o
}

Gboard::Gboard(Player player): the_player(player)
{
    Gboard();
}

void Gboard::init()
{
    int i;
    for (i=0; i< sizeof(Board)/sizeof(board.line[0]); ++i) board.line[i] =0;
    minPosY = minPosX = board_size -1;
    maxPosX = maxPosY = 0;
    fgame_on = true;    //game is going on

}

bool Gboard::chk_boundaries(int pos)    //check the boundaries
{
    return (pos >=0) && pos < board_size;
}

uint32_t Gboard::calc_one_cell_weight(Player who, int cX, int cY , StatusCell* status_arr) const //calculate the weight of the current (one cell) cell
{

    StatusCell status;   //satun for 4 directions
    int i;
    int curStatusN;   //current status (convertation to integer for reading from the weight[])
    int summ_weight = 0;
    for (i =0; i < 4; ++i)
    {

       status = calc_dir_weight(who, cX, cY, dir_arr[i].x, dir_arr[i].y);    //find sattus for the direction
       if (status_arr) status_arr[i] = status;  //if here isn't null ptr
       curStatusN = (int) status;
       assert(curStatusN < sizeof(weight)/sizeof(weight[0]) );  //check if value is correct
       assert(curStatusN == (int)weight[curStatusN].status );   //check if weight table in correct order
       summ_weight+= weight[curStatusN].weight;    //find a summ of the weights for the cell
    }
    return summ_weight;
}

Cell_info Gboard::get_cell_info(int posX, int posY)
{
    return board.cell [posX][posY];
}

StatusCell Gboard::calc_dir_weight(Player who, int cX, int cY, int dirX, int dirY) const
{
    const StatusCell clos_stat_arr[] = {StatusCell::none, StatusCell::s1c, //array of ststus for closed lines
           StatusCell::s2c, StatusCell::s3c};
    const StatusCell open_stat_arr[] = {StatusCell::none, StatusCell::s1o, //array of status for opened lines
           StatusCell::s2o, StatusCell::s3o };
    int cou = 0; //counter
    int cou2 = 0;
    bool fclose = false, fclose2 = false;   //flags of closed sequence
    int cX2 =  cX,  cY2 = cY;   //x, y coordinates
    Player pl;
    while (true)
    {
        cX += dirX;
        cY += dirY;

        if (!chk_boundaries(cX) || !chk_boundaries(cY))
        {
            fclose = true;  //the direction is closed
            break;
        }
        pl = board.cell[cX][cY].pl;
        if (pl == Player::None) break;
        else if (pl != who)
        {
            fclose = true;
            break;
        }
        cou++;
    }
    while (true)
    {
        cX2 -= dirX;
        cY2 -= dirY;

        if (!chk_boundaries(cX2) || !chk_boundaries(cY2))
        {
            fclose = true;  //the direction is closed
            break;
        }
        pl = (Player)board.cell[cX2][cY2].pl;
        if (pl == Player::None) break;
        else if (pl != who)
        {
            fclose2 = true;
            break;
        }
        cou2++;
    }
    int sum = cou + cou2;   //summary counter


    if (!sum) return StatusCell::none; //just open cell
    else if ((fclose && fclose2) && sum < 4) return StatusCell::none;    //closed position
    else if (cou + cou2 >=4) return StatusCell::s4;  //4 in line. next will be winning move

    int size_can_be = sum;  //check if size can grow up to 4
    if (!fclose)  size_can_be++;    //we already moved to extra position
    if (!fclose2) size_can_be++;    //we already moved to extra position
    bool fclosed = fclose | fclose2;    //fag closed if any side already is closed
    while (size_can_be < 4 && (!fclose || !fclose2))    //up t 4 positions
    {
        if (!fclose)    //check if can grow in firs directio
        {
            cX += dirX;
            cY += dirY;

            if (!chk_boundaries(cX) || !chk_boundaries(cY))
            {
                fclose = true;  //the direction is closed
            }
            pl = (Player)board.cell[cX][cY].pl;
            if ((pl == Player::None || pl == who) && !fclose) size_can_be++;
            else fclose = true;
        }
        if (!fclose2)   //check if can grow in second directio
        {
            cX2 -= dirX;
            cY2 -= dirY;

            if (!chk_boundaries(cX2) || !chk_boundaries(cY2))
            {
                fclose2 = true;  //the direction is closed
            }
            pl = (Player)board.cell[cX2][cY2].pl;
            if ((pl == Player::None || pl == who) && !fclose2) size_can_be++;   //cpmpare if the same player
            else fclose2 = true;
        }
    }
    assert(sum >=1 && sum <=3); //sum should be 1<= sum <=3. Check it
    if (size_can_be < 4) return StatusCell::none;   //here impossible to place 5 player's symbols in the line should
    else if (fclosed) return clos_stat_arr[sum];  //closed lines
    else return open_stat_arr[sum]; //opened line

}

void Gboard::calc_weight_arround(Player player, int cX, int cY, StatusCell* status_arr)   //calculate the area arround the cell. The player made the move. Revalculate the area arround
{
    //Player player = (Player)board.cell[cX][cY].pl; //who is the current player ?
    Player player2 = (player == Player::O) ? Player::X : Player::O;  //who is opponent?
    minPosX = (cX < minPosX)? cX: minPosX;  //find minimum of X position
    minPosY = (cY < minPosY)? cY: minPosY;  //find minimum of X position

    maxPosX = (cX > maxPosX)? cX: maxPosX;  //find minimum of X position
    maxPosY = (cY > maxPosY)? cY: maxPosY;  //find minimum of X position

    board.cell[cX][cY].pl_weight = 0;  //reset weight when the cell is used
    board.cell[cX][cY].op_weight = 0;

//    if (cX > maxPosX) maxPosX = cX; //maximum x Posion
//    if (cY > maxPosY) maxPosY = cY; //maximum x Posion

    int cX2, cY2;
    int i,j ;
    for (i=0; i < 8; ++i)    //recalculate weights for 8 directions
    {
        cX2 = cX;
        cY2 = cY;
        for (j =0; j <5; ++j)   //maximum depth for recalculate is 5 steps
        {
           cX2 += dir_arround[i].x;
           cY2 += dir_arround[i].y;
           if (!chk_boundaries(cX2) || !chk_boundaries(cY2)) break;
           if (board.cell[cX2][cY2].pl == Player::None)    //if the cell isn't used - caculate teh new weight
           {
               board.cell[cX2][cY2].pl_weight = calc_one_cell_weight(player, cX2, cY2, board.cell[cX2][cY2].pl_stat_arr);  //calculate the new weight
               break;
           }
           else if (board.cell[cX2][cY2].pl != player) break;    //another player - break

        }
    }

    for (i=0; i < 8; ++i)    //recalculate weights for 8 directions for oponent
    {
        cX2 = cX;
        cY2 = cY;
        for (j =0; j <5; ++j)
        {
           cX2 += dir_arround[i].x;
           cY2 += dir_arround[i].y;
           if (!chk_boundaries(cX) || !chk_boundaries(cY)) break;

           if (board.cell[cX2][cY2].pl == Player::None)    //if the cell isn't used - caculate teh new weight
           {
               board.cell[cX2][cY2].op_weight = calc_one_cell_weight(player2, cX2, cY2, board.cell[cX2][cY2].op_stat_arr);  //calculate the new weight
               break;
           }
           else if (board.cell[cX2][cY2].pl != player2) break;    //another player - break

        }
    }
}

MaxWeightInfo     Gboard::find_max_weights()
{

//    MaxValue maxV_player[8];    //array of maximum value for player
//    MaxValue maxV_opponent[8];    //array of maximum value for opponent
//    MaxValue MaxV_diff[8];  //difference between player and opponent

    MaxWeightInfo max_wei_inf;
    max_wei_inf.weight_sum = 0;

    int startX = find_start_coorinates(minPosX);   //find minimum coordinates
    int startY = find_start_coorinates(minPosY);   //find minimum
    int endX = find_end_coordinates(maxPosX);   //find maximum coordinates
    int endY = find_end_coordinates(maxPosY);   //

    int i, j;
    int adr_Vpl = 0;   //adress in maxValue arrays
    int adr_Vop = 0;
    int adr_Vsum = 0;

    int max_Vpl = 0;   //initial maximin  in maxValue arrays
    int max_Vop = 0;
    int max_Vsum = 0;
    bool fwrite;
    int tmpPlayer, tmpOpponent; //te
    for (i = startX; i <= endX; i++)
    {
        for (j=startY; j <= endY; ++j)
        {
            if (board.cell[i][j].pl == Player::None)
            {
                tmpPlayer = board.cell[i][j].pl_weight;
                tmpOpponent = board.cell[i][j].op_weight;
                fwrite =  false;
                if (tmpPlayer > max_Vpl)    //if weight is > than maximum
                {
                    max_Vpl = tmpPlayer;
                    adr_Vpl = 0;
                    fwrite = true;

                }
                else if (tmpPlayer &&  tmpPlayer == max_Vpl && adr_Vpl < val_arr_size)
                {
                    fwrite = true;
                }
                if (fwrite)
                {
                    maxV_player[adr_Vpl].weightPlayer  = tmpPlayer;
                    maxV_player[adr_Vpl].weightOpponent = tmpOpponent;
                    maxV_player[adr_Vpl].posX = i;
                    maxV_player[adr_Vpl++].posY = j;
                }

                fwrite =  false;
                if (tmpOpponent && tmpOpponent > max_Vop)    //if weight is > than maximum
                {
                    max_Vop = tmpOpponent;
                    adr_Vop = 0;
                    fwrite = true;

                }
                else if (tmpOpponent == max_Vop && adr_Vop < val_arr_size)
                {
                    fwrite = true;
                }
                if (fwrite)
                {
                    maxV_opponent[adr_Vop].weightPlayer = tmpPlayer;
                    maxV_opponent[adr_Vop].weightOpponent = tmpOpponent;   //pay attemtion player and opponent are swapped
                    maxV_opponent[adr_Vop].posX = i;
                    maxV_opponent[adr_Vop++].posY = j;
                }

            }
        }
    }

    int tmpSum, t;

    if (max_Vpl >= weight[(int)StatusCell::s4].weight)   //check the win situation of 4 for the player
    {
        copy_max_wei(max_wei_inf, maxV_player[0]);
    }
    else if (max_Vop >= weight[(int)StatusCell::s4].weight)   //check the win situation of 4 for the player
    {
        copy_max_wei(max_wei_inf, maxV_opponent[0]);
    }

    else if (max_Vpl >= weight[(int)StatusCell::s3o].weight)   //3 open
    {
        gen_rnd_rez(max_wei_inf,maxV_player, adr_Vpl);
    }

    else if (max_Vop >= weight[(int)StatusCell::s3o].weight)   //3 open
    {
        gen_rnd_rez(max_wei_inf,maxV_opponent, adr_Vop);

    }

    else if ((max_Vpl >= 2 * weight[(int)StatusCell::s3c].weight) &&
                (max_Vop >= 2 * weight[(int)StatusCell::s3c].weight))   //ckeck if both players have situation 2* 2o/3c situation
    {
        if (find_both_3c( max_wei_inf, startX,startY,endX, endY)) return  max_wei_inf;
//        if (chk_status_3c(max_wei_inf, maxV_player, adr_Vpl, true)) return max_wei_inf;
//        if (chk_status_3c(max_wei_inf, maxV_opponent, adr_Vpl, false)) return max_wei_inf;
        gen_rnd_rez(max_wei_inf, maxV_player, adr_Vpl); //if here isn't any situation with 3c - use maximum of player

    }

    else if (max_Vpl >= 2 * weight[(int)StatusCell::s3c].weight)
    {
        gen_rnd_rez(max_wei_inf, maxV_player, adr_Vpl);
    }
    else if (max_Vop >= 2 * weight[(int)StatusCell::s3c].weight)   //3 open
    {
        gen_rnd_rez(max_wei_inf, maxV_opponent, adr_Vop);
    }

    else //find maximum weight of the player and the opponent
    {
        for (i = 0; i< adr_Vpl; i++)    //process table for the layer
        {
            find_max_summ(maxV_player[i], maxV_summ, adr_Vsum, max_Vsum);
        }
        for (i = 0; i< adr_Vop; i++)    //process table for the layer
        {
            find_max_summ(maxV_opponent[i], maxV_summ, adr_Vsum, max_Vsum);
        }
        gen_rnd_rez(max_wei_inf, maxV_summ, adr_Vsum, max_Vsum);
    }
    if (!max_Vpl && !max_Vop)   //im maximums = 0 (there isn't any possibility to further move)
    {
        max_wei_inf.x = -1;
        max_wei_inf.y = -1;

    }
    return max_wei_inf; //return coordinates
}

void Gboard::copy_max_wei(MaxWeightInfo &maxWinfo, MaxValue &maxV, uint32_t max_sum)   //copy max weight
{
    maxWinfo.x = maxV.posX;
    maxWinfo.y = maxV.posY;
    maxWinfo.weightPlayer = maxV.weightPlayer;
    maxWinfo.weightOpponent = maxV.weightOpponent;
    maxWinfo.weight_sum = max_sum;

}

void Gboard::copy_max_wei(MaxWeightInfo &maxWinfo, int posX, int posY, Cell_info * cell_info, uint32_t max_sum)
{
        maxWinfo.x = posX;
        maxWinfo.y = posY;
        maxWinfo.weightPlayer = cell_info->pl_weight;
        maxWinfo.weightOpponent = cell_info->op_weight;
        maxWinfo.weight_sum = max_sum;
}

void Gboard::copy_max_wei(MaxWeightInfo &maxWinfo, Coord & coord , Cell_info & cell_info, uint32_t max_sum)
{
        maxWinfo.x = coord.x;
        maxWinfo.y = coord.y;
        maxWinfo.weightPlayer = cell_info.pl_weight;
        maxWinfo.weightOpponent = cell_info.op_weight;
        maxWinfo.weight_sum = max_sum;
}



int Gboard::recalc_pl_weight(int player_weight) //recalculate player weight for calculation sum of player and opponents weight
{
    return 3 * player_weight / 2;  //recalculation *3 / 2
}

void Gboard::find_max_summ(MaxValue &maxVarr, MaxValue *maxV_summ_arr, int &adr_Vsum, int &max_Vsum) //function for calculation
{
    int tmpPlayer= maxVarr.weightPlayer;
    int tmpOpponent = maxVarr.weightOpponent;
    bool fwrite = false;
    int tmpSum = recalc_pl_weight(tmpPlayer) + tmpOpponent; //calculate summary weight
    if (tmpSum > max_Vsum)
    {
        max_Vsum = tmpSum;
        adr_Vsum = 0;
        fwrite = true;
        adr_Vsum = 0;
    }
    else if (tmpSum && tmpSum == max_Vsum && adr_Vsum < val_arr_size)
    {
            fwrite = true;
    }
    if (fwrite)
    {
        maxV_summ_arr[adr_Vsum].weightPlayer  = tmpSum;
        maxV_summ_arr[adr_Vsum].weightOpponent = -1;
        maxV_summ_arr[adr_Vsum].posX = maxVarr.posX;
        maxV_summ_arr[adr_Vsum++].posY = maxVarr.posY;
    }
}

int Gboard::find_start_coorinates(int min_pos)
{
    return (min_pos - 1 > 0)? min_pos - 1: 0;   //find minimum
}

int Gboard::find_end_coordinates(int max_pos)
{
    return (max_pos + 1 < board_size )? max_pos + 1 : board_size - 1;   //find minimum
}


void Gboard::gen_rnd_rez(MaxWeightInfo &max_wei_inf, MaxValue *maxVarr, int arr_length, uint32_t max_sum ) //get a random number if there are few results with the same weight
{
    assert (arr_length>0);
    int n = 0;  //if only 1 member
    if (arr_length !=1)
    {
        n = getRnd(0, arr_length -1);
    }
    assert(n < arr_length); //check the size
    copy_max_wei(max_wei_inf, maxVarr[n], max_sum);
}

bool Gboard::chek_win_line(Player who, int cX, int cY, int dirX, int dirY)
{
    int cou = 0; //counter
    int tmpX, tmpY, cX2 =  cX,  cY2 = cY;   //x, y coordinates

    Player pl;

    tmpX = cX;
    tmpY = cY;

    while (true)
    {
        tmpX += dirX;
        tmpY += dirY;

        if (!chk_boundaries(cX) || !chk_boundaries(cY))
        {
            break;
        }
        pl = board.cell[tmpX][tmpY].pl;
        if (pl == who)
        {
            cou++;
            cX = tmpX;
            cY = tmpY;
        }
        else break;
    }
    tmpX = cX2;
    tmpY = cY2;
    while (true)
    {
        tmpX -= dirX;
        tmpY -= dirY;

        if (!chk_boundaries(cX) || !chk_boundaries(cY))
        {
            break;
        }
        pl = board.cell[tmpX][tmpY].pl;
        if (pl == who)
        {
            cou++;
            cX2 = tmpX;
            cY2 = tmpY;
        }
        else break;
    }
    if (cou >=4)    //4 symbols in line + current line
    {
        win_info.pl_win =  who;
        win_info.x_start = cX ;    //find minimum
        win_info.y_start = cY ;
        win_info.x_end  =  cX2;    //find maximum
        win_info.y_end  =  cY2;
        return true;    //find the win difrection
    }
    return false;
}

bool Gboard::chk_status_3c(MaxWeightInfo &max_wei_inf, MaxValue *maxVarr, int arr_length, bool fPlayer) //check for 3c status
{
     assert (arr_length>0);
    for (int i = 0; i < arr_length; i++)
    {
       if (check_2o3c(maxVarr[i].posX, maxVarr[i].posY, fPlayer ))    //check if there is 3c situaion
       {
               copy_max_wei(max_wei_inf, maxVarr[i],999);  //999 -flag is a fork with at least 3c cell
               return true;
        }
    }
    return false;
}

bool Gboard::check_2o3c(int x, int y, bool fPlayer, bool fChk_both)
{
    StatusCell * cell_inf;   //array for cell info
    if (!fPlayer)    //If player of opponent
    {
        cell_inf = board.cell[x][y].op_stat_arr;    //array ok player
    }
    else cell_inf = board.cell[x][y].pl_stat_arr;
    for (int i =0; i <4; i++)
    {
        if (!fChk_both)
        {
            if (cell_inf[i] == StatusCell::s3c) return true;
        }
        else if (cell_inf[i] == StatusCell::s3c || cell_inf[i] == StatusCell::s2o) return true;

    }
    return false;
}

bool Gboard::find_both_3c(MaxWeightInfo &max_wei_inf, int startX, int startY, int endX, int endY)
{
    int x, y, i;
    Cell_info *   cell_info;   //temporary
    StatusCell * status_arr;
    bool fFound = false;    //if founder result for opponent
    for (x = startX; x <= endX; ++x)
    {
        for (y = startY; y <= endY; ++y)
        {
            cell_info = &board.cell[x][y];
            if(cell_info->pl == Player::None )  //check only emplty cell
            {
                if (cell_info->pl_weight >= 2 * weight[(int)StatusCell::s3c].weight)
                {
                    status_arr = cell_info ->pl_stat_arr;   //check the player
                    for (i =0; i <4; ++i)
                    {
                        if(status_arr[i] == StatusCell::s3c)    // if player has 3c configuration - max_wei_inf and return
                        {
                            copy_max_wei(max_wei_inf, x, y, cell_info, (uint32_t)WeiCalcFl::f3c_player);
                            return true;    //if the player has 3c - exit emidietelly
                        }
                    }
                }
                if (cell_info->op_weight >= 2 * weight[(int)StatusCell::s3c].weight)
                {
                    status_arr = cell_info ->op_stat_arr;   //check the player
                    for (i =0; i <4; ++i)
                    {
                        if(status_arr[i] == StatusCell::s3c)    // if player has 3c configuration - max_wei_inf and return
                        {
                            copy_max_wei(max_wei_inf, x, y, cell_info, (uint32_t)WeiCalcFl::f3c_opponent);
                            fFound = true;  //don't exit out of the function. Check further the player
                        }
                    }
                }

            }
        }
    }
    return fFound;  //exit with
}

bool Gboard::check_foreview(MaxWeightInfo &max_wei_inf, int startX, int startY, int endX, int endY)
{
    const int size_arr = 32;
    Coord pl2o3c_arr[size_arr]; //array of 2o or 3c configuration for player
    Coord op2o3c_arr[size_arr]; //array of 2o or 3c configuration for opponent //summary size is for player and opponent is 128 byte
    int cou_pl = 0;
    int cou_op = 0;
    int x, y;
    bool fFound;    //flag if found foreview fork

    for ( x = startX; x < startX; x++)
    {
        for (y = startY; y < startY; y++)
        {
            if ((board.cell[x][y].pl_weight >= weight[(int)StatusCell::s3c].weight)  && cou_pl < size_arr)  //check weight for 3c and >
            {
                if (check_2o3c(x,y, true, true)) //check 2o or 3c
                {
                     pl2o3c_arr[cou_pl].x = x;
                     pl2o3c_arr[cou_pl++].y = y;
                }
            }
            if ((board.cell[x][y].op_weight >= weight[(int)StatusCell::s3c].weight)  && cou_op < size_arr)  //check weight for 3c and >
            {
                if (check_2o3c(x,y, false, true)) //check 2o or 3c  - for opponent
                {
                     op2o3c_arr[cou_op].x = x;
                     op2o3c_arr[cou_op++].y = y;
                }
            }
        }
    }
    bool f3c = false;   //flag if will found situation with 3c sttus
    int i; int j;
    for (i = 0; i <  cou_pl - 1; i++)
    {
        for (j = i; i < cou_pl ; j++)
        {
           Dist12 fDist = chk_dist(pl2o3c_arr[i], pl2o3c_arr[j]);   //check distance
           if (fDist == Dist12::both_no) break;     //distances are > 2 for bothc oordinates
           if (fDist == Dist12::yes)    //distances to cell with 2o, 3c status are < 2 for bothc oordinates
           {
                if (chk_foreview2(max_wei_inf, pl2o3c_arr[i], pl2o3c_arr[j], true /* fPl*/, fFound)) return true;
           }

        }
    }
    return false;
}

Gboard::Dist12 Gboard::chk_dist(Coord &ref, Coord &target)
{
    int distX =  std::abs(ref.x - target.x);
    int distY =  std::abs(ref.y - target.y);
    if (distX > 2 && distY > 2) return Dist12::both_no;
    if (distX <= 2 && distY <= 2) return Dist12::yes;
    return Dist12::no;

}

bool Gboard::chk_foreview2(MaxWeightInfo &max_wei_inf, Coord & ref, Coord & target, bool fPl, bool &fFound) //, bool & f3c)
{
    int i, j;
    Player who;
    if (fPl) who = the_player;  //whom is check now

    else who = the_opponent;
    StatusCell * status_arr_ref;
    StatusCell * status_arr_target;
    bool fFoundRef= false, fFoundTarg= false;
    bool f3c_local = false;   //local flag 3c
    bool fFound_local = false;   //local flag 3c
    if (fPl)
    {
        status_arr_ref = board.cell[ref.x][ref.y].pl_stat_arr;  //arrays for of referemce and target cells
        status_arr_target = board.cell[target.x][target.y].pl_stat_arr;
    }
    else
    {
        status_arr_ref = board.cell[ref.x][ref.y].op_stat_arr;
        status_arr_target = board.cell[target.x][target.y].op_stat_arr;
    }
    for (i =0; i < 4; i++ )
    {
        if (status_arr_ref[i] >= StatusCell::s3c)
        {
            if (status_arr_ref[i] == StatusCell::s3c) f3c_local = true; // here is local flag 3c
            fFoundRef = true;
            break;
        }
    }
    for (j =0; j < 4; j++ )
    {
        if (status_arr_target[j] >= StatusCell::s3c)
        {
            if (status_arr_ref[i] == StatusCell::s3c) f3c_local = true; // here is local flag 3c
            fFoundTarg = true;
            break;
        }
    }
    assert(fFoundRef && fFoundTarg);    //for debug aims. Status  3c or > must have each cell
    Coord ref_cop, targ_cop ;   //copy of coordinates
    ref_cop = ref;
    targ_cop = target;
    //ref in corren position. Check target forward and back
    if (go_near(who, targ_cop, dir_arr[j].x, dir_arr[j].y))
    {
        if (write_foreview_info(max_wei_inf, ref_cop, targ_cop, fFound_local, fFound, f3c_local)) return true;       //3c foreview fork was found
        if (fFound_local)
        {
            return false;   //found 2o foreview fork
        }
    }
    targ_cop = target;
    if (go_near(who, targ_cop, -dir_arr[j].x, -dir_arr[j].y))
    {
        if (write_foreview_info(max_wei_inf, ref_cop, targ_cop, fFound_local, fFound, f3c_local)) return true;       //3c foreview fork was found
        if (fFound_local)
        {
            return false;   //found 2o foreview fork
        }
    }

    //ref goes forward. Check target in the current position, forward and back
    targ_cop = target;
    if (go_near(who, ref_cop, dir_arr[j].x, dir_arr[j].y))
    {
        if (write_foreview_info(max_wei_inf, ref_cop, targ_cop, fFound_local, fFound, f3c_local)) return true;       //3c foreview fork was found
        if (fFound_local)
        {
            return false;   //found 2o foreview fork
        }
    }
    if (go_near(who, targ_cop, dir_arr[j].x, dir_arr[j].y)) //taret foeward
    {
        if (write_foreview_info(max_wei_inf, ref_cop, targ_cop, fFound_local,  fFound, f3c_local)) return true;       //3c foreview fork was found
        if (fFound_local)
        {
            return false;   //found 2o foreview fork
        }
    }
    targ_cop = target;
    if (go_near(who, targ_cop, -dir_arr[j].x, -dir_arr[j].y))
    {
        if (write_foreview_info(max_wei_inf, ref_cop, targ_cop, fFound_local, fFound, f3c_local)) return true;       //3c foreview fork was found
        if (fFound_local)
        {
            return false;   //found 2o foreview fork
        }
    }

    //ref goes forward. Check target in the current position, forward and back
    ref_cop = ref;
    targ_cop = target;
    if (go_near(who, ref_cop, -dir_arr[j].x, -dir_arr[j].y))
    {
        if (write_foreview_info(max_wei_inf, ref_cop, targ_cop, fFound_local, fFound, f3c_local)) return true;       //3c foreview fork was found
        if (fFound_local)
        {
            return false;   //found 2o foreview fork
        }
    }
    if (go_near(who, targ_cop, dir_arr[j].x, dir_arr[j].y))
    {
        if (write_foreview_info(max_wei_inf, ref_cop, targ_cop, fFound_local, fFound, f3c_local)) return true;       //3c foreview fork was found
        if (fFound_local)
        {
            fFound = true;  //here is located foreview fork
            return false;   //found 2o foreview fork
        }
    }
    targ_cop = target;
    if (go_near(who, targ_cop, -dir_arr[j].x, -dir_arr[j].y))
    {
        if (write_foreview_info(max_wei_inf, ref_cop, targ_cop, fFound_local, fFound, f3c_local)) return true;       //3c foreview fork was found
        if (fFound_local)
        {
            fFound = true;  //here is located foreview fork
            return false;   //found 2o foreview fork
        }
    }

    return false;

}



bool Gboard::go_near(Player who, Coord &coord, int dir_x, int dir_y)
{
   Player whois;    //whose move is located in the current cell
   for (int i = 0; i < 3; i++)
   {
        coord.x += dir_x;
        coord.y += dir_y;   //to move in direction
        if (!(chk_boundaries(coord.x) && chk_boundaries(coord.y))) return false;    //check boundaries
        {
            whois = board.cell[coord.x][coord.y].pl;
            if (whois == Player::None) return true;  //the cell is free
            if (whois != who) return false; //anothe player - return
        }
    }
   return false;   //return if wasn't find any free cell in this direction
}

bool Gboard::write_foreview_info(MaxWeightInfo &max_wei_inf, Coord& ref, Coord& targ, bool & fFound_local, bool &fFound, /*bool &f3c,*/ bool f3c_local)
{
    if (ref == targ)
    {
        if (f3c_local)  // if here is 3c line
        {
            //f3c =  true;
            copy_max_wei(max_wei_inf, ref, board.cell[ref.x][ref.y], (int)WeiCalcFl::f3c_foreview);    //copy information
            fFound = true;
            return true;
        }
        fFound_local = true;
        if (fFound) return false;   //already was found foreview fork - don't copy information
        copy_max_wei(max_wei_inf, ref, board.cell[ref.x][ref.y], (int)WeiCalcFl::f2o_foreview);    //copy information
        fFound = true;
    }
    return false;

}

uint32_t Gboard::getRnd(uint32_t start, uint32_t end)   //get randon number
{
    std::uniform_int_distribution<int> range_distribution(start, end);
    uint64_t time = uint64_t(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    std::mt19937 random_number_engine((uint32_t)(time ^ (time >> 32)) ); // pseudorandom number generator
    auto get_number = std::bind(range_distribution, random_number_engine);
    return get_number();
}

bool Gboard::set_move(Player who, int posX, int posY) //set a move for the player
{
    board.cell[posX][posY].pl = who;
    if (chek_win_situation(posX, posY)) return true;    //check win situation
    board.cell[posX][posY].pl_weight =0;
    board.cell[posX][posY].op_weight = 0; //reset  weightf for player and opponent
    calc_weight_arround(the_player, posX, posY);    //cal weight arround
    return false;
}
/*
 *check the win situation
 *input - coordinares of the cell
 *output true if founf 5 cell in the
*/
bool Gboard::chek_win_situation(int posX, int posY)    //check the win situation
{
   Cell_info & the_cell = board.cell[posX][posY];
   StatusCell * status_arr = nullptr; //status arr with weight for 4 direction
    Player who = the_cell.pl;
   if (who == the_player)
   {
       if (the_cell.pl_weight < weight[(int)StatusCell::s4].weight) return false;
       status_arr =  the_cell.pl_stat_arr;  //copy status array pointer
   }
   else if (who == the_opponent)
   {
       if (the_cell.op_weight < weight[(int)StatusCell::s4].weight) return false;
       status_arr =  the_cell.op_stat_arr;  //copy status array pointer for 4 direction
   }
   else assert(false); //wrong. players must be O or X
   bool fFound = false;
   int i;
   for (i=0; i<4; ++i)
   {
       if (status_arr[i] == StatusCell::s4)
       {
            if (chek_win_line(who, posX, posY, dir_arr[i].x, dir_arr[i].y))
            {
                fFound =true;
                break;
            }
       }
   }
   assert(fFound);  //
   fgame_on = false;    //game is finished
   return true;
}

void Gboard::set_player(Player who)     //set the player
{
    the_player = who;
    the_opponent = (the_player == Player::O)? Player::X : Player ::O ;
}

Player Gboard::get_player() //get the player value
{
    return the_player;
}
Player Gboard::get_opponet()    //get the player value
{
    return the_opponent;
}

Player Gboard::check_cell(int x, int y) //check board cell
{
    return board.cell[x][y].pl;
}


