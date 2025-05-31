#ifndef PICSHOW_H
#define PICSHOW_H

#include <QDialog>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
class ProTreeItem;
namespace Ui {
class PicShow;
}

class PicShow : public QDialog
{
    Q_OBJECT

public:
    explicit PicShow(QWidget *parent = nullptr);
    ~PicShow();
    void ReloadPic();
protected:
     bool event(QEvent *e) override;
private:
    void ShowPreNextBtns(bool b_show);
    Ui::PicShow *ui;
    QString _selected_path;
    QPixmap _pix_map;
    QPropertyAnimation *_animation_show_pre;
    QPropertyAnimation *_animation_show_next;
    bool _b_btnvisible;
public slots:
    void SlotSelectItem(const QString& path);
    void SlotDeleteItem();
    void SlotUpdatePic(const QString& _path);

signals:
   void SigPreClicked();
   void SigNextClicked();
};

#endif // PICSHOW_H
