#include "mainwi.h"
#include "./ui_mainw.h"
//#include "common_h.h"
#include <qstring.h>

MainW::MainW(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainW)
{
    ui->setupUi(this);
    scene = new XOScene(ui->XOGrView);   //0, 0, 490, 490, ui->XOGrView);
    scene -> setSceneRect(0, 0, XOScene::edge + XOScene::line_width, XOScene::edge + XOScene::line_width);
    ui->XOGrView->setScene(scene);
//    connect(scene, SIGNAL(sc_mouse_pos()), this, SLOT(mouse_curr_pos()));
    connect(scene, SIGNAL(sc_mouse_pressed()), this, SLOT(mouse_pressed()));

    gboard = std::make_unique<Gboard>();

    on_startButton_pressed();   //initialisation
//    gboard ->init();
//    gboard->set_player(Player::O);

}

MainW::~MainW()
{
    scene->clear();
    delete scene;
    delete ui;
}

//void MainW::mouse_curr_pos()
//{
//    int x = scene->get_x();
//    int y = scene->get_y();
//    ui->label->setText("Mouse move -> x = " + QString::number(x) + ", y = " + QString::number(y));
//}

void MainW::mouse_pressed()
{

    int x = scene->get_x();
    int y = scene->get_y();
    ui->label->setText("Mouse pressed -> x = " + QString::number(x) + ", y = " + QString::number(y));
    QString str = "cell is -> x = " + QString::number(scene->get_xcell()) + ", y = " + QString::number(scene->get_ycell());

    int posX = scene->get_xcell();
    int posY =  scene->get_ycell();

    if (scene ->mouse_but() == Qt::LeftButton)
    {
        if (gboard -> check_cell(posX, posY) != Player::None) return;  //check board cell)

//        gboard ->set_move(gboard ->get_opponet(), posX, posY);
//        scene-> draw_player(gboard ->get_opponet(), posX, posY );   //draw O
        gboard ->set_move(pl_move, posX, posY);
        scene-> draw_player(pl_move, posX, posY );   //draw O

        pl_move = (pl_move == Player::X)? Player::O: Player::X;
        ui->label_2->setText(str);
        wite_move(pl_move);    //show whose turn is
    }
    else if (scene ->mouse_but() == Qt::RightButton)        //show the debug information
    {
        StatusCell status_arr[4];
        int sum_weight = gboard -> calc_one_cell_weight(gboard ->get_opponet(), posX, posY, status_arr);  //calculate the new weight
        str = str + "\n Oppon = ";
        int i;
        for (i =0; i < 4; ++i)
        {
            assert(i == (int)gboard-> weight_name[i].status );   //check if weight table in correct order
            int numStatus = (int)status_arr[i];
            str = str + gboard ->weight_name[numStatus].name + ", ";
        }
        str = str + " weight " + QString::number(sum_weight);

        //calculation for player
        sum_weight = gboard -> calc_one_cell_weight(gboard ->get_player(), posX, posY, status_arr);  //calculate the new weight
        str = str + "\n Player = ";
        for (i =0; i < 4; ++i)
        {
            assert(i == (int)gboard-> weight_name[i].status );   //check if weight table in correct order
            int numStatus = (int)status_arr[i];
            str = str + gboard ->weight_name[numStatus].name + ", ";
        }
        str = str + " weight " + QString::number(sum_weight);
        str = str + "\n Sign = ";
         Cell_info cell_inf = gboard -> get_cell_info(posX, posY);
         switch(cell_inf.pl)
         {
         case Player::None :
                str += "none, ";
                break;
         case Player::X:
             str += "X, ";
             break;
         case Player::O:
             str += "X, ";
             break;
         }
         str += "calc weight = pl =" + QString::number(cell_inf.weight_player);
         str += ", opp =" + QString::number(cell_inf.weight_opponent);
         ui->label_2->setText(str);
    }
}

void MainW::on_startButton_pressed()
{
    scene->clear();
    scene->drawCells();
    gboard->init();
    gboard->set_player(Player::O);
    pl_move = Player::X;  //who should move now
    //ui->but_comp->setEnabled(false);
    wite_move(pl_move);    //show whose turn is
}

void MainW::on_but_comp_pressed()
{

}

void MainW::wite_move(Player pl)    //show whose turn is
{
    if (pl == Player::X)
    {
        ui->lab_turn->setText("Netxt turn: X");
        ui->but_comp->setEnabled(false);
    }
    else
    {
        ui->lab_turn->setText("Netxt turn: O");
        ui->but_comp->setEnabled(true);
    }
}
