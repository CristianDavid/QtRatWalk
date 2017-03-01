#ifndef RATWALKGUI_EXPORTANGLESDIALOG_H
#define RATWALKGUI_EXPORTANGLESDIALOG_H

#include <QDialog>

namespace RatWalkGui {

namespace Ui {
class ExportAnglesDialog;
}

class ExportAnglesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportAnglesDialog(QWidget *parent = 0);
    ~ExportAnglesDialog();

private:
    Ui::ExportAnglesDialog *ui;
};


} // namespace RatWalkGui
#endif // RATWALKGUI_EXPORTANGLESDIALOG_H
