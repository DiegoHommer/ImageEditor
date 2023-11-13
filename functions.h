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

void generate_histogram(QImage &img, QLabel &hist_window)
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
    hist_window.setWindowTitle("Edited Image Histogram");
    hist_window.setPixmap(hist_map.scaled(hist_window.height(), hist_window.width(), Qt::KeepAspectRatio));
    hist_window.show();


}
#endif // FUNCTIONS_H
