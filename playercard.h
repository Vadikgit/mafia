#ifndef PLAYERCARD_H
#define PLAYERCARD_H

#include <QFrame>
#include <QWidget>
#include <QGridLayout>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>
#include "mainwindow.h"

class MainWindow;

class PlayerCard:public QWidget
{
    Q_OBJECT
public:
    PlayerCard(QWidget *parent = nullptr);
    QFrame * p_frame;
    QPixmap* p_man_image;
    QGridLayout* p_grid;
    QLabel *p_label;
    QLabel *p_name;
    QLabel *p_role;

    bool is_clickable;

    MainWindow* p_Main_Window;


    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void set_you_style();
    void set_killed_style();
    void show_role(int);
};

#endif // PLAYERCARD_H
