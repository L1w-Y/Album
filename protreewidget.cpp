#include "protreewidget.h"
#include <QHeaderView>
#include <QDebug>
#include <QGuiApplication>
#include <QMenu>
#include <QDir>
#include "protreeitem.h"
#include "const.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QProgressDialog>
#include <QVector>
#include <QMessageBox>
#include "removeprodialog.h"
#include <QApplication>
#include "protreethread.h"
#include "opentreethread.h"
#include "slideshowdlg.h"


ProTreeWidget::ProTreeWidget(QWidget *parent ):QTreeWidget (parent),_right_btn_item(nullptr),
    _active_item(nullptr),_selected_item(nullptr),_open_progressdlg(nullptr),_slide_show_dlg(nullptr)
{
    qRegisterMetaType<QVector<int> >("QVector<int>");
    //隐藏表头
    this->header()->hide();
    connect(this, &ProTreeWidget::itemPressed, this, &ProTreeWidget::SlotItemPressed);
    connect(this, &ProTreeWidget::itemDoubleClicked, this, &ProTreeWidget::SlotDoubleClickItem);
    _action_import = new QAction(QIcon(":/icon/import.png"),tr("导入文件"), this);
    _action_setstart = new QAction(QIcon(":/icon/core.png"), tr("设置活动项目"),this);
    _action_closepro = new QAction(QIcon(":/icon/close.png"), tr("关闭项目"), this);
    _action_slideshow = new QAction(QIcon(":/icon/slideshow.png"), tr("轮播图播放"),this);

    connect(_action_import, &QAction::triggered, this, &ProTreeWidget::SlotImport);
    connect(_action_closepro, &QAction::triggered, this, &ProTreeWidget::SlotClosePro);
    connect(_action_setstart, &QAction::triggered, this, &ProTreeWidget::SlotSetActive);
    connect(_action_slideshow, &QAction::triggered, this, &ProTreeWidget::SlotSlideShow);

    _player = new QMediaPlayer(this);
    _playlist = new QMediaPlaylist(this);
    _playlist->setPlaybackMode(QMediaPlaylist::Loop);//设置循环模式
    _player->setPlaylist(_playlist);//获取将播放列表要播放的文件

}


//在protree槽函数AddProToTree调用
void ProTreeWidget::AddProToTree(const QString &name, const QString &path)
{
    qDebug() << "ProTreeWidget::AddProToTree name is " << name << " path is " << path << Qt::endl;
    QDir dir(path);
    QString file_path = dir.absoluteFilePath(name);
    //检测重名，判断路径和名字都一样则拒绝加入
    if(_set_path.find(file_path) != _set_path.end()){
        qDebug() << "file has loaded" << Qt::endl;
        return;
    }
    //构造项目用的文件夹
    QDir pro_dir(file_path);
    //如果文件夹不存在则创建
    if(!pro_dir.exists()){
        bool enable = pro_dir.mkpath(file_path);
        if(!enable){
            qDebug() << "pro_dir make path failed" << Qt::endl;
            return;
        }
    }
    _set_path.insert(file_path);
    //创建一个ProTreeItem，设置名称、路径、图标属性
    auto * item = new ProTreeItem(this, name, file_path,  TreeItemPro);
    item->setData(0,Qt::DisplayRole, name);
    item->setData(0,Qt::DecorationRole, QIcon(":/icon/dir.png"));
    item->setData(0,Qt::ToolTipRole, file_path);
}

ProTreeWidget::~ProTreeWidget()
{
    _player->stop();
}

//左侧项目树的点击事件：点击右键生成menu
void ProTreeWidget::SlotItemPressed(QTreeWidgetItem *pressedItem, int column)
{
    qDebug() << "ProTreeWidget::SlotItemPressed" << Qt::endl;
    if(QGuiApplication::mouseButtons() == Qt::RightButton)   //判断是否为右键
        {
            QMenu menu(this);
            qDebug() << "menu addr is " << &menu << Qt::endl;
            int itemtype = (int)(pressedItem->type());
            if (itemtype == TreeItemPro)
            {
                _right_btn_item = pressedItem;
                menu.addAction(_action_import);
                menu.addAction(_action_setstart);
                menu.addAction(_action_closepro);
                menu.addAction(_action_slideshow);
                menu.exec(QCursor::pos());   //菜单弹出位置为鼠标点击位置
            }
    }
}

//左侧项目树的点击事件：点击左键发送信号PicShow::SlotSelectItem接收，展示图片到label
void ProTreeWidget::SlotDoubleClickItem(QTreeWidgetItem* doubleItem, int col){
    qDebug() << "ProTreeWidget::SlotDoubleClickItem" << Qt::endl;

    if(QGuiApplication::mouseButtons() == Qt::LeftButton)   //判断是否为左键
        {
            auto * tree_doubleItem = dynamic_cast<ProTreeItem*>(doubleItem);
            if(!tree_doubleItem){
                return;
            }
            int itemtype = (int)(tree_doubleItem->type());
            if(itemtype == TreeItemPic) {
                emit SigUpdateSelected(tree_doubleItem->GetPath());
                _selected_item = doubleItem;
            }
    }
}

//左侧menu的点击事件：通过子线程导入文件夹
void ProTreeWidget::SlotImport()
{
    QFileDialog file_dialog;
    file_dialog.setFileMode(QFileDialog::Directory);
    file_dialog.setWindowTitle("选择导入的文件夹");
    QString path = "";
    if(!_right_btn_item){
        qDebug() << "_right_btn_item is empty" << Qt::endl;
        path = QDir::currentPath();
        return ;
    }

    path = dynamic_cast<ProTreeItem*>(_right_btn_item)->GetPath();

    file_dialog.setDirectory(path);
    file_dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
      if (file_dialog.exec()){
            fileNames = file_dialog.selectedFiles();
      }

      if(fileNames.length() <= 0){
          return;
      } 
      QString import_path = fileNames.at(0);
     // qDebug() << "import_path is " << import_path << endl;
      int file_count = 0;

     //创建进度条对话框
     _dialog_progress = new QProgressDialog(this);

    //使用 ProTreeThread 处理文件导入过程，并传递文件路径等相关参数
     _thread_create_pro = std::make_shared<ProTreeThread>(std::ref(import_path), std::ref(path),
                                                         _right_btn_item,
                                                         std::ref(file_count), this,_right_btn_item,nullptr);
     //连接更新进度框操作
     connect(_thread_create_pro.get(), &ProTreeThread::SigUpdateProgress,
             this, &ProTreeWidget::SlotUpdateProgress);

     connect(_thread_create_pro.get(), &ProTreeThread::SigFinishProgress, this,
             &ProTreeWidget::SlotFinishProgress);

     connect(_dialog_progress, &QProgressDialog::canceled, this, &ProTreeWidget::SlotCancelProgress);
     connect(this, &ProTreeWidget::SigCancelProgress, _thread_create_pro.get(),
             &ProTreeThread::SlotCancelProgress);
     _thread_create_pro->start();

    //连接信号和槽
    _dialog_progress->setWindowTitle("Please wait...");
    _dialog_progress->setFixedWidth(PROGRESS_WIDTH);
    _dialog_progress->setRange(0, PROGRESS_MAX);
    //显示进度条对话框
    _dialog_progress->exec();
}

//左侧menu的点击事件：关闭项目
void ProTreeWidget::SlotClosePro()
{
    //初始化删除对话框
    RemoveProDialog  remove_pro_dialog;
    auto res = remove_pro_dialog.exec();
    bool b_remove = remove_pro_dialog.IsRemoved();
    auto index_right_btn = this->indexOfTopLevelItem(_right_btn_item);
    auto * protreeitem = dynamic_cast<ProTreeItem*>(_right_btn_item);
    auto * selecteditem = dynamic_cast<ProTreeItem*>(_selected_item);
    //路径管理：并从 _set_path 集合中移除记录
    auto delete_path = protreeitem->GetPath();
     //qDebug() << "remove project from path: " << delete_path;
    _set_path.remove(delete_path);
    //如果用户选择删除，删除文件夹及其内容
    if(b_remove){ 
       QDir delete_dir(delete_path);
       delete_dir.removeRecursively();
    }

    if(protreeitem == _active_item){
        _active_item = nullptr;
    }

    if(selecteditem && protreeitem == selecteditem->GetRoot()){
        selecteditem = nullptr;
         _selected_item = nullptr;
        emit SigClearSelected();
    }

    delete this->takeTopLevelItem(index_right_btn);
    _right_btn_item = nullptr;

}

//左侧menu的点击事件：幻灯片放映
void ProTreeWidget::SlotSlideShow(){
    if(!_right_btn_item){
        return;
    }
    auto *right_pro_item = dynamic_cast<ProTreeItem*>(_right_btn_item);

    auto * last_child_item = right_pro_item->GetLastPicChild();
    if(!last_child_item){
        return;
    }

    qDebug()<< "last child item name is " << last_child_item->GetPath()<< Qt::endl;

    auto * first_child_item = right_pro_item->GetFirstPicChild();
    if(!first_child_item){
        return;
    }

    qDebug()<< "first child item name is " << first_child_item->GetPath()<< Qt::endl;

    _slide_show_dlg = std::make_shared<SlideShowDlg>(this, first_child_item, last_child_item);
    _slide_show_dlg->setModal(true);
    _slide_show_dlg->showMaximized();

}

//左侧menu的点击事件：设置为活动项目
void ProTreeWidget::SlotSetActive()
{
    if(!_right_btn_item){
        return;
    }

    QFont nullFont;
    nullFont.setBold(false);
    if(_active_item){
        _active_item->setFont(0,nullFont);
    }

    _active_item = _right_btn_item;
    nullFont.setBold(true);
    _active_item->setFont(0,nullFont);

}

//更新创建项目时进度对话框 (_dialog_progress) 的进度值
void ProTreeWidget::SlotUpdateProgress(int count)
{
    qDebug() << "count is " << count;
    if(!_dialog_progress){
        qDebug() << "dialog_progress is empty!!!" << Qt::endl;
        return;
    }

    if(count >= PROGRESS_MAX){
         _dialog_progress->setValue(count%PROGRESS_MAX);
    }else{
         _dialog_progress->setValue(count%PROGRESS_MAX);
    }

}

//处理用户取消导入项目
void ProTreeWidget::SlotCancelProgress()
{

    emit SigCancelProgress();
    delete _dialog_progress;
    _dialog_progress =nullptr;
}

void ProTreeWidget::SlotFinishProgress()
{
    _dialog_progress->setValue(PROGRESS_MAX);
    _dialog_progress->deleteLater();
}
//更新文件打开时进度条对话框（_open_progressdlg）的进度值
void ProTreeWidget::SlotUpOpenProgress(int count)
{
    if(!_open_progressdlg){
        return;
    }
    //qDebug()<<"SlotUpOpenProgress count is " << count;
    if(count >= PROGRESS_MAX){
         _open_progressdlg->setValue(count%PROGRESS_MAX);
    }else{
         _open_progressdlg->setValue(count%PROGRESS_MAX);
    }
}

void ProTreeWidget::SlotCancelOpenProgress()
{

}

void ProTreeWidget::SlotFinishOpenProgress()
{
    if(!_open_progressdlg){
        return;
    }
    _open_progressdlg->setValue(PROGRESS_MAX);
    delete  _open_progressdlg;
    _open_progressdlg = nullptr;
}
//处理“显示上一项”的操作
void ProTreeWidget::SlotPreShow(){
    if(!_selected_item){
        return;
    }

    auto * curItem = dynamic_cast<ProTreeItem*>(_selected_item)->GetPreItem();
    if(!curItem){
        return;
    }
    emit SigUpdatePic(curItem->GetPath());
    _selected_item = curItem;
    this->setCurrentItem(curItem);
}
//处理“显示下一项”的操作
void ProTreeWidget::SlotNextShow(){

    if(!_selected_item){
        return;
    }

    auto * curItem = dynamic_cast<ProTreeItem*>(_selected_item)->GetNextItem();
    if(!curItem){
        return;
    }
    emit SigUpdatePic(curItem->GetPath());
    _selected_item = curItem;
    this->setCurrentItem(curItem);
}
//打开项目，在主窗口中与点击menuBar时发送的信号SigOpenPro相连
void ProTreeWidget::SlotOpenPro(const QString& path)
{
    if(_set_path.find(path) != _set_path.end()){
        qDebug() << "file has loaded" << Qt::endl;
        return;
    }

    _set_path.insert(path);
    int file_count = 0;
    QDir pro_dir(path);
    const QString& proname = pro_dir.dirName();

    _thread_open_pro = std::make_shared<OpenTreeThread>(path, file_count, this,nullptr);
    _thread_open_pro->start();

    _open_progressdlg = new QProgressDialog(this);

    //连接更新进度框操作
    connect(_thread_open_pro.get(), &OpenTreeThread::SigUpdateProgress,
            this, &ProTreeWidget::SlotUpOpenProgress);

    connect(_thread_open_pro.get(), &OpenTreeThread::SigFinishProgress, this,
            &ProTreeWidget::SlotFinishOpenProgress);

    _open_progressdlg->setWindowTitle("Please wait...");
    _open_progressdlg->setFixedWidth(PROGRESS_WIDTH);
    _open_progressdlg->setRange(0, PROGRESS_MAX);
    _open_progressdlg->exec();

}

void ProTreeWidget::SlotSetMusic(bool)
{
    qDebug() << "SlotSetMusic" <<Qt::endl;
    QFileDialog file_dialog;
    file_dialog.setFileMode(QFileDialog::ExistingFiles);
    file_dialog.setWindowTitle("选择导入的文件夹");

    file_dialog.setDirectory(QDir::currentPath());
    file_dialog.setViewMode(QFileDialog::Detail);
    file_dialog.setNameFilter("(*.mp3)");

    QStringList fileNames;
    if (file_dialog.exec()){
         fileNames = file_dialog.selectedFiles();
    }

    if(fileNames.length() <= 0){
          return;
    }

    _playlist->clear();

    for(auto filename : fileNames){
        qDebug() << "filename is " << filename << Qt::endl;
        _playlist->addMedia(QUrl::fromLocalFile(filename));
    }

    if(_player->state()!=QMediaPlayer::PlayingState)
      {
          _playlist->setCurrentIndex(0);
      }


}
void ProTreeWidget::SlotStartMusic()
{
    qDebug()<< "ProTreeWidget::SlotStartMusic" << Qt::endl;
     _player->play();
}
void ProTreeWidget::SlotStopMusic()
{
    qDebug()<< "ProTreeWidget::SlotStopMusic" << Qt::endl;
     _player->pause();
}


