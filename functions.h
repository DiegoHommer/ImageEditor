#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <bits/stdc++.h>
#include <iostream>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QMessageBox>

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

bool check_greyscale(QImage &img)
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
                return false;
            }
        }
    }
    return true;
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
    float scaling_factor = 255.0 / *std::max_element(histogram.begin(), histogram.end());

    // Histogram normalization
    for(auto it = histogram.begin(); it != histogram.end(); it++)
    {
            // Multiplies each histogram column by scaling factor for visualization
            *it = static_cast<int>(*it * scaling_factor);
    }

    // Declare the PixelMap that will serve as the "canvas"
    QPixmap hist_map(255, 255);
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

void generate_histogram(QImage &img, vector<int>& histogram, bool isGrey)
{
    QImage copyImage = img;
    if(!isGrey)
    {
        generate_grey_img(copyImage);
    }

    // Histogram computation
    for(int y = 0; y < copyImage.height(); y++)
    {
            for(int x = 0; x < copyImage.width(); x++)
            {
                 // Gets the Rgb colors from original pixel
                QColor color = copyImage.pixelColor(x, y);

                // Adds 1 to the corresponding shade collumn of the histogram
                histogram[color.red()]++;
            }
    }
}

void equalize_histogram(QImage &img, QLabel &hist_window_before, QLabel &hist_window_after, bool isGrey)
{
    vector<int> original_histogram(256,0);
    vector<int> equalized_histogram(256,0);
    vector<int> cumulative_histogram(256,0);
    float scaling_factor = 255.0 / (img.width() * img.height());

    generate_histogram(img, original_histogram, isGrey);

    // Compute the cumulative histogram (using static_cast to avoid precision errors)
    cumulative_histogram[0] = static_cast<double>(original_histogram[0]);
    for (int i = 1; i < 256; i++)
    {
        cumulative_histogram[i] = cumulative_histogram[i - 1] + static_cast<double>(original_histogram[i]);
    }

    for(int y = 0; y < img.height(); y++)
    {
        for(int x = 0; x < img.width(); x++)
        {
                // Get the original pixel shade
                QColor color = img.pixelColor(x,y);

                // Map pixel values based on the normalized cumulative histogram (multiplying with scaling_factor to normalize)
                int red = static_cast<int>(cumulative_histogram[color.red()] * scaling_factor);
                int green = static_cast<int>(cumulative_histogram[color.green()] * scaling_factor);
                int blue = static_cast<int>(cumulative_histogram[color.blue()] * scaling_factor);

                // Ensure that mapped values are in the valid range [0, 255]
                red = std::clamp(red, 0, 255);
                green = std::clamp(green, 0, 255);
                blue = std::clamp(blue, 0, 255);

                // Set the equalized pixel color on the image
                img.setPixelColor(x,y,QColor(red,green,blue));
        }
    }


    if(isGrey)
    {
        // If the input image is greyscale, display the histogram of the image before and after the equalization
        generate_histogram(img,equalized_histogram,true);
        display_histogram(original_histogram, hist_window_before, "Histogram before equalization");
        display_histogram(equalized_histogram, hist_window_after, "Histogram after equalization");
    }
}

QColor average_color(QImage &img, int x_start, int y_start, int x_end, int y_end, int num_pixels)
{
    int average_red = 0;
    int average_blue = 0;
    int average_green = 0;

    // Rectangle:
    //  (x_start, y_start) ... (x_end, y_start)
    //          .                     .
    //          .                     .
    //  (x_start, y_end)  ... (x_end, y_end)

    // Loop calculates the sum of intensities (each channel) from pixels inside the rectangle
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
    QColor new_pixel_color;
    new_pixel_color.setRgb(average_red, average_green, average_blue);
    return new_pixel_color;
}

void zoomOut(QImage &img, int Sx, int Sy)
{
    int newHeight = static_cast<int>(floor(img.height() / Sy));
    int newWidth = static_cast<int>(floor(img.width() / Sx));
    int num_pixels = Sx * Sy;

    QImage zoomedImage(newWidth, newHeight, img.format());

    for(int y = 0; y < newHeight; y++)
    {
        for(int x = 0; x < newWidth; x++)
        {
            // Determines the coordinates (position) of where the factor rectangle will be applied
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
    // Calculate the interpolation of two pixels (average between pixel intensity on each channel)
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

    QImage zoomedImage(newWidth, newHeight, img.format());

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
    QImage rotatedImage(img.height(), img.width(), img.format());

    if(clockwise)
    {
        for(int y = 0; y < img.height(); y++)
        {
            for(int x = 0; x < img.width(); x++)
            {
                // First row (left to right) goes to last column (top to bottom) and so on...
                rotatedImage.setPixel(img.height() - y - 1, x, img.pixel(x,y));
            }
        }
    }else
    {
        for(int y = 0; y < img.height(); y++)
        {
            for(int x = 0; x < img.width(); x++)
            {
                // First row (left to right) goes to first column (bottom to top) and so on...
                rotatedImage.setPixel(y, img.width() - x - 1, img.pixel(x,y));
            }
        }
    }

    // Update the original image in place
    img = rotatedImage;
}

QColor convolve_pixel(QImage &img, int x_start, int y_start, int x_end, int y_end, vector<double> kernel, bool embossing)
{
    int kernel_red = 0;
    int kernel_green = 0;
    int kernel_blue = 0;
    int kernel_index = 0;

    // Loop for multyplying each pixel (inside the dimensions) intensity  with it's respective kernel value
    for(int y = y_start; y < y_end; y++)
    {
        for(int x = x_start; x < x_end; x++)
        {
            QColor color = img.pixelColor(x, y);
            kernel_red += kernel[kernel_index] * color.red();
            kernel_green += kernel[kernel_index] * color.green();
            kernel_blue += kernel[kernel_index] * color.blue();
            kernel_index++;
        }
    }

    // If embossing (greybackground) was enabled, add 127 before clamping the concolved pixel value
    if(embossing)
    {
        kernel_red += 127;
        kernel_green += 127;
        kernel_blue += 127;
    }
    kernel_red = std::clamp(static_cast<int>(kernel_red), 0, 255);
    kernel_green = std::clamp(static_cast<int>(kernel_green), 0, 255);
    kernel_blue = std::clamp(static_cast<int>(kernel_blue), 0, 255);

    return QColor(kernel_red ,  kernel_green,  kernel_blue);
}

void convolve(QImage &img, vector<double> kernel, bool embossing)
{
    int height = img.height();
    int width = img.width();
    QImage outputImage(width, height, img.format());

    for(int y = 1; y < height - 1; y++)
    {
        for(int x = 1; x < width - 1; x++)
        {
            // Determine the coordinates (location) of the 3x3 rectangle where the kernel will be applied
            int x_start = x - 1;
            int y_start = y - 1;
            int x_end = x + 2;
            int y_end = y + 2;

            // Calculate the concolved pixel and position it on output image
            QColor color = convolve_pixel(img, x_start, y_start, x_end, y_end, kernel, embossing);
            outputImage.setPixelColor(x, y, color);
        }
    }

    // Update the original image in place
    img = outputImage;
}

void histogram_matching(QImage &img_src, QImage &img_target, QLabel &hist_window_before, QLabel &hist_window_after)
{
    vector<int> src_histogram(256,0);
    vector<int> target_histogram(256,0);
    vector<int> src_cumulative_histogram(256,0);
    vector<int> target_cumulative_histogram(256,0);
    vector<int> HM(256,0);
    float scaling_factor_src = 255.0 / (img_src.width() * img_src.height());
    float scaling_factor_target = 255.0 / (img_target.width() * img_target.height());

    // Compute source and target histograms
    generate_histogram(img_src, src_histogram, true);
    generate_histogram(img_target, target_histogram, true);

    // Compute source and target cumulative histograms
    src_cumulative_histogram[0] = static_cast<double>(src_histogram[0]);
    target_cumulative_histogram[0] =  static_cast<double>(target_histogram[0]);
    for(int i = 1; i < 256; i++)
    {
        src_cumulative_histogram[i] = src_cumulative_histogram[i - 1] + static_cast<double>(src_histogram[i]);
        target_cumulative_histogram[i] = target_cumulative_histogram[i - 1] + static_cast<double>(target_histogram[i]);
    }

    // Normalize source and target cumulative histograms (apart from original loop to avoid precision erros)
    for(int i = 0; i < 256; i++)
    {
        src_cumulative_histogram[i] = static_cast<int>(src_cumulative_histogram[i] * scaling_factor_src);
        target_cumulative_histogram[i] = static_cast<int>(target_cumulative_histogram[i] * scaling_factor_target);
    }

    // Compute Matching Histogram (HM)
    for(int shade_level = 0; shade_level < 256; shade_level++)
    {
        int src_cumulative = src_cumulative_histogram[shade_level] ;
        int closest_target_shade = 0;
        int min_difference = INT_MAX;

        // Loop for looking for the closest target shade (relative to src_cumulative) in the target cumulative histogram
        for(int i = 0; i < 256; i++)
        {
            if(std::abs(target_cumulative_histogram[i] - src_cumulative) < min_difference)
            {
                min_difference = std::abs(target_cumulative_histogram[i] - src_cumulative);
                closest_target_shade = i;
            }
        }
        HM[shade_level] = closest_target_shade;
    }

    for(int y = 0; y < img_src.height(); y++)
    {
        for(int x = 0; x < img_src.width(); x++)
        {
            // Get the original pixel shade
            QColor color = img_src.pixelColor(x,y);

            // Map the original shade to the matching histogram shade
            int mapped_shade = HM[color.red()];

            // Set the matched pixel color on the image
            img_src.setPixelColor(x,y,QColor(mapped_shade, mapped_shade, mapped_shade));
        }
    }

    display_histogram(src_cumulative_histogram, hist_window_before, "Source Normalized Cumulative Histogram");
    display_histogram(target_cumulative_histogram, hist_window_after, "Target Normalized Cumulative Histogram");
}

#endif // FUNCTIONS_H
