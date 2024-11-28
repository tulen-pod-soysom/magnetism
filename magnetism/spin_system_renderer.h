#ifndef SPIN_SYSTEM_RENDERER_H
#define SPIN_SYSTEM_RENDERER_H

#include <QWidget>
#include <QPainter>
#include <mutex>

class spin_system_renderer : public QWidget
{
    Q_OBJECT
public:
    explicit spin_system_renderer(QWidget *parent = nullptr);

    std::vector<int> values;

    int width, height;

    void set_range(unsigned height, unsigned width);

    template <typename InputIt>
    void set_values(InputIt begin, InputIt end)
    {
        std::lock_guard<std::mutex> g(data_mutex);
        std::copy(begin,end,values.begin());
    }

signals:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);

    void draw_spin(QPainter &painter, QPointF center, double width, double height, int value);
    void draw_spin(QPainter &painter, QPointF center, int value);

    std::mutex data_mutex;
};

#endif // SPIN_SYSTEM_RENDERER_H
