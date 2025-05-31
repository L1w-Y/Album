#include "prelistwid.h"
#include "protreeitem.h"
#include "prelistitem.h"
#include "const.h"
#include <QDebug>
#include <QPainter>
#include "const.h"
#include <QGuiApplication>

PreListWid::PreListWid(QWidget *parent):QListWidget(parent),_global(0),_last_index(17)
{
    this->setViewMode(QListWidget::IconMode);//设置内容为图片
    this->setIconSize(QSize(PREICON_SIZE, PREICON_SIZE));//设置图片的大小
    this->setSpacing(5);//设置每个item之间的间隔大小
    connect(this,&PreListWid::itemPressed, this, &PreListWid::SlotItemPressed);
}

PreListWid::~PreListWid()
{

}

void PreListWid::AddListItem(const QString &path)
{
    QPixmap src_pixmap(path);
    src_pixmap = src_pixmap.scaled(PREICON_SIZE,PREICON_SIZE,Qt::KeepAspectRatio);
    QPixmap dst_pixmap(QSize(PREICON_SIZE, PREICON_SIZE));
    auto src_width = src_pixmap.width();
    auto src_height = src_pixmap.height();
//    qDebug() << "src_pixmap.width is " << src_width
//             << " src_pixmap.height is " << src_height;
    auto dist_width = dst_pixmap.width();
    auto dist_height = dst_pixmap.height();
//    qDebug() << "dst_pixmap.width is " << dist_width
//             << " dst_pixmap.height is " << dist_height;
    dst_pixmap.fill(QColor(220,220,220, 50));
    QPainter painter(&dst_pixmap);

    auto x = (dist_width-src_width)/2;
    auto y = (dist_height-src_height)/2;
    painter.drawPixmap(x,y,src_pixmap);

    _global++;
    PreListItem *pItem = new PreListItem(QIcon(dst_pixmap),path,_global,this);
    pItem->setSizeHint(QSize(PREITEM_SIZE,PREITEM_SIZE));
    this->addItem(pItem);
    _set_items[path] = pItem;

    if(_global==1){
        _pos_origin = this->pos();
    }
}

void PreListWid::SlotItemPressed(QListWidgetItem *item)
{
    if(QGuiApplication::mouseButtons() != Qt::LeftButton){
        return;
    }

    auto * list_item = dynamic_cast<PreListItem*>(item);
    auto cur_index = list_item->GetIndex();
    auto path = list_item->GetPath();

//    //每隔18个是一组，一组同屏显示不需要跳转
//    if(cur_index/18 == _last_index/18){
//        this->setCurrentItem(item);
//        emit SigUpSelectShow(path);
//        return;
//    }

//    if(cur_index > 17){
//        auto pos_cur = this->pos();
//        this->move(pos_cur.x()-(cur_index-_last_index)*100, pos_cur.y());
//        _last_index = cur_index;

//    }else{
////        auto pos_cur = this->pos();
////        qDebug()<< "pos_cur is " << pos_cur << "origin_point is " << _origin_point << endl;
//       this->move(_pos_origin);
//        _last_index = 17;
//    }

     this->setCurrentItem(item);
    emit SigUpSelectShow(path);
}



void PreListWid::SlotUpPreList(QTreeWidgetItem *tree_item)
{
    if(!tree_item){
        qDebug() << "tree_item is empty" << endl;
        return;
    }

    auto * pro_item = dynamic_cast<ProTreeItem*>(tree_item);
    auto path = pro_item->GetPath();
    auto iter = _set_items.find(path);
    if(iter != _set_items.end()){
        qDebug() << "path " <<path<< " exists" << endl;
        return;
    }

    AddListItem(path);
}

void PreListWid::SlotUpSelect(QTreeWidgetItem *tree_item)
{
    if(!tree_item){
        qDebug() << "tree_item is empty" << endl;
        return;
    }

    auto * pro_item = dynamic_cast<ProTreeItem*>(tree_item);
    auto path = pro_item->GetPath();
    auto iter = _set_items.find(path);
    if(iter == _set_items.end()){
        qDebug() << "path " <<path<< " not exists" << endl;
       // this->setCurrentItem(iter.value());
        return;
    }

    auto * list_item = dynamic_cast<PreListItem*>(iter.value());
    auto index = list_item->GetIndex();

    if(index > 17){
        auto pos_cur = this->pos();
        this->move(pos_cur.x()-(index-_last_index)*100, pos_cur.y());
        _last_index = index;

    }else{
//        auto pos_cur = this->pos();
//        qDebug()<< "pos_cur is " << pos_cur << "origin_point is " << _origin_point << endl;
       this->move(_pos_origin);
        _last_index = 17;
    }
    this->setCurrentItem(iter.value());
}
