#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    Ui::MainWindow *ui;
    QWidget* _protree;
    QWidget* _picshow;
    QMap<QString, QString> _map_projs;
private slots:
    void SlotCreatePro(bool);
    void SlotOpenPro(bool);

signals:
    void SigOpenPro(const QString& path);
};

#endif // MAINWINDOW_H
