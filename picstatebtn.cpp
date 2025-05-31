#include "picstatebtn.h"
#include <QEvent>
#include "const.h"
#include <QDebug>
PicStateBtn::PicStateBtn(QWidget *parent ):QPushButton (parent){

}

void PicStateBtn::SetIcons(const QString &normal, const QString &hover, const QString &pressed,
                           const QString &normal_2, const QString &hover_2, const QString &pressed_2)
{
    _normal = normal;
    _hover = hover;
    _pressed = pressed;

    _normal_2 = normal_2;
    _hover_2 = hover_2;
    _pressed_2 = pressed_2;

    QPixmap tmpPixmap;
    tmpPixmap.load(normal);
    this->resize(tmpPixmap.size());
    this->setIcon(tmpPixmap);
    this->setIconSize(tmpPixmap.size());
    _cur_state = PicBtnStateNormal;
}

PicStateBtn::~PicStateBtn()
{

}


void PicStateBtn::setNormalIcon(){
    qDebug()<<"setNormalIcon _normal " << _normal << endl;
    QPixmap tmpPixmap;
    tmpPixmap.load(_normal);
    this->setIcon(tmpPixmap);
    _cur_state = PicBtnStateNormal;
}

void PicStateBtn::setHoverIcon(){
    qDebug()<<"setHoverIcon _hover " << _hover << endl;
    QPixmap tmpPixmap;
    tmpPixmap.load(_hover);
    this->setIcon(tmpPixmap);
    _cur_state = PicBtnStateHover;
}

void PicStateBtn::setPressIcon(){
    qDebug()<<"setPressIcon _pressed " << _pressed << endl;
    QPixmap tmpPixmap;
    tmpPixmap.load(_pressed);
    this->setIcon(tmpPixmap);
    _cur_state = PicBtnStatePress;
}

void PicStateBtn::setNormal2Icon()
{
    qDebug()<<"setPressIcon _normal_2 " << _normal_2 << endl;
    QPixmap tmpPixmap;
    tmpPixmap.load(_normal_2);
    this->setIcon(tmpPixmap);
    _cur_state = PicBtnState2Normal;
}

void PicStateBtn::setHover2Icon()
{
    qDebug()<<"setPressIcon _hover_2 " << _hover_2 << endl;
    QPixmap tmpPixmap;
    tmpPixmap.load(_hover_2);
    this->setIcon(tmpPixmap);
    _cur_state = PicBtnState2Hover;
}

void PicStateBtn::setPress2Icon()
{
    qDebug()<<"setPressIcon _pressed_2 " << _pressed_2 << endl;
    QPixmap tmpPixmap;
    tmpPixmap.load(_pressed_2);
    this->setIcon(tmpPixmap);
    _cur_state = PicBtnState2Press;
}

void PicStateBtn::SlotStart()
{
    setNormal2Icon();
}

void PicStateBtn::SlotStop()
{
    setNormalIcon();
}


bool PicStateBtn::event(QEvent *event)
{
    switch (event->type())
        {
        case QEvent::Enter:
            if(_cur_state < PicBtnState2Normal){
                 setHoverIcon();
            }else{
                setHover2Icon();
            }

            break;
        case QEvent::Leave:
            if(_cur_state < PicBtnState2Normal){
                setNormalIcon();
            }else{
                setHover2Icon();
            }

            break;
        case QEvent::MouseButtonPress:
            if(_cur_state < PicBtnState2Normal){
                setPressIcon();
            }else{
                setPress2Icon();
            }

            break;
        case QEvent::MouseButtonRelease:
            if(_cur_state < PicBtnState2Normal){
                setHover2Icon();
            }else{
                setHoverIcon();
            }
            break;
        default:
            break;
        }
         return QPushButton::event(event);
}

