#include "slideshowdlg.h"
#include "ui_slideshowdlg.h"
#include <QDebug>
#include "protreeitem.h"
#include "prelistwid.h"
#include "protreewidget.h"

SlideShowDlg::SlideShowDlg(QWidget *parent, QTreeWidgetItem *first_item,
                           QTreeWidgetItem *last_item): QDialog(parent),
    ui(new Ui::SlideShowDlg),_first_item(first_item),_last_item(last_item)
{
    ui->setupUi(this);
    //隐藏窗口
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    ui->slidpreBtn->SetIcons(":/icon/previous.png",
                              ":/icon/previous_hover.png",
                              ":/icon/previous_press.png");

    ui->slidenextBtn->SetIcons(":/icon/next.png",
                              ":/icon/next_hover.png",
                              ":/icon/next_press.png");
    ui->closeBtn->SetIcons(":/icon/closeshow.png",":/icon/closeshow_hover.png",
                           ":/icon/closeshow_press.png");

    ui->playBtn->SetIcons(":/icon/play.png",":/icon/play_hover.png",":/icon/play_press.png",
                          ":/icon/pause.png",":/icon/pause_hover.png",":/icon/pause_press.png");

    //连接关闭按钮
    connect(ui->closeBtn, &QPushButton::clicked, this, &SlideShowDlg::close);
    //连接向后查看按钮
    connect(ui->slidenextBtn, &QPushButton::clicked,this,&SlideShowDlg::SlotSlideNext);
    //连接向前查看按钮
    connect(ui->slidpreBtn, &QPushButton::clicked,this,&SlideShowDlg::SlotSlidePre);

    auto * prelistWid = dynamic_cast<PreListWid*>(ui->preListWidget);
//    qDebug()<<"ui->picAnimation is " << ui->picAnimation << endl;
//    qDebug()<<"prelistWid is " << prelistWid << endl;

    connect(ui->picAnimation, &PicAnimationWid::SigUpPreList, prelistWid,&PreListWid::SlotUpPreList);
    connect(ui->picAnimation, &PicAnimationWid::SigSelectItem, prelistWid, &PreListWid::SlotUpSelect);
    //连接动画和按钮显示状态
    connect(ui->picAnimation,&PicAnimationWid::SigStart, ui->playBtn, &PicStateBtn::SlotStart);
    connect(ui->picAnimation,&PicAnimationWid::SigStop, ui->playBtn, &PicStateBtn::SlotStop);

    //按钮控制启动动画和停止动画
    connect(ui->playBtn, &PicStateBtn::clicked, ui->picAnimation, &PicAnimationWid::SlotStartOrStop);

    auto * _protree_widget = dynamic_cast<ProTreeWidget*>(parent);

    //连接音乐启动信号
    connect(ui->picAnimation, &PicAnimationWid::SigStartMusic, _protree_widget,
            &ProTreeWidget::SlotStartMusic);
    //连接音乐关闭信号
    connect(ui->picAnimation,&PicAnimationWid::SigStopMusic, _protree_widget,
            &ProTreeWidget::SlotStopMusic);

    //连接下方预览条点击与上方动画区图片显示
    connect(prelistWid, &PreListWid::SigUpSelectShow,
            ui->picAnimation, &PicAnimationWid::SlotUpSelectShow);

    ui->picAnimation->SetPixmap(_first_item);
    ui->picAnimation->Start();


}

SlideShowDlg::~SlideShowDlg()
{
    delete ui;
}

void SlideShowDlg::testPreList()
{
    for(int i = 0; i < 2; i++){
        auto * prelistWid = dynamic_cast<PreListWid*>(ui->preListWidget);
        prelistWid->AddListItem(":/icon/show.jpg");
    }

}

void SlideShowDlg::closeEvent(QCloseEvent * event)
{
    ui->picAnimation->Stop();
    QDialog::closeEvent(event);
}

void SlideShowDlg::SlotSlideNext()
{
    ui->picAnimation->SlideNext();
}

void SlideShowDlg::SlotSlidePre()
{
    ui->picAnimation->SlidePre();
}

void SlideShowDlg::SlotStartMusic()
{
    qDebug() << "SlideShowDlg::SlotStartMusic" << Qt::endl;
    emit SigStartMusic();
}



