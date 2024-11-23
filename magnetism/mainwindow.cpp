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
    using namespace std::chrono_literals;

    static QFuture<void> future;
    static bool running = true;

    if (checked)
    {
        running = true;

        auto w = ui->SpinBox->value();
        auto h = ui->SpinBox_2->value();
        size = w * h;

        m.temperature = ui->DoubleSpinBox->value();
        m.J = ui->jDoubleSpinBox->value();
        m.set_initial_conditions(w,h);
        ui->widget->set_range(w,h);

        ui->pushButton->setText("Стоп");
        future = QtConcurrent::run([&]{start_simulation(running);});
        draw_timer.start();
    }
    else
    {
        try{
        running = false;
        draw_timer.stop();
        ui->pushButton->setText("Старт");
        mks_counter = 0;
        std::this_thread::sleep_for(100ms);
        future.waitForFinished();
        }

        catch(std::exception exc)
        {
            std::cout << exc.what() << std::endl;
        }
    }
}

void MainWindow::start_simulation(bool& running){
    using namespace std::chrono_literals;

    std::chrono::system_clock clk;

    int plus_spins  = 0;
    int minus_spins = 0;

    while (running)
    {
        auto tp1 = clk.now();

        m.process(size); // 1 MK step
        //m.process();  // not MK step
        mks_counter++;

        auto v = m.get_spins();
        ui->widget->set_values(v.begin(),v.end());


        m.get_spins_statistic(plus_spins, minus_spins);
        ui->SpinBox_4->setValue(plus_spins);
        ui->SpinBox_5->setValue(minus_spins);
        ui->SpinBox_6->setValue(mks_counter);

        auto tp2 = clk.now();

        double speed = ui->SpinBox_3->value();
        std::this_thread::sleep_for(1000ms/speed - (tp2 - tp1));
    }




    return;
}

void MainWindow::timer_event(){
    ui->widget->repaint();
}

