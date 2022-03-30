#include "gboard.h"

Gboard::Gboard()
{
    board = std::make_unique<board_>();
}

Gboard::Gboard(Player player): the_player(player)
{
    Gboard();
}


void Gboard::init()
{
    int i;
    for (i=0; i< sizeof(board_)/sizeof(board->line[0]); ++i) board->line[i] =0;
    minPosY = minPosX = board_size -1;
    maxPosX = maxPosY = 0;

}


bool Gboard::chk_boundaries(int pos) const    //check the boundaries
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
    return board ->cell [posX][posY];
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
        pl = board->cell[cX][cY].pl;
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
        pl = (Player)board->cell[cX2][cY2].pl;
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
            pl = (Player)board->cell[cX][cY].pl;
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
            pl = (Player)board->cell[cX2][cY2].pl;
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
    //Player player = (Player)board->cell[cX][cY].pl; //who is the current player ?
    Player player2 = (player == Player::O) ? Player::X : Player::O;  //who is opponent?
    minPosX = (cX < minPosX)? cX: minPosX;  //find minimum of X position
    minPosY = (cY < minPosY)? cY: minPosY;  //find minimum of X position

    maxPosX = (cX > maxPosX)? cX: maxPosX;  //find minimum of X position
    maxPosY = (cY > maxPosY)? cY: maxPosY;  //find minimum of X position

    board ->cell[cX][cY].weight_player = 0;  //reset weight when the cell is used
    board ->cell[cX][cY].weight_opponent = 0;

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
           if (board ->cell[cX2][cY2].pl == Player::None)    //if the cell isn't used - caculate teh new weight
           {
               board ->cell[cX2][cY2].weight_player = calc_one_cell_weight(player, cX2, cY2);  //calculate the new weight
               break;
           }
           else if (board ->cell[cX2][cY2].pl != player) break;    //another player - break

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

           if (board ->cell[cX2][cY2].pl == Player::None)    //if the cell isn't used - caculate teh new weight
           {
               board ->cell[cX2][cY2].weight_opponent = calc_one_cell_weight(player2, cX2, cY2);  //calculate the new weight
               break;
           }
           else if (board ->cell[cX2][cY2].pl != player2) break;    //another player - break

        }
    }
}

Gboard::BoardCoord Gboard::find_max_wights()
{

//    MaxValue maxV_player[8];    //array of maximum value for player
//    MaxValue maxV_opponent[8];    //array of maximum value for opponent
//    MaxValue MaxV_diff[8];  //difference between player and opponent

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
            if (board ->cell[i][j].pl == Player::None)
            {
                tmpPlayer = board ->cell[i][j].weight_player;
                tmpOpponent = board ->cell[i][j].weight_opponent;
                fwrite =  false;
                if (tmpPlayer > max_Vpl)    //if weight is > than maximum
                {
                    max_Vpl = tmpPlayer;
                    adr_Vpl = 0;
                    fwrite = true;

                }
                else if (tmpPlayer == max_Vpl && adr_Vpl < val_arr_size)
                {
                    fwrite = true;
                }
                if (fwrite)
                {
                    maxV_player[adr_Vpl].weightPlayer  = tmpPlayer;
                    maxV_player[adr_Vpl].weightOpponent = tmpOpponent;
                    maxV_player[adr_Vpl].posX = i;
                    maxV_player[adr_Vpl++].posX = j;
                }

                fwrite =  false;
                if (tmpOpponent > max_Vop)    //if weight is > than maximum
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
                    maxV_opponent[adr_Vop++].posX = j;
                }

            }
        }
    }
    BoardCoord b_coord;
    int tmpSum, t;

    if (max_Vpl >= weight[(int)StatusCell::s4].weight)   //check the win situation of 4 for the player
    {
        b_coord.x = maxV_player[0].posX;
        b_coord.y = maxV_player[0].posY;

    }
    else if (max_Vop >= weight[(int)StatusCell::s4].weight)   //check the win situation of 4 for the player
    {
        b_coord.x = maxV_opponent[0].posX;
        b_coord.y = maxV_opponent[0].posY;
    }

    else if (max_Vpl >= weight[(int)StatusCell::s3o].weight)   //3 open
    {
        b_coord = gen_rnd_rez(maxV_player, max_Vpl);
    }

    else if (max_Vop >= weight[(int)StatusCell::s3o].weight)   //3 open
    {
        b_coord = gen_rnd_rez(maxV_opponent, max_Vop);

    }

    else if (max_Vpl >= 2 * weight[(int)StatusCell::s2o].weight)   //3 open
    {
        b_coord = gen_rnd_rez(maxV_player, max_Vpl);
    }

    else if (max_Vop >= 2 * weight[(int)StatusCell::s2o].weight)   //3 open
    {
        b_coord = gen_rnd_rez(maxV_opponent, max_Vop);
    }

    else //find maximum weight of the player and the opponent
    {
        for (i = 0; i< max_Vpl; i++)    //process table for the layer
        {
            find_max_summ(maxV_player[i], maxV_summ, adr_Vsum, max_Vsum);
        }
        for (i = 0; i< max_Vop; i++)    //process table for the layer
        {
            find_max_summ(maxV_opponent[i], maxV_summ, adr_Vsum, max_Vsum);
        }
        b_coord = gen_rnd_rez(maxV_summ, max_Vsum);
    }
    if (!max_Vpl && !max_Vop)   //im maximums = 0 (there isn't any possibility to move)
    {
        b_coord.x = -1;
        b_coord.y = -1;

    }
    return b_coord; //return coordinates
}

int Gboard::recalc_pl_weight(int player_weight) //recalculate player weight for calculation sum of player and opponents weight
{
    return 3 * player_weight / 2;  //recalculation *3 / 2
}

void Gboard::find_max_summ(MaxValue &maxVarr, MaxValue *maxV_summ, int &adr_Vsum, int &max_Vsum) //function for calculation
{
    int tmpPlayer= maxVarr.weightPlayer;
    int tmpOpponent = maxVarr.weightPlayer;
    bool fwrite = false;
    int tmpSum = recalc_pl_weight(tmpPlayer) + tmpOpponent;
    if (tmpSum > max_Vsum)
    {
        max_Vsum = tmpSum;
        adr_Vsum = 0;
        fwrite = true;
        adr_Vsum = 0;
    }
    else if (tmpSum == max_Vsum && adr_Vsum < val_arr_size)
    {
            fwrite = true;
    }
    if (fwrite)
    {
        maxV_summ[adr_Vsum].weightPlayer  = tmpSum;
        maxV_summ[adr_Vsum].weightOpponent = 0;
        maxV_summ[adr_Vsum].posX = maxVarr.posX;
        maxV_summ[adr_Vsum++].posX = maxVarr.posY;
    }

}


int Gboard::find_start_coorinates(int min_pos)
{
    return (min_pos - 1 > 0)? min_pos - 1: 0;   //find minimum
}

int Gboard::find_end_coordinates(int max_pos)
{
    return (max_pos + 1 < board_size )? max_pos : board_size - 1;   //find minimum
}


Gboard::BoardCoord Gboard::gen_rnd_rez(MaxValue *maxVarr, int arr_lenth) //get a random number if there are few results with the same weight
{
    int n = 0;  //if only 1 member
    if (arr_lenth !=1)
    {
        n = getRnd(0, arr_lenth -1);
    }
    assert(n < arr_lenth); //check the size
    BoardCoord b_coord;
    b_coord.x = maxVarr[n].posX;
    b_coord.y = maxVarr[n].posY;
    return b_coord;
}



uint32_t Gboard::getRnd(uint32_t start, uint32_t end)   //get randon number
{
    std::uniform_int_distribution<int> range_distribution(1, 0xff);
    uint64_t time = uint64_t(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    std::mt19937 random_number_engine((uint32_t)(time ^ (time >> 32)) ); // pseudorandom number generator
    auto get_number = std::bind(range_distribution, random_number_engine);
    return get_number();
}

void Gboard::set_move(Player who, int posX, int posY) //set a move for the player
{
    board ->cell[posX][posY].pl = who;
    board->cell[posX][posY].weight_player =0;
    board->cell[posX][posY].weight_opponent = 0; //reset  weightf for player and opponent
    calc_weight_arround(the_player, posX, posY);    //cal weight arround
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
    return board->cell[x][y].pl;
}

