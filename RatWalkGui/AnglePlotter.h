#ifndef ANGLEPLOTTER_H
#define ANGLEPLOTTER_H

#include <QWidget>

#include <RatWalkGui/Plotter.h>

namespace RatWalkGui {

namespace Ui {
class AnglePlotter;
}

class AnglePlotter : public QWidget {
    Q_OBJECT
public:
    explicit AnglePlotter(QWidget *parent = 0);
    ~AnglePlotter();
    Plotter *getPlotter();
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    void on_toolBtnZoomIn_clicked();
    void on_toolBtnZoomOut_clicked();
    void on_toolBtnZoomFitBest_clicked();
private:
    Ui::AnglePlotter *ui;
};

} // namespace RatWalkGui

#endif // ANGLEPLOTTER_H
