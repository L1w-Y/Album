#ifndef SLIDESHOWDLG_H
#define SLIDESHOWDLG_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QTreeWidgetItem>

namespace Ui {
class SlideShowDlg;
}

class SlideShowDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SlideShowDlg(QWidget *parent = nullptr,QTreeWidgetItem* first_item= nullptr,
                          QTreeWidgetItem* last_item = nullptr);
    ~SlideShowDlg();
    void testPreList();
protected:
    void closeEvent(QCloseEvent *) override;
private:
    QTreeWidgetItem* _first_item;
    QTreeWidgetItem* _last_item;
    Ui::SlideShowDlg *ui;
    QPixmap  _pix_map;
private slots:
    void SlotSlideNext();
    void SlotSlidePre();
    void SlotStartMusic();
signals:
    void SigStartMusic();
    void SigStopMusic();
};

#endif // SLIDESHOWDLG_H
