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
    painter.fillRect(QRectF(QPointF(0,0),QSizeF(w,h)),QBrush(QColor(255,255,255)));
    for (auto j = 1; j < w; ++j) {
        painter.drawLine(QLineF(QPointF(j,h),QPointF(j,0)));
    }
    for (auto i = 1; i < h; ++i) {
        painter.drawLine(QLineF(QPointF(w,i),QPointF(0,i)));
    }
}


void draw_spin(QPainter& painter, QPointF center, double width, double height, int value)
{
    QPointF l1 = center + QPointF(-width/3.,0);
    QPointF r1 = center + QPointF(width/3.,0);
    QPointF up = center + QPointF(0,height/2.);
    QPointF down = center + QPointF(0,-height/2.);

    if (value == 1)
    {
        painter.setPen(QPen(QColor(255,0,0),0.01));
        painter.drawLine(QLineF(l1,up));
        painter.drawLine(QLineF(r1,up));
    }
    else
    {
        painter.setPen(QPen(QColor(0,0,255),0.01));
        painter.drawLine(QLineF(l1,down));
        painter.drawLine(QLineF(r1,down));
    }
    painter.drawLine(QLineF(up,down));
}

void draw_spin(QPainter& painter, QPointF center, int value)
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

    draw_table(painter, w, h);
    for (auto i = 0; i < w; i++) {
        for (auto j = 0; j < h; j++) {
                draw_spin(painter,QPointF{i + 0.5,j + 0.5},values[h*i + j]);
        }
    }
}
