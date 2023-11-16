#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateEditedImageDisplay();
    void on_ImportButton_clicked();
    void on_CopyButton_clicked();
    void on_GreyOutButton_clicked();
    void on_VerticalFlipButton_clicked();
    void on_HorizontalFlipButton_clicked();
    void on_QuantizeButton_clicked();
    void on_SaveButton_clicked();
    void on_BrightnessButton_clicked();
    void on_ContrastButton_clicked();
    void on_NegativeButton_clicked();
    void on_MakeHistogram_clicked();

    void on_ZoomInButton_clicked();

    void on_ZoomOutButton_clicked();

    void on_RotateButton_clicked();

    void on_ConvolveButton_clicked();
    void on_EqualizeButton_clicked();

    void on_MatchButton_clicked();

private:
    Ui::MainWindow *ui;
    QPixmap original_image;
    QImage  *original_imageObj = nullptr;
    QPixmap new_image;
    QImage *new_imageObj = nullptr;
    QLabel *originalImage_window = new QLabel();
    QLabel *newImage_window = new QLabel();
    QLabel *newImageBeforeMatching_window = new QLabel();
    QLabel *histogram_before_window = new QLabel();
    QLabel *histogram_window = new QLabel();
    bool isGreyScale = false;
};
#endif // MAINWINDOW_H
