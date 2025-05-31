#ifndef PRELISTITEM_H
#define PRELISTITEM_H
#include <QListWidgetItem>
#include <QListWidget>
class PreListItem : public QListWidgetItem
{
public:
    PreListItem(const QIcon &icon, const QString &text, const int &index,
                QListWidget *view = nullptr, int type = Type);
    int GetIndex();
    QString GetPath();
protected:

private:
    QString  _path;
    int _index;
};

#endif // PRELISTITEM_H
