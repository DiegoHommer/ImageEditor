#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>

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
    void on_ImportButton_clicked();
    void on_CopyButton_clicked();
    void on_GrayOutButton_clicked();
    void on_VerticalFlipButton_clicked();
    void on_HorizontalFlipButton_clicked();
    void on_QuantizeButton_clicked();
    void on_SaveButton_clicked();

private:
    Ui::MainWindow *ui;
    QPixmap original_image;
    QImage  *original_imageObj = nullptr;
    QPixmap new_image;
    QImage *new_imageObj = nullptr;
    QLabel *original_imageWindow = nullptr;
    QLabel *new_imageWindow = nullptr;
};
#endif // MAINWINDOW_H
