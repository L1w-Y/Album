#ifndef PICANIMATIONWID_H
#define PICANIMATIONWID_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QTreeWidgetItem>
#include <QMap>
#include <QString>

class PicAnimationWid : public QWidget
{
    Q_OBJECT
public:
    explicit PicAnimationWid(QWidget *parent = nullptr);
    ~PicAnimationWid();
    void SetPixmap(QTreeWidgetItem* item);
    void UpSelectPixmap(QTreeWidgetItem* item);
    void Start();//开始动画
    void Stop();//停止动画
    void SlideNext();
    void SlidePre();
protected:
     virtual void paintEvent(QPaintEvent *event);//重写动画事件
private:
    float _factor; //动画因子(0 - 1.0之间变化)

    QTimer* _timer;
    QPixmap _pixmap1;
    QPixmap _pixmap2;
    QTreeWidgetItem * _cur_item;
    QMap<QString , QTreeWidgetItem*> _map_items;
    bool _b_start;
signals:
    void SigUpPreList(QTreeWidgetItem* );
    void SigSelectItem(QTreeWidgetItem*);
    void SigStart();
    void SigStop();
    void SigStartMusic();
    void SigStopMusic();
public slots:
    void SlotStartOrStop();
    void SlotUpSelectShow(QString path);

private slots:
    void TimeOut();
};

#endif // PICANIMATIONWID_H
