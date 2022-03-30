#include "XOScene.h"

XOScene::XOScene(QObject *parent)
    : QGraphicsScene{parent}
{
    drawCells();

    colorO.setRgb(30, 150, 30, 255);

    colorX.setRgb(150, 40, 255, 255);
    penO = QPen(colorO, size_Oline, Qt::SolidLine, Qt::RoundCap);    //color for O sign
    penX = QPen(colorX, size_Xline, Qt::SolidLine, Qt::RoundCap);    //color for O sign

}

void XOScene::drawCells()
{
    //QGraphicsLineItem * line1 = nullptr ;
    setSceneRect(0, 0, edge + line_width, edge + line_width);

    int pos;
    QPen linePen(Qt::gray,line_width,Qt::SolidLine,Qt::RoundCap);
    int i;
    for (i= 0; i <= cells; ++i)
    {
        pos = i * (cells_width+ line_width);
        /*line1 =  */this->addLine(0, pos, edge, pos, linePen);
    }
    for (i= 0; i <= cells; ++i)
    {
        pos = i * (cells_width+ line_width);
        /*line1 =  */this->addLine(pos, 0, pos, edge, linePen);
    }
    //QGraphicsItem * item =static_cast<QGraphicsItem *>(line1);
    //scene->removeItem(line1);

    //scene->clear();

}

//calculate cell
int XOScene::calc_cell(int pos) //calculate cell in the scene
{
    int cell = pos /(cells_width + line_width);
    if (cell > cells) return -1;    //cell is out of range
    int range = pos % (cells_width + line_width);
    if (range >= min_range && range < max_range) return cell;    //cech the range inside of the cell
    return -1;
}


void XOScene::drawO(int cell_x, int cell_y)
{
    if (cell_x >= cells || cell_y >= cells) return; //check the boundaries
    int posX = calc_pos(cell_x);
    int posY = calc_pos(cell_y);
    addEllipse(QRectF(posX+startO, posY+startO, endO - startO, endO - startO), penO);
}

void XOScene::drawX(int cell_x, int cell_y)
{
    if (cell_x >= cells || cell_y >= cells) return; //check the boundaries
    int posX = calc_pos(cell_x);
    int posY = calc_pos(cell_y);
    addLine(posX+startX, posY+startX, posX + endX, posY + endX, penX);
    addLine(posX+startX, posY+endX, posX + endX, posY + startX, penX);

}

void XOScene::draw_player(Player player, int cell_x, int cell_y)    //draw player's symbol
{
    if (player == Player::O) drawO(cell_x, cell_y);
    else drawX(cell_x, cell_y);
}

void XOScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
        x = event->scenePos().rx();

        int tmp_xc = calc_cell(x);
        y = event->scenePos().ry();
        int tmp_yc = calc_cell(y);
        if (tmp_xc < 0 || tmp_yc < 0) return;   //check if the both coordinates are valid
        cellX = tmp_xc;
        cellY = tmp_yc;
        mouse_but_=  event->button();
        emit sc_mouse_pressed();

}

int XOScene::calc_pos(int cell)
{
    return cell * (cells_width + line_width) + line_width;

}


//void XOScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//{
//    x = event->scenePos().rx();
//    y = event->scenePos().ry();
//    emit sc_mouse_pos();
//}
