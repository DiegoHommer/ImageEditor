#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <bits/stdc++.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

void generate_gray_img(unsigned char *img, int width, int height, int channels)
{
    int img_size = width * height * channels;
    int gray_img_size = width * height; // Como temos apenas um canal, temos dimensões widthXheight

    unsigned char *gray_img = new unsigned char [gray_img_size];

    for(unsigned char *p_img = img, *p_gray_img = gray_img; p_img != img + img_size; p_img += channels, p_gray_img += 1)
    {
        // Aplicação da fórmula L = 0.299*R + 0.587*G + 0.114*B em cada pixel da nova imagem
        *p_gray_img = (0.299 * (*p_img)) + (0.587 * (*(p_img + 1))) + (0.114 * (*(p_img + 2)));
    }

    stbi_write_jpg("grama_cinza.jpg", width, height, 1, gray_img, 100);
    stbi_image_free(gray_img);

    return;
}

void mirror(unsigned char *img, int width, int height, int channels, bool option)
{
    int img_size = width * height * channels;

    unsigned char *inv_img = new unsigned char [img_size];
    if(option) // OPÇÃO: ESPELHAMENTO VERTICAL
    {
        // Loop percorre linha a linha das imagens (imagem original: 1º -> última | imagem nova: última -> 1º)
        for(unsigned char *p_img = img, *p_inv = inv_img + img_size - (width * channels); p_img != img + img_size; p_img += width * channels, p_inv -= width * channels)
        {
            //  (1° linha antiga -> última linha nova | 2º -> antepenúltima...)
            memcpy(p_inv, p_img, width * channels);
        }
    }else // OPÇÃO: ESPELHAMENTO HORIZONTAL
    {
        // Loop percorre linha a linha das imagens (ambas imagens: 1º -> última)
        for(unsigned char *p_img = img, *p_inv = inv_img; p_img != img + img_size; p_img += width * channels, p_inv += width * channels)
        {
            // Percorre pixel a pixel da forma que: (1º pixel da linha antiga -> último pixel da linha nova | 2º -> antepenúltimo...)
            for(unsigned char *pp_img = p_img, *pp_inv = ((p_inv + (width * channels)) - channels); pp_inv >= p_inv; pp_img += channels, pp_inv -= channels)
            {
                memcpy(pp_inv, pp_img, channels);
            }
        }
    }

    stbi_write_jpg("grama_inv.jpg", width, height, channels, inv_img, 100);
    stbi_image_free(inv_img);

    return;
}

void quantization(unsigned char* img, int width, int height, int n)
{
    int max_shade = INT_MIN, min_shade = INT_MAX;
    int img_size = width * height, tam_int;
    vector<float> bins;

    // Alocando espaço para nova imagem
    unsigned char *new_img = new unsigned char [img_size];

    // Procura pelo tom mais intenso e menos intenso da imagem
    for(unsigned char *p_img = img; p_img != img + img_size; p_img += 1)
    {
        if(*p_img > max_shade) max_shade = *p_img;
        if(*p_img < min_shade) min_shade = *p_img;
    }
    tam_int = max_shade - min_shade + 1;

    if(tam_int > n)
    {
        int i = 0, center;
        float tb = tam_int / n, upper;

        // Criação dos intervalos (bins) para quantização
        for(upper = min_shade - 0.5; i <= n + 1; i++, upper += tb)
        {
            bins.push_back(upper);
        }

        // Quantização da imagem
        for(unsigned char *p_img = img, *p_newimg = new_img; p_img != img + img_size; p_img += 1, p_newimg += 1)
        {
            auto it = upper_bound(bins.begin(), bins.end(), *p_img); // Procura bin do tom do pixel imagem original
            center = (*(it - 1) + *(it)) / 2;  // Grava o inteiro mais próximo do centro desse bin
            *p_newimg = (unsigned char)center; // Pixel correspondente da nova imagem = center
        }

        stbi_write_jpg("grama_quantizada.jpg", width, height, 1, new_img, 100);
    }
    stbi_image_free(new_img);
}

#endif // FUNCTIONS_H
