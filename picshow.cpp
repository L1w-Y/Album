#include "picshow.h"
#include "ui_picshow.h"
#include <QPixmap>
#include <QDir>
#include <QImage>
#include <QEvent>
#include "picbutton.h"
#include <QDebug>

PicShow::PicShow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PicShow),_selected_path(""),_b_btnvisible(false)
{
    ui->setupUi(this);
    ui->previousBtn->SetIcons(":/icon/previous.png",
                              ":/icon/previous_hover.png",
                              ":/icon/previous_press.png");

    ui->nextBtn->SetIcons(":/icon/next.png",
                              ":/icon/next_hover.png",
                              ":/icon/next_press.png");

    connect(ui->nextBtn,&QPushButton::clicked,this, &PicShow::SigNextClicked);
    connect(ui->previousBtn,&QPushButton::clicked,this, &PicShow::SigPreClicked);

    QGraphicsOpacityEffect *opacity_pre = new QGraphicsOpacityEffect(this);
    opacity_pre->setOpacity(0); //设置透明度0.5,透明范围：[0,1]
    ui->previousBtn->setGraphicsEffect(opacity_pre);


    QGraphicsOpacityEffect *opacity_next = new QGraphicsOpacityEffect(this);
    opacity_next->setOpacity(0); //设置透明度0.5,透明范围：[0,1]
    //应用到需要透明变化的控件；
    ui->nextBtn->setGraphicsEffect(opacity_next);

    //使用属性动画类让控件在透明度范围内变化
    _animation_show_pre = new QPropertyAnimation(opacity_pre, "opacity",this);
    _animation_show_pre->setEasingCurve(QEasingCurve::Linear);
    _animation_show_pre->setDuration(500); //动效时长3s


    _animation_show_next = new QPropertyAnimation(opacity_next, "opacity",this);
    _animation_show_next->setEasingCurve(QEasingCurve::Linear);
    _animation_show_next->setDuration(500); //动效时长3s

}

PicShow::~PicShow()
{
    delete ui;
}

void PicShow::ReloadPic()
{
   if(_selected_path != ""){
     const auto &width = ui->gridLayout->geometry().width();
     const auto &height = ui->gridLayout->geometry().height();
     _pix_map.load(_selected_path);

     _pix_map = _pix_map.scaled(width,height,Qt::KeepAspectRatio);
     ui->label->setPixmap(_pix_map);
   }
}

bool PicShow::event(QEvent *event)
{
    switch (event->type())
        {
        case QEvent::Enter:
            ShowPreNextBtns(true);
            break;
        case QEvent::Leave:
            ShowPreNextBtns(false);
            break;
        default:
            break;
    }
    return QDialog::event(event);
}

void PicShow::ShowPreNextBtns(bool b_show)
{
    if(!b_show&&_b_btnvisible){
        _animation_show_pre->stop();
        _animation_show_pre->setStartValue(1);
        _animation_show_pre->setEndValue(0);
        _animation_show_pre->start();

        _animation_show_next->stop();
        _animation_show_next->setStartValue(1);
        _animation_show_next->setEndValue(0);
        _animation_show_next->start();
        _b_btnvisible = false;
        return;
    }

    if(_selected_path ==""){
        return;
    }

    if(b_show&&!_b_btnvisible){
        _animation_show_pre->stop();
       _animation_show_pre->setStartValue(0);
       _animation_show_pre->setEndValue(1);
       _animation_show_pre->start();

       _animation_show_next->stop();
       _animation_show_next->setStartValue(0);
       _animation_show_next->setEndValue(1);
       _animation_show_next->start();
       _b_btnvisible = true;
    }

}



void PicShow::SlotSelectItem(const QString& path)
{
    _selected_path = path;

    _pix_map.load(path);
    auto width = this->width()-20;
    auto height = this->height()-20;
    _pix_map = _pix_map.scaled(width,height,Qt::KeepAspectRatio);
    ui->label->setPixmap(_pix_map);

}

void PicShow::SlotDeleteItem()
{
    _selected_path = "";
}

void PicShow::SlotUpdatePic(const QString &_path)
{
    _selected_path = _path;
    if(_selected_path != ""){
      const auto &width = ui->gridLayout->geometry().width();
      const auto &height = ui->gridLayout->geometry().height();
      _pix_map.load(_selected_path);

      _pix_map = _pix_map.scaled(width,height,Qt::KeepAspectRatio);
      ui->label->setPixmap(_pix_map);
    }
}




