#ifndef OPENTREETHREAD_H
#define OPENTREETHREAD_H

#include <QThread>
#include <QObject>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDir>
class OpenTreeThread:public QThread
{
     Q_OBJECT
public:
    explicit OpenTreeThread(const QString&src_path, int &file_count,
                           QTreeWidget* self,QObject *parent = nullptr);
    void OpenProTree(const QString &src_path,
                                    int &file_count, QTreeWidget *self);
protected:
     virtual void run();
private:
    void RecursiveProTree(const QString &src_path,
                          int &file_count, QTreeWidget *self,
                          QTreeWidgetItem* root, QTreeWidgetItem* parent, QTreeWidgetItem* preitem);
    QString _src_path;//源路径，用于打开项目树的根路径
    int _file_count;//处理的文件计数器
    QTreeWidget* _self;//关联的 QTreeWidget 实例，用于在其中添加项目
    bool _bstop;//控制线程停止的标志
    QTreeWidgetItem* _root;//保存项目树的根节点
signals:
     void SigFinishProgress(int);
     void SigUpdateProgress(int);
};

#endif // OPENTREETHREAD_H
