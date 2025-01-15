#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"
#include <vector>
#include <string>
#include <string_view>

using namespace util;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Warning labels for invalid input
    ui->invalid_address_label->setText("<font color='red'>Invalid network address</font>");
    ui->invalid_mask_label->setText("<font color='red'>Invalid mask</font>");
    ui->invalid_address_label->setVisible(false);
    ui->invalid_mask_label->setVisible(false);

    // Set default state for input fields
    ui->address_input->setPlaceholderText("ex. 127.0.0.1");
    ui->octets_button->click();
    ui->mask_input->setPlaceholderText("ex. 255.255.255.0");

    setSubnetOptsVisibility(false);
    setTableVisibility(false);
    initSubnetTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setSubnetOptsVisibility(const bool value) {
    ui->num_subnets_label->setVisible(value);
    ui->num_subnets_spinbox->setVisible(value);
    ui->subnet_bits_label->setVisible(value);
    ui->subnet_bits_spinbox->setVisible(value);

    if (!value) {
        ui->subnet_bits_spinbox->setValue(0);
        ui->num_subnets_spinbox->setValue(0);
    }

    else {
        std::string addr = ui->address_input->text().toStdString();
    }
}

void MainWindow::setTableVisibility(const bool value) {
    ui->subnet_table->setVisible(value);

    if (value) {
        std::string address_str = ui->address_input->text().toStdString();

        std::string mask_str = ui->mask_input->text().toStdString();
        util::MaskFormat format = ui->octets_button->isChecked() ? util::MaskFormat::Octets : util::MaskFormat::CIDR;
        int networkBits { util::networkBits(std::string_view { mask_str }, format) };

        util::buildSubnets(std::string_view { address_str }, networkBits);
    }
}

void MainWindow::initSubnetTable() {
    ui->subnet_table->setColumnWidth(0, 200);
    ui->subnet_table->setColumnWidth(1, 200);
    ui->subnet_table->setColumnWidth(2, 200);
    ui->subnet_table->setColumnWidth(3, 200);
    ui->subnet_table->setColumnWidth(4, 200);
}

void MainWindow::on_cidr_button_clicked()
{
    ui->mask_input->clear();
    ui->mask_input->setPlaceholderText("ex. /24");
}


void MainWindow::on_octets_button_clicked()
{
    ui->mask_input->clear();
    ui->mask_input->setPlaceholderText("ex. 255.255.255.0");
}

void MainWindow::on_address_input_textEdited(const QString &arg1)
{
    std::string address_input = arg1.toStdString();

    // Check for valid IP address (v4)
    if (!address_input.empty()) {
        validNetwork = util::validAddress(std::string_view(address_input));

        if (validNetwork) {
            ui->invalid_address_label->setVisible(false);
        }
        else {
            ui->invalid_address_label->setVisible(true);
        }

        if (validNetwork && validMask) {
            setSubnetOptsVisibility(true);
            setTableVisibility(true);
            ui->invalid_address_label->setVisible(false);
        }
        else {
            setTableVisibility(false);
            setSubnetOptsVisibility(false);
        }

    }
}

void MainWindow::on_mask_input_textEdited(const QString &arg1)
{
    std::string mask_input = arg1.toStdString();

    // Check for valid network mask
    if (!mask_input.empty()) {
        util::MaskFormat format = ui->octets_button->isChecked() ? util::MaskFormat::Octets : util::MaskFormat::CIDR;
        validMask = util::validMask(std::string_view(mask_input), format);

        if (validMask) {
            ui->invalid_mask_label->setVisible(false);
        }
        else {
            ui->invalid_mask_label->setVisible(true);
        }

        if (validMask && validNetwork) {
            setSubnetOptsVisibility(true);
            setTableVisibility(true);
            ui->invalid_mask_label->setVisible(false);
        }
        else {
            setTableVisibility(false);
            setSubnetOptsVisibility(false);
        }
    }
}
