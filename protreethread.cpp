#include "protreethread.h"
#include <QDir>
#include "protreeitem.h"
#include "const.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include "protreewidget.h"

ProTreeThread::ProTreeThread(const QString &src_path, const QString &dist_path,
                             QTreeWidgetItem *parent_item, int &file_count,
                             QTreeWidget *self, QTreeWidgetItem *root, QObject *parent)
    :QThread (parent),_src_path(src_path),_dist_path(dist_path),
      _file_count(file_count),_parent_item(parent_item),_self(self),_root(root),_bstop(false)
{

}


ProTreeThread::~ProTreeThread()
{

}

void ProTreeThread::run()
{
    CreateProTree(_src_path,_dist_path,_parent_item,_file_count,_self,_root);
    if(_bstop){
        auto path = dynamic_cast<ProTreeItem*>(_root)->GetPath();
        auto index = _self->indexOfTopLevelItem(_root);
        delete _self->takeTopLevelItem(index);
        QDir dir(path);
        dir.removeRecursively();
        return;
    }

    emit SigFinishProgress(_file_count);
}

void ProTreeThread::CreateProTree(const QString &src_path, const QString &dist_path,
                                  QTreeWidgetItem *parent_item, int &file_count,
                                  QTreeWidget *self, QTreeWidgetItem *root, QTreeWidgetItem* preItem)
{
    if(_bstop){
        return;
    }
    bool needcopy = true;
    if(src_path == dist_path){
        needcopy = false;
    }

    QDir import_dir(src_path);
    qDebug() << "src_path is " << src_path << "dis_path is " << dist_path << endl;
    //设置文件过滤器
    QStringList nameFilters;
    import_dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);//除了目录或文件，其他的过滤掉
    import_dir.setSorting(QDir::Name);//优先显示名字
    QFileInfoList list = import_dir.entryInfoList();
    qDebug() << "list.size " << list.size() << endl;
    for(int i = 0; i < list.size(); i++){
        if(_bstop){
            return;
        }
        QFileInfo fileInfo = list.at(i);
        bool bIsDir = fileInfo.isDir();
        if (bIsDir)
        {
            if(_bstop){
                return;
            }
            file_count++;
            emit SigUpdateProgress(file_count);
            QDir dist_dir(dist_path);
            //构造子目的路径
            QString sub_dist_path = dist_dir.absoluteFilePath(fileInfo.fileName());
            qDebug()<< "sub_dist_path " << sub_dist_path;
            //子目的目录
            QDir sub_dist_dir(sub_dist_path);
            //不能存在则创建
            if(!sub_dist_dir.exists()){
                //可以创建多级目录
                bool ok = sub_dist_dir.mkpath(sub_dist_path);
                if(!ok){
                    qDebug()<< "sub_dist_dir mkpath failed"<< endl;
                    continue;
                }
            }

            auto * item = new ProTreeItem(parent_item, fileInfo.fileName(),
                                          sub_dist_path, root,TreeItemDir);
            item->setData(0,Qt::DisplayRole, fileInfo.fileName());
            item->setData(0,Qt::DecorationRole, QIcon(":/icon/dir.png"));
            item->setData(0,Qt::ToolTipRole, sub_dist_path);
             ;
            CreateProTree(fileInfo.absoluteFilePath(), sub_dist_path, item, file_count, self,root,preItem);

         }else{
            if(_bstop){
                return;
            }
            const QString & suffix = fileInfo.completeSuffix();
            if(suffix != "png" && suffix != "jpeg" && suffix != "jpg"){
                qDebug() << "suffix is not pic " << suffix << endl;
                continue;
            }
            file_count++;
            emit SigUpdateProgress(file_count);
            if(!needcopy){
                continue;
            }

            QDir dist_dir(dist_path);
            QString dist_file_path = dist_dir.absoluteFilePath(fileInfo.fileName());
            if(!QFile::copy(fileInfo.absoluteFilePath(), dist_file_path)){
                qDebug() << "file src to dist  copy failed" << endl;
                continue;
            }

            auto * item = new ProTreeItem(parent_item, fileInfo.fileName(),
                                          dist_file_path, root,TreeItemPic);
            item->setData(0,Qt::DisplayRole, fileInfo.fileName());
            item->setData(0,Qt::DecorationRole, QIcon(":/icon/pic.png"));
            item->setData(0,Qt::ToolTipRole, dist_file_path);

            if(preItem){
                auto* pre_proitem = dynamic_cast<ProTreeItem*>(preItem);
                pre_proitem->SetNextItem(item);
            }

            item->SetPreItem(preItem);
            preItem = item;
         }
    }

   //  dynamic_cast<ProTreeWidget*>(self)->SlotFinishProgress();
    parent_item->setExpanded(true);
}


void ProTreeThread::SlotCancelProgress()
{
    this->_bstop = true;
}
