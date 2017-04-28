#ifndef RATWALKGUI_SHOWIMAGEDIALOG_H
#define RATWALKGUI_SHOWIMAGEDIALOG_H

#include <QDialog>
#include <QImage>
#include <QString>

namespace RatWalkGui {

namespace Ui {
class ShowImageDialog;
}

class ShowImageDialog : public QDialog {
   Q_OBJECT
public:
   ShowImageDialog(QString title, QImage image, QWidget *parent = 0);
   ~ShowImageDialog();
private:
   Ui::ShowImageDialog *ui;
};


} // namespace RatWalkGui
#endif // RATWALKGUI_SHOWIMAGEDIALOG_H
