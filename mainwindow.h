#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_cidr_button_clicked();

    void on_octets_button_clicked();

    void on_address_input_textEdited(const QString &arg1);

    void on_mask_input_textEdited(const QString &arg1);

private:
    void setSubnetOptsVisibility(const bool value);
    void setTableVisibility(const bool value);
    void initSubnetTable();

    Ui::MainWindow *ui;
    bool validNetwork { false };
    bool validMask { false };
};
#endif // MAINWINDOW_H
