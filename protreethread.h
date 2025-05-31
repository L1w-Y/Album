#ifndef PROTREETHREAD_H
#define PROTREETHREAD_H
#include <QThread>
#include <QObject>
#include <QTreeWidgetItem>
#include <QTreeWidget>
class ProTreeThread : public QThread
{
    Q_OBJECT
public:

    explicit ProTreeThread(const QString &src_path, const QString & dist_path, QTreeWidgetItem *parent_item,
                           int &file_count,QTreeWidget* self,QTreeWidgetItem* root, QObject *parent = nullptr);


    ~ProTreeThread();
protected:
     virtual void run();
private:
     void CreateProTree(const QString& src_path, const QString& dist_path,
                              QTreeWidgetItem* parent_item,
                              int &file_count, QTreeWidget* self, QTreeWidgetItem* root, QTreeWidgetItem* preItem = nullptr);


    QString _src_path;//源路径
    QString _dist_path;//目标路径
    int _file_count;//文件计数器
    QTreeWidgetItem * _parent_item;//父级项目
    QTreeWidget* _self;//当前的树视图指针
    QTreeWidgetItem* _root;//根节点
    bool _bstop;//停止控制

public slots:
    void SlotCancelProgress();

signals:
    void SigUpdateProgress(int);
    void SigFinishProgress(int);

};

#endif // PROTREETHREAD_H
