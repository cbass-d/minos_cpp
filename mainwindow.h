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

    void on_subnet_bits_spinbox_valueChanged(int arg1);

    void on_num_subnets_spinbox_valueChanged(int arg1);

private:
    void setSubnetOptsVisibility(const bool value);
    void setTableVisibility(const bool value);
    void initSubnetTable();
    void populateSubnetTable(const std::vector<std::vector<std::string>>& networks);

    Ui::MainWindow *ui;
    bool validNetwork { false };
    bool validMask { false };
    std::string address {};
    std::string mask {};
    int network_bits { 0 };
    int subnet_bits { 0 };
    int num_subnets { 1 };	// Default is 1 since with 0 subnet bits there is still the original network
};
#endif // MAINWINDOW_H
