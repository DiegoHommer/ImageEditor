#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <bits/stdc++.h>
#include <iostream>
#include <QImage>
#include <QLabel>
#include <QPainter>

using namespace std;

void generate_grey_img(QImage &img)
{
    for(int y = 0; y < img.height(); y++)
    {
        for(int x = 0; x < img.width(); x++)
        {
            // Gets the Rgb colors from original pixel
            QColor color = img.pixelColor(x, y);

            // L = 0.299*R + 0.587*G + 0.114*B
            int grey = (0.299 * color.red())+(0.587 * color.green()) + (0.114 * color.blue());

            // Ri = Gi = Bi = Li on new pixel;
            img.setPixelColor(x,y,QColor(grey,grey,grey));
        }
    }
}

void check_greyscale(QImage &img, bool *isGrey)
{
    for(int y = 0; y < img.height(); y++)
    {
        for(int x = 0; x < img.width(); x++)
        {
            // Gets the Rgb colors from original pixel
            QColor color = img.pixelColor(x, y);

            if((color.red() != color.blue())
                || (color.blue() != color.green())
                || (color.red() != color.green()))
            {
                *isGrey = false;
                return;
            }
        }
    }
    *isGrey = true;
    return;
}

void mirror(QImage &img, bool option)
{
    QImage temp_img = img;
    int h_temp = temp_img.height();
    int w_temp = temp_img.width();

    switch(option)
    {
        // Vertical Mirroring (use of memcpy and scanLine for efficiency)
        case 0:
            for(int y = 0; y < h_temp; y += 1)
            {
                // Pointer to source row (1º row -> 2º row...)
                uchar* sourceRow = temp_img.scanLine(y);

                // Pointer to target row (last row -> penultimate row...)
                uchar* targetRow = img.scanLine(h_temp - y - 1);

                // Copies the source row data (pixels) to target row
                memcpy(targetRow, sourceRow, temp_img.bytesPerLine());
            }
            break;

        // Horizontal Mirroring (transfer pixel by pixel)
        // equivalent to swapping columns [1º column to last -> 2º column to penultimate])
        case 1:
            // Loop for rows
            for(int y = 0; y < h_temp; y += 1)
            {
                // Loop for columns
                for(int x = 0; x < w_temp; x += 1)
                {
                    // Transfers (1º original pixel to last new-> 2º pixel to penultimate ...)
                    img.setPixel(w_temp - x - 1, y, temp_img.pixel(x,y));
                }
            }
            break;
    }
}

void quantize(QImage &img, int num_shades)
{
    int max_shade = INT_MIN, min_shade = INT_MAX, interval_size;
    vector<float> bins;
    int h_temp = img.height();
    int w_temp = img.width();

    // Looks for the most and least intense shade in the image
    for(int y = 0; y < h_temp; y++)
    {
        for(int x = 0; x < w_temp; x++)
        {
            int greyscaleValue = qRed(img.pixel(x,y));
            if(greyscaleValue < min_shade) min_shade = greyscaleValue;
            if(greyscaleValue > max_shade) max_shade = greyscaleValue;
        }
    }
    interval_size = max_shade - min_shade + 1;

    if(interval_size > num_shades)
    {
            int i = 0;
            int center;
            float tb = static_cast<float>(interval_size) / num_shades;
            float upper;

            // Creation of the intervals (bins) for quantizing
            for(upper = min_shade - 0.5; i <= num_shades + 1; i++, upper += tb)
            {
                bins.push_back(upper);
            }

            // Image quantization
            for(int y = 0; y < h_temp; y++)
            {
                for(int x = 0; x < w_temp; x++)
                {
                    // Looks for the bin correspondent to the original image shade
                    auto it = upper_bound(bins.begin(), bins.end(), img.pixelColor(x,y).red());

                    // Saves the closest integer to the center of the bin
                    center = std::clamp(static_cast<int>((*(it - 1) + *(it)) / 2), min_shade, max_shade);

                    // Shade of pixel in new image = center integer
                    img.setPixel(x,y,qRgb(center,center,center));
                }
            }
    }
}

void linear_transformations(QImage &img, double scale, int option)
{
    for(int y = 0; y < img.height(); y++)
    {
            for(int x = 0; x < img.width(); x++)
            {
                // Gets the Rgb colors from original pixel
                QColor new_color = img.pixelColor(x, y);

                switch(option)
                {
                    // Adjust Brightness
                    case 0:
                        // Adjusts pixel value by adding the scale to each channel value of the pixel
                        new_color.setRed(std::clamp((int)(new_color.red() + scale), 0, 255));
                        new_color.setBlue(std::clamp((int)(new_color.blue() + scale), 0, 255));
                        new_color.setGreen(std::clamp((int)(new_color.green() + scale), 0, 255));
                        break;

                    // Adjust Contrast
                    case 1:
                        // Adjusts pixel value by multiplying the scale with each channel value of the pixel
                        new_color.setRed(std::clamp((int)(new_color.red() * scale), 0, 255));
                        new_color.setBlue(std::clamp((int)(new_color.blue() * scale), 0, 255));
                        new_color.setGreen(std::clamp((int)(new_color.green() * scale), 0, 255));
                        break;

                    // Image negative
                    case 2:
                        // Calculates the negative of the pixel by doing 255 - old_value
                        new_color.setRed(std::clamp(255 - new_color.red(), 0, 255));
                        new_color.setBlue(std::clamp(255 - new_color.blue(), 0, 255));
                        new_color.setGreen(std::clamp(255 - new_color.green(), 0, 255));
                        break;

                }

                // New pixel has it's brightness adjusted;
                img.setPixelColor(x,y,new_color);
            }
    }
}

void display_histogram(vector<int> histogram, QLabel &hist_window, QString window_name)
{
    // Declare the PixelMap that will serve as the "canvas"
    QPixmap hist_map(260,260);
    hist_map.fill(Qt::white);

    QPainter painter(&hist_map);

    // Painting normalized histogram columns for display
    for(int i = 0; i < 256; i++)
    {
            int xPos = i;
            int yPos = 260 - histogram[i];
            int columnWidth = 1;
            int columnHeight = histogram[i];

            // Paints a column (top to bottom) starting from (xPos,yPos) with columnHeight black pixels
            painter.fillRect(QRect(xPos, yPos, columnWidth, columnHeight), Qt::black);
    }

    // Setup window for display
    hist_window.setWindowTitle(window_name);
    hist_window.setPixmap(hist_map.scaled(hist_window.height(), hist_window.width(), Qt::KeepAspectRatio));
    hist_window.show();
}

void generate_histogram(QImage &img, QLabel &hist_window, QString window_name)
{
    vector<int> histogram(256,0);
    int total_pixels = img.width() * img.height();
    float scaling_factor;

    // Histogram computation
    for(int y = 0; y < img.height(); y++)
    {
            for(int x = 0; x < img.width(); x++)
            {
                 // Gets the Rgb colors from original pixel
                QColor color = img.pixelColor(x, y);

                // Adds 1 to the corresponding shade collumn of the histogram
                histogram[color.red()]++;
            }
    }

    scaling_factor = 255.0 / *std::max_element(histogram.begin(), histogram.end());

    // Histogram normalization
    for(auto it = histogram.begin(); it != histogram.end(); it++)
    {
        // Multiplies each histogram column by scaling factor for visualization
        *it = static_cast<int>(*it * scaling_factor);
    }

    display_histogram(histogram, hist_window, window_name);
}

void equalize_histogram(QImage &img, QLabel &hist_window, bool isGrey)
{
}

QColor average_color(QImage &img, int x_start, int y_start, int x_end, int y_end, int num_pixels)
{
    int average_red = 0;
    int average_blue = 0;
    int average_green = 0;

    for(int y = y_start; y < y_end; y++)
    {
        for(int x = x_start; x < x_end; x++)
        {
            QColor color = img.pixelColor(x, y);
            average_red += color.red();
            average_blue += color.blue();
            average_green += color.green();
        }
    }
    average_red = static_cast<int>(average_red / num_pixels);
    average_blue = static_cast<int>(average_blue / num_pixels);
    average_green = static_cast<int>(average_green / num_pixels);
    return QColor(average_red, average_green, average_blue);
}

void zoomOut(QImage &img, int Sx, int Sy)
{
    int newHeight = static_cast<int>(floor(img.height() / Sy));
    int newWidth = static_cast<int>(floor(img.width() / Sx));
    int num_pixels = Sx * Sy;

    QImage zoomedImage(newWidth, newHeight, QImage::Format_ARGB32);

    for(int y = 0; y < newHeight; y++)
    {
        for(int x = 0; x < newWidth; x++)
        {
            // Fill up the zoomed image pixels with
            int x_start = x * Sx;
            int y_start = y * Sy;
            int x_end = std::min(x_start + Sx, img.width());
            int y_end = std::min(y_start + Sy, img.height());


            QColor color = average_color(img, x_start, y_start, x_end, y_end, num_pixels);
            zoomedImage.setPixelColor(x, y, color);
        }
    }

    // Update the original image in place
    img = std::move(zoomedImage);
}

QColor linear_interpolation(QColor pix1, QColor pix2)
{
    QColor interpol_pix;
    interpol_pix.setRed(static_cast<int>((pix1.red() + pix2.red()) / 2));
    interpol_pix.setBlue(static_cast<int>((pix1.blue() + pix2.blue()) / 2));
    interpol_pix.setGreen(static_cast<int>((pix1.green() + pix2.green()) / 2));
    return interpol_pix;
}

void zoomIn(QImage &img)
{
    // Define the new size (e.g., double the width and height)
    int newHeight = (img.height() * 2);
    int newWidth = (img.width() * 2);

    QImage zoomedImage(newWidth, newHeight, QImage::Format_ARGB32);

    /*
        Loop for initizaling columns/rows (leaving empty columns/rows between each one)
      and using linear interpolation to fill up the rows of zoomed image.
    */
    for(int y = 0; y < img.height(); y++)
    {
        for(int x = 0; x < img.width() - 1; x++)
        {
            // Fill up the zoomed image pixels with their respective original pixels

            // (Zoomed image pixels located at even rows and columns simultaneously)
            QColor color = img.pixelColor(x, y);
            zoomedImage.setPixelColor(x * 2, y * 2, color);

            // (Zoomed image pixels located at even rows and uneven columns simultaneously)
            QColor interpol_color = linear_interpolation(img.pixelColor(x,y), img.pixelColor(x + 1, y));
            zoomedImage.setPixelColor((x * 2) + 1, y * 2, interpol_color);
        }
        // Deal with exceptions (last and second to last columns)
        QColor lastX = img.pixelColor(img.width() - 1, y);
        zoomedImage.setPixelColor((img.width() - 1) * 2, y * 2, lastX);
        zoomedImage.setPixelColor(newWidth - 1, y * 2, lastX);
    }

    /*
        Loop for using linear interpolation to fill up the columns of zoomed image.
    */
    for(int x = 0; x < newWidth; x++)
    {
        for(int y = 1; y < newHeight - 1; y += 2)
        {
            QColor pixel1 = zoomedImage.pixelColor(x,y - 1);
            QColor pixel2 = zoomedImage.pixelColor(x,y + 1);

            // (Zoomed image pixels located at even rows and uneven columns simultaneously)
            QColor interpol_color = linear_interpolation(pixel1, pixel2);
            zoomedImage.setPixelColor(x, y, interpol_color);
        }
        // Deal with exceptions (last row)
        QColor lastY = zoomedImage.pixelColor(x, newHeight - 2);
        zoomedImage.setPixelColor(x, newHeight - 1, lastY);
    }

    // Update the original image in place
    img = std::move(zoomedImage);
}

void rotate(QImage &img, bool clockwise)
{
    // Image with inverse height and width dimensions (when compared to original)
    QImage rotatedImage(img.height(), img.width(), QImage::Format_ARGB32);

    if(clockwise)
    {
        for(int y = 0; y < img.height(); y++)
        {
            for(int x = 0; x < img.width(); x++)
            {
                rotatedImage.setPixel(img.height() - y - 1, x, img.pixel(x,y));
            }
        }
    }else
    {
        for(int y = 0; y < img.height(); y++)
        {
            for(int x = 0; x < img.width(); x++)
            {
                rotatedImage.setPixel(y, img.width() - x - 1, img.pixel(x,y));
            }
        }
    }

    // Update the original image in place
    img = rotatedImage;
}

#endif // FUNCTIONS_H
