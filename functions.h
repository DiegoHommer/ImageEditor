#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <bits/stdc++.h>
#include <iostream>
#include <QImage>

using namespace std;

void generate_gray_img(QImage &img)
{
    for(int y = 0; y < img.height(); y++)
    {
        for(int x = 0; x < img.width(); x++)
        {
            QColor color = img.pixelColor(x, y);
            int grey = (0.299 * color.red())+(0.587 * color.green()) + (0.114 * color.blue());
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

        case 1:
            for(int y = 0; y < h_temp; y += 1)
            {
                for(int x = 0; x < w_temp; x += 1)
                {
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
            int grayscaleValue = qRed(img.pixel(x,y));
            if(grayscaleValue < min_shade) min_shade = grayscaleValue;
            if(grayscaleValue > max_shade) max_shade = grayscaleValue;
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
                    auto it = upper_bound(bins.begin(), bins.end(), img.pixelColor(x,y).red()); // Looks for the bin correspondent to the original image shade
                    center = std::clamp(static_cast<int>((*(it - 1) + *(it)) / 2), min_shade, max_shade);;  // Saves the closest integer to the center of the bin
                    img.setPixel(x,y,qRgb(center,center,center)); // Shade of pixel in new image = center integer
                }
            }
    }
}

#endif // FUNCTIONS_H
