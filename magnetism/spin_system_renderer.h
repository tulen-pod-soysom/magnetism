#ifndef SPIN_SYSTEM_RENDERER_H
#define SPIN_SYSTEM_RENDERER_H

#include <QWidget>
#include <QPainter>

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
        std::copy(begin,end,values.begin());
    }

signals:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);;

};

#endif // SPIN_SYSTEM_RENDERER_H
