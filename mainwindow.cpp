#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_ImportButton_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "C:/",
                                                     tr("Images (*.png *.xpm *.jpg)"));
    if(imagePath != nullptr)
    {
        imageObj = new QImage();
        imageObj->load(imagePath);
        image = QPixmap::fromImage(*imageObj);

        ui->label_4->setPixmap(image);
        ui->label_4->show();
    }
}

