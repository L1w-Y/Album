#ifndef PROTREEITEM_H
#define PROTREEITEM_H
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QString>

class ProTreeItem:public QTreeWidgetItem
{
public:
    ProTreeItem(QTreeWidget *view, const QString & name, const QString & path,int type = Type);
    ProTreeItem(QTreeWidgetItem *parent, const QString & name, const QString & path,
                QTreeWidgetItem* root, int type = Type);
    const QString& GetPath();
    QTreeWidgetItem* GetRoot();
    void SetPreItem(QTreeWidgetItem* item);
    void SetNextItem(QTreeWidgetItem* item);
    ProTreeItem* GetPreItem();
    ProTreeItem* GetNextItem();
    ProTreeItem* GetLastPicChild();
    ProTreeItem* GetFirstPicChild();
private:
    QString _path;
    QString _name;
    QTreeWidgetItem* _root;
    QTreeWidgetItem* _pre_item;
    QTreeWidgetItem* _next_item;
};

#endif // PROTREEITEM_H
