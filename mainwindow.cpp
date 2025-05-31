#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include "protree.h"
#include "picshow.h"
#include <QDebug>
#include <QWizard>
#include <QWizardPage>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QFormLayout>
#include "wizard.h"
#include "protreewidget.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumSize(1629,869);
   //创建菜单栏
    QMenu * menu_file = menuBar()->addMenu(tr("文件(&F)"));
    //创建项目动作
    QAction * act_create_pro = new QAction(QIcon(":/icon/createpro.png"), tr("创建项目"),this);
    act_create_pro->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    menu_file->addAction(act_create_pro);

    //打开项目动作
    QAction * act_open_pro = new QAction(QIcon(":/icon/openpro.png"), tr("打开项目"),this);
    act_open_pro->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    menu_file->addAction(act_open_pro);

    //创建设置菜单
    QMenu * menu_set = menuBar()->addMenu(tr("设置(&S)"));
    //设置背景音乐动作
    QAction * act_music = new QAction(QIcon(":/icon/music.png"), tr("背景音乐"),this);
    act_music->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    menu_set->addAction(act_music);

    //添加自定义控件到主窗口布局
    _protree = new ProTree();
    ui->proLayout->addWidget(_protree,0);

    _picshow = new PicShow();
    ui->picLayout->addWidget(_picshow);

    //连接创建项目槽函数
    connect(act_create_pro, &QAction::triggered, this, &MainWindow::SlotCreatePro);

    //动态类型转化
    QTreeWidget* tree_widget = dynamic_cast <ProTree*>(_protree)->GetTreeWidget();
    auto * pro_tree_widget = dynamic_cast<ProTreeWidget*>(tree_widget);
    auto * pro_pic_show = dynamic_cast<PicShow*>(_picshow);

    //picshow 和 protreewidget之间的通信
    connect(pro_tree_widget, &ProTreeWidget::SigUpdateSelected,pro_pic_show,&PicShow::SlotSelectItem);
    connect(pro_tree_widget, &ProTreeWidget::SigClearSelected,pro_pic_show,&PicShow::SlotDeleteItem );
    connect(pro_pic_show, &PicShow::SigPreClicked,pro_tree_widget,&ProTreeWidget::SlotPreShow);
    connect(pro_pic_show, &PicShow::SigNextClicked,pro_tree_widget,&ProTreeWidget::SlotNextShow);
    connect(pro_tree_widget,&ProTreeWidget::SigUpdatePic,pro_pic_show,&PicShow::SlotUpdatePic);

    //连接打开项目的槽函数
     connect(act_open_pro, &QAction::triggered, this, &MainWindow::SlotOpenPro);
     connect(this, &MainWindow::SigOpenPro, pro_tree_widget, &ProTreeWidget::SlotOpenPro);

     //设置音乐
     connect(act_music, &QAction::triggered, pro_tree_widget, &ProTreeWidget::SlotSetMusic);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//重写resizeEvent
void MainWindow::resizeEvent(QResizeEvent *event)
{
    auto * pro_pic_show = dynamic_cast<PicShow*>(_picshow);
    pro_pic_show->ReloadPic();
    QMainWindow::resizeEvent(event);
}


void MainWindow::SlotCreatePro(bool){
    qDebug() << "slot create pro triggered" << Qt::endl;
    Wizard wizard(this);
    wizard.setWindowTitle(tr("创建项目"));
    auto *page = wizard.page(0);
    page->setTitle(tr("设置项目配置"));
    //Wizard调用done方法时,发送信号回传项目配置
    connect(&wizard, &Wizard::SigProSettings, dynamic_cast<ProTree*>(_protree),&ProTree::AddProToTree);
    wizard.show();
    wizard.exec();
    disconnect(&wizard);
}

//打开文件选择对话框
void MainWindow::SlotOpenPro(bool)
{
    QFileDialog file_dialog;
    file_dialog.setFileMode(QFileDialog::Directory);
    file_dialog.setWindowTitle("选择导入的文件夹");

    file_dialog.setDirectory(QDir::currentPath());
    file_dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (file_dialog.exec()){
         fileNames = file_dialog.selectedFiles();
    }

    if(fileNames.length() <= 0){
          return;
    }

    QString import_path = fileNames.at(0);
    qDebug() << "import_path is " << import_path << Qt::endl;
    //发送信号，ProTreeWidget::SlotOpenPro接收
      emit SigOpenPro(import_path);
}


