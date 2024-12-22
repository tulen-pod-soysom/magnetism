#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget->set_range(12,12);

    // ui->widget_2->addGraph();

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
        bool only_neighbours = ui->only_neighboursCheckBox->isChecked();
        m.set_only_neighbours(only_neighbours);

        auto w = ui->SpinBox->value();
        auto h = ui->SpinBox_2->value();
        size = w * h;

        m.temperature = ui->DoubleSpinBox->value();
        m.J = ui->jDoubleSpinBox->value();
        m.set_per_spins(ui->neg_spins_per->value());
        m.set_initial_conditions(w,h);



        ui->widget->set_range(w+2,h+2);

        ui->pushButton->setText("Стоп");
        future = QtConcurrent::run([&]{start_simulation(running);});
        draw_timer.start();
    }
    else
    {
        running = false;
        draw_timer.stop();
        ui->pushButton->setText("Старт");
        mks_counter = 0;
        future.waitForFinished();
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


        // QVector<double> energy(m.energy_sequence.begin(),m.energy_sequence.end());

        // QVector<double> linspace(energy.size());
        // std::iota(linspace.begin(),linspace.end(),0);

        // ui->widget_2->graph(0)->setData(linspace,energy,true);
        // ui->widget_2->rescaleAxes();

        m.get_spins_statistic(plus_spins, minus_spins);

        // changing values to often causes a crash

         ui->SpinBox_4->setValue(plus_spins);
         ui->SpinBox_5->setValue(minus_spins);
        if (!(mks_counter % 100))
            ui->SpinBox_6->setValue(mks_counter);

        auto tp2 = clk.now();

        double speed = ui->DoubleSpinBox_3->value() * size;

        auto t = 1'000'000us / speed - (tp2 - tp1);
        if (t.count() < 0)
            continue;
        else
            std::this_thread::sleep_for(t);
    }

    return;
}

void MainWindow::timer_event(){
    ui->widget->repaint();
    // ui->widget_2->replot();
}


