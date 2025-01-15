#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"
#include <string>
#include <cmath>
#include <string_view>
#include <QStandardItemModel>

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
        ui->num_subnets_spinbox->setValue(1);
    }

    else {
        // Set limit on subnet bits and number of subnets based on network mask
        std::string mask_str = ui->mask_input->text().toStdString();
        util::MaskFormat format = ui->octets_button->isChecked() ? util::MaskFormat::Octets : util::MaskFormat::CIDR;
        network_bits = util::networkBits(std::string_view { mask_str }, format);

        int allowed_bits = 32 - network_bits;
        ui->subnet_bits_spinbox->setMaximum(allowed_bits);
        ui->num_subnets_spinbox->setMaximum(std::pow(2, allowed_bits));
    }
}

void MainWindow::setTableVisibility(const bool value) {
    ui->subnet_table->setVisible(value);

    if (value) {
        std::vector<std::vector<std::string>> networks = util::buildSubnets(std::string_view { this->address}, this->network_bits, this->subnet_bits, this->num_subnets);
        populateSubnetTable(networks);
    }
}

void MainWindow::initSubnetTable() {
    ui->subnet_table->setColumnWidth(0, 200);
    ui->subnet_table->setColumnWidth(1, 200);
    ui->subnet_table->setColumnWidth(2, 200);
    ui->subnet_table->setColumnWidth(3, 200);
    ui->subnet_table->setColumnWidth(4, 200);
}

void MainWindow::populateSubnetTable(const std::vector<std::vector<std::string>>& networks) {
    ui->subnet_table->setRowCount(networks.size());
    for(int i = 0; i < networks.size(); i++) {
        QTableWidgetItem* cidr_item = new QTableWidgetItem(QString::fromStdString(networks[i][0]));
        QTableWidgetItem* net_item = new QTableWidgetItem(QString::fromStdString(networks[i][1]));
        QTableWidgetItem* first_item = new QTableWidgetItem(QString::fromStdString(networks[i][2]));
        QTableWidgetItem* last_item = new QTableWidgetItem(QString::fromStdString(networks[i][3]));
        QTableWidgetItem* broadcast_item = new QTableWidgetItem(QString::fromStdString(networks[i][4]));

        cidr_item->setTextAlignment(Qt::Alignment(4));
        net_item->setTextAlignment(Qt::Alignment(4));
        first_item->setTextAlignment(Qt::Alignment(4));
        last_item->setTextAlignment(Qt::Alignment(4));
        broadcast_item->setTextAlignment(Qt::Alignment(4));

        ui->subnet_table->setItem(i, 0, cidr_item);
        ui->subnet_table->setItem(i, 1, net_item);
        ui->subnet_table->setItem(i, 2, first_item);
        ui->subnet_table->setItem(i, 3, last_item);
        ui->subnet_table->setItem(i, 4, broadcast_item);
    }
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
            this->address = address_input;
        }
        else {
            ui->invalid_address_label->setVisible(true);
            this->address.clear();
        }

        if (validNetwork && validMask) {
            setSubnetOptsVisibility(true);
            setTableVisibility(true);
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
            this->mask = mask_input;
        }
        else {
            ui->invalid_mask_label->setVisible(true);
            this->mask.clear();
        }

        if (validMask && validNetwork) {
            setSubnetOptsVisibility(true);
            setTableVisibility(true);
        }
        else {
            setSubnetOptsVisibility(false);
            setTableVisibility(false);
        }
    }
}

void MainWindow::on_subnet_bits_spinbox_valueChanged(int arg1)
{
    int subnet_bits_input { arg1 };

   // Update the value in 'number of subnets' box
    ui->num_subnets_spinbox->setValue(std::pow(2, subnet_bits_input));

    this->subnet_bits = subnet_bits_input;

    std::vector<std::vector<std::string>> networks = util::buildSubnets(std::string_view { this->address}, this->network_bits, this->subnet_bits, this->num_subnets);
    populateSubnetTable(networks);
}


void MainWindow::on_num_subnets_spinbox_valueChanged(int arg1)
{
    int num_subnets_input { arg1 };

    // Update subnet bits field based on number of subnets
    int bits = std::ceil(std::log(num_subnets_input) / std::log(2));

    // Needs to be set before changing value of subnet bits box
    // since the subnets are created when that value is changed
    this->num_subnets = num_subnets_input;

    ui->subnet_bits_spinbox->setValue(bits);

    std::vector<std::vector<std::string>> networks = util::buildSubnets(std::string_view { this->address}, this->network_bits, this->subnet_bits, this->num_subnets);
    populateSubnetTable(networks);

}

