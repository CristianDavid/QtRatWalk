#ifndef RATWALKGUI_MDIMAINWINDOW_H
#define RATWALKGUI_MDIMAINWINDOW_H

#include <QMainWindow>

namespace RatWalkGui {

namespace Ui {
class MdiMainWindow;
}

class MdiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MdiMainWindow(QWidget *parent = 0);
    ~MdiMainWindow();

private:
    Ui::MdiMainWindow *ui;
};


} // namespace RatWalkGui
#endif // RATWALKGUI_MDIMAINWINDOW_H
