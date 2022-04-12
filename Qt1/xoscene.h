#ifndef XOSCENE_H
#define XOSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "gcommon.h"



class XOScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit XOScene(QObject *parent = nullptr);
    explicit XOScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = nullptr);
    ~XOScene(){};

static const int cells = 21;
static const int cells_width = 20;
static const int line_width = 2;
static const int min_range = line_width+2 ;   //minumum position in a cell where should be plased the cursor
static const int max_range = cells_width ;    //maximum position in cell a where should be plased the cursor
static const int edge  = ((cells_width + line_width ) * cells);
static const int scene_size = edge + line_width;
static const int gridV_size = scene_size + 20;
static const int size_Oline = 3;
static const int size_Xline = 3;
static const int startO =  2;
static const int startX = 2;
static const int endO = cells_width - 5;
static const int endX = cells_width - 5;
static const int start_win =  1;
static const int end_win = cells_width - 4;
static const int size_win_line = 3;



int get_x() {return x;}
int get_y() {return y;}
int get_xcell() {return cellX;}    //the cell is
int get_ycell() {return cellY;}

int calc_cell(int pos); //calculate cell in the scene


void drawO(int cell_x, int cell_y);    //draw O Symbol
void drawX(int cell_x, int cell_y);    //draw X Symbol
void draw_player(Player player, int cell_x, int cell_y);    //draw player
void draw_winner_line(WinInfo win_inf); //draw the line of the winner
Qt::MouseButton mouse_but() {return mouse_but_;}
void drawCells();   //darw celle (board)


private:
    QPointF     previousPoint;
    QColor colorO, colorX, color_win;
    int x, y;   //mouse coordinates
    int cellX, cellY;   //mouse coordinates
    Qt::MouseButton mouse_but_;

    void mousePressEvent(QGraphicsSceneMouseEvent * event); //mouse event
//    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    QPen penO; //(QColor::setRgb(10, 127, 10));    //color for O sign
    QPen penX; //color for X sign
    QPen pen_win;   //winner's pen
    inline int calc_pos(int cell); //static const int cells = 21;

signals:
    void sc_mouse_pressed();
    //void sc_mouse_pos();
};

#endif // XOSCENE_H

