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

void MainWindow::updateEditedImageDisplay()
{
    new_image = QPixmap::fromImage(*new_imageObj);
    newImage_window->setPixmap(new_image);
    newImage_window->setFixedSize(new_image.width(), new_image.height());
    check_greyscale(*new_imageObj, &isGreyScale);
}

void MainWindow::on_ImportButton_clicked()
{
    // Reading the path of the image to be opened
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "C:/",
                                                     tr("Images (*.png *.xpm *.jpg)"));

    // If the path exists
    if(imagePath != nullptr)
    {
        // Creates the QImages and QPixmaps of the edited and original image
        if(original_imageObj == nullptr)
        {
            original_imageObj = new QImage();
            originalImage_window->setWindowTitle("Original Image");
        }
        original_imageObj->load(imagePath);
        original_image = QPixmap::fromImage(*original_imageObj);
        check_greyscale(*original_imageObj, &isGreyScale);

        if(new_imageObj == nullptr || newImage_window->isHidden())
        {
            new_imageObj = new QImage(original_imageObj->copy());
            newImage_window->setWindowTitle("Edited Image");
            newImage_window->setPixmap(original_image);
        }
        new_image = QPixmap::fromImage(*new_imageObj);

        // Displays the images on the application preserving the aspect ratio
        originalImage_window->setPixmap(original_image);
        originalImage_window->adjustSize();
        originalImage_window->show();
        newImage_window->setFixedSize(new_image.width(), new_image.height());
        newImage_window->show();
    }else
    {
        qWarning() << "Non existent image path";
    }
}

// Function copies original image to edited image
void MainWindow::on_CopyButton_clicked()
{
    if(original_imageObj != nullptr)
    {
        *new_imageObj = original_imageObj->copy(); // Create a new copy
        updateEditedImageDisplay();
        newImage_window->show();
    }
}


void MainWindow::on_GreyOutButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        generate_grey_img(*new_imageObj);
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to gray out!";
    }
}


void MainWindow::on_VerticalFlipButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        mirror(*new_imageObj, 0);
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to mirror!";
    }
}


void MainWindow::on_HorizontalFlipButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        mirror(*new_imageObj, 1);
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to mirror!";
    }
}


void MainWindow::on_QuantizeButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        quantize(*new_imageObj, ui->spinBox->value());
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to quantize!";
    }
}

void MainWindow::on_MakeHistogram_clicked()
{
    if(new_imageObj != nullptr)
    {
        generate_histogram(*new_imageObj, *histogram_window, "Edited Image Histogram");
    }else
    {
        qWarning() << "No image to adjust!";
    }
}

void MainWindow::on_BrightnessButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        linear_transformations(*new_imageObj, ui->doubleSpinBox->value(), 0);
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to adjust!";
    }
}

void MainWindow::on_ContrastButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        linear_transformations(*new_imageObj, ui->doubleSpinBox_2->value(), 1);
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to adjust!";
    }
}

void MainWindow::on_NegativeButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        linear_transformations(*new_imageObj, 0, 2);
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to adjust!";
    }
}

void MainWindow::on_ZoomInButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        zoomIn(*new_imageObj);
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to zoom in!";
    }
}

void MainWindow::on_ZoomOutButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        zoomOut(*new_imageObj, ui->X_factor->value(), ui->Y_factor->value());
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to zoom out!";
    }
}

void MainWindow::on_RotateButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        rotate(*new_imageObj, ui->Clockwise->isChecked());
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to rotate!";
    }
}

void MainWindow::on_SaveButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        // Directory where image will be saved (with it's chosen name)
        QString imagePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                         "C:/",
                                                         tr("Images (*.png *.xpm *.jpg)"));
        new_imageObj->save(imagePath);
    }else
    {
        qWarning() << "There is no edited image to save!";
    }
}
