#ifndef PLOTTER_H
#define PLOTTER_H

#include <vector>
#include <QWidget>
#include <QPointF>

namespace RatWalkGui {

class Plotter : public QWidget {
    Q_OBJECT
public:
    Plotter(QWidget *parent = 0);
protected:
    void paintEvent(QPaintEvent *event);
private:
    std::vector<QPointF> points;
};

} // namespace RatWalkGui

#endif // PLOTTER_H
