#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "functions.h"

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
        original_imageObj = new QImage();
        original_imageObj->load(imagePath);
        original_image = QPixmap::fromImage(*original_imageObj);
        new_imageObj = new QImage(original_imageObj->copy());
        new_image = QPixmap::fromImage(*new_imageObj);

        int h = ui->Original_Image->height();
        int w = ui->Original_Image->width();

        ui->Original_Image->setPixmap(original_image.scaled(h, w, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->Edited_Image->setPixmap(original_image.scaled(h, w, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

// Function copies original image to edited image
void MainWindow::on_CopyButton_clicked()
{
    if(original_imageObj != nullptr)
    {
        if (new_imageObj) {
            delete new_imageObj; // Deallocate the existing QImage
        }

        int h = ui->Edited_Image->height();
        int w = ui->Edited_Image->width();

        new_imageObj = new QImage(original_imageObj->copy()); // Create a new copy
        new_image = QPixmap::fromImage(*new_imageObj);
        ui->Edited_Image->setPixmap(new_image.scaled(h, w, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}


void MainWindow::on_GrayOutButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        int h = ui->Edited_Image->height();
        int w = ui->Edited_Image->width();

        generate_gray_img(*new_imageObj);
        new_image = QPixmap::fromImage(*new_imageObj);
        ui->Edited_Image->setPixmap(new_image.scaled(h, w, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }else
    {
        qWarning() << "No image to gray out!";
    }
}


void MainWindow::on_VerticalFlipButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        int h = ui->Edited_Image->height();
        int w = ui->Edited_Image->width();

        mirror(*new_imageObj, 0);
        new_image = QPixmap::fromImage(*new_imageObj);
        ui->Edited_Image->setPixmap(new_image.scaled(h, w, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }else
    {
        qWarning() << "No image to mirror!";
    }
}


void MainWindow::on_HorizontalFlipButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        int h = ui->Edited_Image->height();
        int w = ui->Edited_Image->width();

        mirror(*new_imageObj, 1);
        new_image = QPixmap::fromImage(*new_imageObj);
        ui->Edited_Image->setPixmap(new_image.scaled(h, w, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }else
    {
        qWarning() << "No image to mirror!";
    }
}


void MainWindow::on_QuantizeButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        int h = ui->Edited_Image->height();
        int w = ui->Edited_Image->width();

        quantize(*new_imageObj, ui->spinBox->value());
        new_image = QPixmap::fromImage(*new_imageObj);
        ui->Edited_Image->setPixmap(new_image.scaled(h, w, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }else
    {
        qWarning() << "No image to quantize!";
    }
}


void MainWindow::on_SaveButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        QString imagePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                         "C:/",
                                                         tr("Images (*.png *.xpm *.jpg)"));
        new_imageObj->save(imagePath);
    }else
    {
        qWarning() << "There is no edited image to save!";
    }
}

