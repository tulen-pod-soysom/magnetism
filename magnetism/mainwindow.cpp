#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget->set_range(10,10);

    // std::vector<int> v(100);
    // std::generate_n(v.begin(),100,[]{return rand()> RAND_MAX/2? -1: 1;});

    // ui->widget->set_values(v.begin(),v.end());


    draw_timer.setInterval(1000 / 30);
    connect(&draw_timer, SIGNAL(timeout()), this, SLOT(timer_event()));
}

MainWindow::~MainWindow()
{
    delete ui;
}







void MainWindow::on_pushButton_clicked(bool checked)
{
    static QFuture<void> future;
    static bool running = true;

    if (checked)
    {
        running = true;

        auto w = ui->SpinBox->value();
        auto h = ui->SpinBox_2->value();

        m.temperature = ui->DoubleSpinBox->value();
        m.set_initial_conditions(w,h);
        ui->widget->set_range(w,h);

        ui->widget->set_values(m.spins.begin(),m.spins.end());
        ui->widget->repaint();


        ui->pushButton->setText("Стоп");
        future = QtConcurrent::run([&]{start_simulation(running);});
        draw_timer.start();
    }
    else
    {
        running = false;
        draw_timer.stop();
        ui->pushButton->setText("Старт");
        future.waitForFinished();
    }
}

void MainWindow::start_simulation(bool& running){
    using namespace std::chrono_literals;

    std::chrono::system_clock clk;

    while (running)
    {
        auto tp1 = clk.now();

        m.process();

        auto v = m.get_spins();
        ui->widget->set_values(v.begin(),v.end());

        auto tp2 = clk.now();

        double speed = ui->SpinBox_3->value();
        std::this_thread::sleep_for(1000ms/speed - (tp2 - tp1));
    }
}

void MainWindow::timer_event(){
    ui->widget->repaint();
}

