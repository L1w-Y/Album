#ifndef PROTREEWIDGET_H
#define PROTREEWIDGET_H
#include <QTreeWidget>
#include <QString>
#include <QMap>
#include <QAction>
#include <QSet>
#include <QTreeWidgetItem>
#include <QProgressDialog>
#include <thread>
#include <QTimer>
#include <memory>
#include<QtMultimedia/QMediaPlayer>
#include<QtMultimedia/QMediaPlaylist>

class ProTreeThread;
class OpenTreeThread;
class SlideShowDlg;
class ProTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    ProTreeWidget(QWidget *parent = nullptr);
    void AddProToTree(const QString& name, const QString& path);
    ~ProTreeWidget();
private:

    QSet<QString> _set_path;
    QAction * _action_import;
    QAction * _action_setstart;
    QAction * _action_closepro;
    QAction * _action_slideshow;
    //右键单击某个item之后弹出菜单
    QTreeWidgetItem * _right_btn_item;
    //设置启动项目
    QTreeWidgetItem * _active_item;
    //双击选中的项目
    QTreeWidgetItem * _selected_item;
    QProgressDialog* _dialog_progress;
    QProgressDialog* _open_progressdlg;
    //copy文件线程
    std::shared_ptr<ProTreeThread>  _thread_create_pro;
    //打开项目线程
    std::shared_ptr<OpenTreeThread> _thread_open_pro;
    //幻灯片展示
    std::shared_ptr<SlideShowDlg> _slide_show_dlg;

    QMediaPlayer* _player;
    QMediaPlaylist * _playlist;

private slots:
    void SlotItemPressed(QTreeWidgetItem * pressedItem, int column);
    void SlotDoubleClickItem(QTreeWidgetItem* doubleItem, int col);
    void SlotImport();
    void SlotClosePro();
    void SlotSetActive();
    void SlotSlideShow();
    void SlotUpdateProgress(int count);
    void SlotCancelProgress();
    void SlotFinishProgress();

    //打开文件进度
    void SlotUpOpenProgress(int count);
    void SlotCancelOpenProgress();
    void SlotFinishOpenProgress();
public slots:
    void SlotPreShow();
    void SlotNextShow();
    void SlotOpenPro(const QString& path);
    void SlotSetMusic(bool);
    void SlotStartMusic();
    void SlotStopMusic();
signals:
    void SigUpdateProgress(int);
    void SigUpdateSelected(const QString&);
    void SigClearSelected();
    void SigCancelProgress();
    void SigUpdatePic(const QString&);
};

#endif // PROTREEWIDGET_H
