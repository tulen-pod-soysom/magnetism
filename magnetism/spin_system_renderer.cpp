#include "spin_system_renderer.h"

spin_system_renderer::spin_system_renderer(QWidget *parent)
    : QWidget{parent}
{}

void spin_system_renderer::set_range(unsigned int height, unsigned int width){
    this->width = width;
    this->height = height;

    values.resize(width*height);
}


void draw_table(QPainter& painter, unsigned w, unsigned h)
{
    painter.fillRect(QRectF(QPointF(0,0),QSizeF(w,h)),QBrush(QColor(127,127,127)));
    painter.fillRect(QRectF(QPointF(1,1),QSizeF(w-2,h-2)),QBrush(QColor(255,255,255)));
    for (auto j = 1; j < w; ++j) {
        painter.drawLine(QLineF(QPointF(j,h),QPointF(j,0)));
    }
    for (auto i = 1; i < h; ++i) {
        painter.drawLine(QLineF(QPointF(w,i),QPointF(0,i)));
    }
}


void spin_system_renderer::draw_spin(QPainter& painter, QPointF center, double w, double h, int value)
{
    QPointF l1 = center + QPointF(-w/3.,0);
    QPointF r1 = center + QPointF(w/3.,0);
    QPointF up = center + QPointF(0,h/2.);
    QPointF down = center + QPointF(0,-h/2.);

    if (value == 1)
    {
        QRectF r(center - QPointF(w/2/0.7,h/2/0.7),QSizeF(w/0.7,h/0.7));
        if ((center.x() < 1) || (center.x() > this->width - 1) ||
            (center.y() < 1) || (center.y() > this->height - 1))
            painter.fillRect(r,QBrush(QColor(63,63,63)));
        else
            painter.fillRect(r,Qt::GlobalColor::black);

        // painter.setPen(QPen(QColor(255,0,0),0.03));
        // painter.drawLine(QLineF(l1,up));
        // painter.drawLine(QLineF(r1,up));
    }
    else
    {
        // painter.setPen(QPen(QColor(0,0,255),0.03));
        // painter.drawLine(QLineF(l1,down));
        // painter.drawLine(QLineF(r1,down));
    }
    // painter.drawLine(QLineF(up,down));
}

void spin_system_renderer::draw_spin(QPainter& painter, QPointF center, int value)
{
    draw_spin(painter,center,0.7,0.7,value);
}

void spin_system_renderer::paintEvent(QPaintEvent *event){
    if (values.empty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect r = this->rect();

    double w = width;
    double h = height;

    QTransform transform;
    transform.reset();
    transform.scale(r.width()/w,-r.height()/h);
    transform.translate(0,-h);

    painter.setTransform(transform);
    painter.setPen(QPen(QColor(0,0,0),0.01));

    // draw_table(painter, w, h);
    painter.fillRect(QRectF(QPointF(0,0),QSizeF(w,h)),QBrush(QColor(127,127,127)));
    painter.fillRect(QRectF(QPointF(1,1),QSizeF(w-2,h-2)),QBrush(QColor(255,255,255)));

    std::lock_guard<std::mutex> g(data_mutex);
    for (auto i = 0; i < w; i++) {
        for (auto j = 0; j < h; j++) {
            draw_spin(painter,QPointF{i + 0.5,j + 0.5},values[h*i + j]);
        }
    }
}
