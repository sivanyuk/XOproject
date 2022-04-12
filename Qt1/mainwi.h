#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include "XOScene.h"
#include "common_h.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainW; }
QT_END_NAMESPACE

class MainW : public QMainWindow
{
    Q_OBJECT

public:
    MainW(QWidget *parent = nullptr);
    ~MainW();

private:
    Ui::MainW *ui;
    XOScene * scene;
    std::unique_ptr<Gboard> gboard;
    Player pl_move;  //who should move now

private slots:
//    void mouse_curr_pos();
    void mouse_pressed();


    void on_startButton_pressed();  //restart the game
    void on_but_comp_pressed();     //make a computer's move
    void wite_move(Player pl);  //to write whose move is
    void on_pushButton_pressed();
    void on_but_calculate_pressed();
};
#endif // MAINW_H

