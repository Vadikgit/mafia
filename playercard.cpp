#include "playercard.h"

#include <QPainter>
PlayerCard::PlayerCard(QWidget *parent)
    :QWidget(parent)
{
    setMaximumSize(100,160);
    setMinimumSize(100,160);
    p_frame = new QFrame(this);
    p_frame->setMinimumSize(this->minimumSize());
    p_frame->setMaximumSize(this->maximumSize());

    //p_frame->setStyleSheet("background-color: rgb(198, 0, 198);");

    //this->setStyleSheet(".QFrame{background-color: rgb(198, 100, 255); border: 6px solid yellow; border-radius: 20px;}");
    p_frame->setStyleSheet(".QFrame{background-color: rgb(198, 100, 255); border: 6px solid yellow; border-radius: 20px;}");
    //this->setStyle(p_frame->style());

    //p_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    p_grid = new QGridLayout(p_frame);


    p_man_image = new QPixmap(":/new/images/man.png");
    //*p_man_image = p_man_image->scaled(73,73,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    p_label = new QLabel(p_frame);
    p_name = new QLabel(p_frame);
    p_name->setStyleSheet(".QLabel{color: rgb(220, 0, 0); }");
    p_role = new QLabel(p_frame);
    p_name->setText("Player");
    //p_role->setText("Role: *playerrl*");
    p_role->setText("");

    p_label->setPixmap(*p_man_image);
    p_grid->addWidget(p_label,0,0,2,2);
    p_grid->addWidget(p_name,3,0,1,1);
    p_grid->addWidget(p_role,4,0,1,1);

    is_clickable = true;

}

void PlayerCard::mousePressEvent(QMouseEvent *event){

    if(is_clickable){

        if(event->button() == Qt::MouseButton::LeftButton){
            qDebug() << "presseded left\n";
            p_frame->setStyleSheet(".QFrame{background-color: rgb(255, 0, 255); border: 6px solid rgb(0, 255, 0); border-radius: 20px;}");
        }else if(event->button() == Qt::MouseButton::RightButton){
            qDebug() << "presseded right\n";
            //p_frame->setStyleSheet(".QFrame{background-color: rgb(198, 100, 255); border: 6px solid yellow; border-radius: 20px;}");
        }
        p_frame->setStyleSheet(".QFrame{background-color: rgb(255, 0, 255); border: 6px solid rgb(0, 255, 0); border-radius: 20px;}");
    }
}


void PlayerCard::mouseReleaseEvent(QMouseEvent *event){

    if(is_clickable){

        if(event->button() == Qt::MouseButton::LeftButton){
            qDebug() << "released left\n";
            //p_frame->setStyleSheet(".QFrame{background-color: rgb(255, 0, 255); border: 6px solid rgb(0, 255, 0); border-radius: 20px;}");
        }else if(event->button() == Qt::MouseButton::RightButton){
            qDebug() << "released right\n";

        }
        p_frame->setStyleSheet(".QFrame{background-color: rgb(198, 100, 255); border: 6px solid yellow; border-radius: 20px;}");

        QString str = p_name->text();

        p_Main_Window->process_users_choise(str);
    }
}


void PlayerCard::set_you_style(){
    setMaximumSize(110,176);
    setMinimumSize(110,176);
    p_frame->setMinimumSize(this->minimumSize());
    p_frame->setMaximumSize(this->maximumSize());

    p_frame->setStyleSheet(".QFrame{ background-color: rgb(255, 127, 255); border: 9px solid rgb(255, 255, 153); border-radius: 20px;}");
}

void PlayerCard::set_killed_style(){
    p_frame->setStyleSheet(".QFrame{background-color: rgb(60, 60, 60); border: 6px solid rgb(180, 180, 180); border-radius: 20px;}");
}

void PlayerCard::show_role(int pl_role){
    QPixmap temp_pix = *p_man_image;
    p_label->setPixmap(temp_pix);

    switch (pl_role)
    {
    case 0:
        p_role->setText("VILLIAN");
        p_man_image = new QPixmap(":/new/images/villian.png");
        *p_man_image = p_man_image->scaled(73,73,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        break;
    case 1:
        p_role->setText("POLICEMAN");
        p_man_image = new QPixmap(":/new/images/policeman.png");
        *p_man_image = p_man_image->scaled(73,73,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        break;
    case 2:
        p_role->setText("DOCTOR");
        p_man_image = new QPixmap(":/new/images/doctor.png");
        *p_man_image = p_man_image->scaled(73,73,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        break;
    case 3:
        p_role->setText("MAFIA");
        p_man_image = new QPixmap(":/new/images/mafia.png");
        *p_man_image = p_man_image->scaled(73,73,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        break;
    }
    p_label->setPixmap(*p_man_image);
    is_clickable = false;

}
