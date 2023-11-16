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
    isGreyScale = check_greyscale(*new_imageObj);
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
        isGreyScale = check_greyscale(*original_imageObj);

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
        vector<int> histogram(256,0);
        generate_histogram(*new_imageObj, histogram);
        display_histogram(histogram, *histogram_window, "Edited Image Histogram");
    }else
    {
        qWarning() << "No image to make histogram with!";
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


void MainWindow::on_EqualizeButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        // Display a window with the image before it is equalized
        newImageBeforeMatching_window->setPixmap(new_image);
        newImageBeforeMatching_window->setWindowTitle("Edited Image before matching");
        newImageBeforeMatching_window->setFixedSize(new_image.width(), new_image.height());
        newImageBeforeMatching_window->show();

        equalize_histogram(*new_imageObj, *histogram_before_window, *histogram_window, isGreyScale);
        updateEditedImageDisplay();
    }else
    {
        qWarning() << "No image to equalize!";
    }
}

void MainWindow::on_MatchButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        QMessageBox::information(this, "Matching", "Choose a greyscale target image for the matching");

        // Reading the path of the target image to be opened
        QString imagePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                         "C:/",
                                                         tr("Images (*.png *.xpm *.jpg)"));

        // If the path exists
        if(imagePath != nullptr)
        {
            QImage *target_imageObj = new QImage();
            target_imageObj->load(imagePath);

            // If both target and source images are greyscale
            if(check_greyscale(*target_imageObj) && isGreyScale)
            {
                histogram_matching(*new_imageObj, *target_imageObj, *histogram_before_window, *histogram_window);
                updateEditedImageDisplay();
            }else
            {
                qWarning() << "One of the images is not in greyscale";
            }
        }
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

void MainWindow::on_ConvolveButton_clicked()
{
    if(new_imageObj != nullptr)
    {
        // Set the kernel to be the 180º rotated version of the input kernel (so it can be used directly on convolution)
        vector<double> kernel = {ui->Kernel9->value(), ui->Kernel6->value(), ui->Kernel3->value(),
                                 ui->Kernel8->value(), ui->Kernel5->value(), ui->Kernel2->value(),
                                 ui->Kernel7->value(), ui->Kernel4->value(), ui->Kernel1->value()};

        convolve(*new_imageObj, kernel, ui->Embossing->isChecked());
        updateEditedImageDisplay();
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
