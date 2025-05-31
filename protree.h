#ifndef PROTREE_H
#define PROTREE_H

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QTreeWidget>
namespace Ui {
class ProTree;
}

class ProTree : public QDialog
{
    Q_OBJECT

public:
    explicit ProTree(QWidget *parent = nullptr);
    ~ProTree();
    QTreeWidget* GetTreeWidget();
public slots:
    void AddProToTree(const QString& name, const QString& path);
private:
    Ui::ProTree *ui;
};

#endif // PROTREE_H
