#include "opentreethread.h"
#include <QTreeWidget>
#include <QDir>
#include "protreeitem.h"
#include "const.h"
#include <QDebug>

OpenTreeThread::OpenTreeThread(const QString &src_path, int &file_count,
                               QTreeWidget *self, QObject *parent):
    QThread (parent),_bstop(false),_src_path(src_path),_file_count(file_count)
    ,_self(self),_root(nullptr)
{

}


void OpenTreeThread::OpenProTree(const QString &src_path,
                                int &file_count, QTreeWidget *self)
{
    //创建根节点
    QDir src_dir(src_path);
    auto name = src_dir.dirName();
    auto * item = new ProTreeItem(self, name, src_path,  TreeItemPro);
    item->setData(0,Qt::DisplayRole, name);
    item->setData(0,Qt::DecorationRole, QIcon(":/icon/dir.png"));
    item->setData(0,Qt::ToolTipRole, src_path);
    _root = item;

    //读取根节点下目录和文件
    RecursiveProTree(src_path,file_count,self,_root,item, nullptr);
}

void OpenTreeThread::run()
{
    OpenProTree(_src_path,_file_count,_self);
    if(_bstop&&_root){
        auto path = dynamic_cast<ProTreeItem*>(_root)->GetPath();
        auto index = _self->indexOfTopLevelItem(_root);
        delete _self->takeTopLevelItem(index);
        QDir dir(path);
        dir.removeRecursively();
        return;
    }
    emit SigFinishProgress(_file_count);
}

void OpenTreeThread::RecursiveProTree( const QString &src_path, int &file_count, QTreeWidget *self,
                                       QTreeWidgetItem *root, QTreeWidgetItem *parent,QTreeWidgetItem* preitem)
{
    QDir src_dir(src_path);
    //设置文件过滤器
    QStringList nameFilters;
    src_dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);//除了目录或文件，其他的过滤掉
    src_dir.setSorting(QDir::Name);//优先显示名字
    //存文件和子目录
    QFileInfoList list = src_dir.entryInfoList();
    //qDebug() << "list.size " << list.size() << Qt::endl;
    for(int i = 0; i < list.size(); i++){
        if(_bstop){
            return;
        }
        QFileInfo fileInfo = list.at(i);
        bool bIsDir = fileInfo.isDir();
        //如果是目录
        if (bIsDir)
        {
            if(_bstop){
                return;
            }
            file_count++;
            emit SigUpdateProgress(file_count);
//            qDebug()<< "fileInfo.fileName()" << fileInfo.fileName();
//            qDebug()<< "fileInfo.absoluteFilePath() " << fileInfo.absoluteFilePath();
//            qDebug()<< "fileInfo.absolutePath() " << fileInfo.absolutePath();
            auto * item = new ProTreeItem(_root, fileInfo.fileName(),
                                          fileInfo.absoluteFilePath(), _root,TreeItemDir);
            item->setData(0,Qt::DisplayRole, fileInfo.fileName());
            item->setData(0,Qt::DecorationRole, QIcon(":/icon/dir.png"));
            item->setData(0,Qt::ToolTipRole, fileInfo.absoluteFilePath());

            RecursiveProTree(fileInfo.absoluteFilePath(), file_count,
                             self,root,item, preitem);


         }
        //是图片
        else{
            if(_bstop){
                return;
            }
            //获取后缀
            const QString & suffix = fileInfo.completeSuffix();
            if(suffix != "png" && suffix != "jpeg" && suffix != "jpg"){
                qDebug() << "suffix is not pic " << suffix << Qt::endl;
                continue;
            }

            file_count++;
            //更新进度条
             emit SigUpdateProgress(file_count);

            auto * item = new ProTreeItem(parent, fileInfo.fileName(),
                                          fileInfo.absoluteFilePath(), root,TreeItemPic);
            item->setData(0,Qt::DisplayRole, fileInfo.fileName());
            item->setData(0,Qt::DecorationRole, QIcon(":/icon/pic.png"));
            item->setData(0,Qt::ToolTipRole, fileInfo.absoluteFilePath());
            //前一个节点不为空
            if(preitem){
                auto* pre_proitem = dynamic_cast<ProTreeItem*>(preitem);
                pre_proitem->SetNextItem(item);
            }
            item->SetPreItem(preitem);
            preitem = item;
        }
    }

    emit SigFinishProgress(file_count);
}
