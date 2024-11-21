#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include <vector>
#include <izing_model.hpp>
#include <chrono>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    izing_model::model m;

private slots:
    void on_pushButton_clicked(bool checked);
    void start_simulation(bool& running);
    void timer_event();

private:
    Ui::MainWindow *ui;
    QTimer draw_timer;
};
#endif // MAINWINDOW_H
