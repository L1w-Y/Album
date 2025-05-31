#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include "protreewidget.h"
#include "protreeitem.h"
#include "const.h"
#include <QHeaderView>
#include "slideshowdlg.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QFile qss(":/style/style.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("open qss success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug("Open failed");
    }

    w.setWindowTitle("Album");
    w.showMaximized();
    return a.exec();

//    SlideShowDlg dlg;
//    dlg.testPreList();
//    dlg.showMaximized();

   // return a.exec();
}
