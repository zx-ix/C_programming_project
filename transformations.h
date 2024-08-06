// Functions for transforming images
// Created By : Puah Zi Xu
// Created Date: 25th April 2023
// version ='1.0'

#include "2071_image.h"

void rotate_90_anticlockwise(Image *image);
void rotate_90_clockwise(Image *image);
void rotate_180(Image *image);
void flip(Image *image);
void mirror(Image *image);

/**
 * Rotates an image by 90 degrees anticlockwise
 * @param image Image struct containing pixel data
 * Example Usage:
 *     Image my_image = read_image_from_file("my_directory", "test_image.png");
 *     rotate_90_anticlockwise(&my_image);
 */
void rotate_90_anticlockwise(Image *image)
{
    int height = (image->height);
    int width = (image->width);

    // Transpose matrix
    if (height != 0 && width != 0)
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = i; j < width; j++)
            {
                int temp = image->data[(i * width) + j];
                image->data[(i * width) + j] = image->data[(j * width) + i];
                image->data[(j * width) + i] = temp;
            }
        }
    }

    flip(image);
    return;
}

/**
 * Rotates an image by 90 degrees clockwise
 * @param image Image struct containing pixel data
 * Example Usage:
 *     Image my_image = read_image_from_file("my_directory", "test_image.png");
 *     rotate_90_clockwise(&my_image);
 */
void rotate_90_clockwise(Image *image)
{
    int height = (image->height);
    int width = (image->width);

    // Transpose matrix
    if (height != 0 && width != 0)
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = i; j < width; j++)
            {
                int temp = image->data[(i * width) + j];
                image->data[(i * width) + j] = image->data[(j * width) + i];
                image->data[(j * width) + i] = temp;
            }
        }
    }

    mirror(image);
    return;
}

/**
 * Rotates an image by 180 degrees
 * @param image Image struct containing pixel data
 * Example Usage:
 *     Image my_image = read_image_from_file("my_directory", "test_image.png");
 *     rotate_180(&my_image);
 */
void rotate_180(Image *image)
{
    mirror(image);
    flip(image);
    return;
}

/**
 * Reverses the rows of an image
 * @param image Image struct containing pixel data
 * Example Usage:
 *     Image my_image = read_image_from_file("my_directory", "test_image.png");
 *     flip(&my_image);
 */
void flip(Image *image)
{
    int height = (image->height);
    int width = (image->width);

    if (height != 0 && width != 0)
    {
        for (int i = 0, midrow = height / 2; i < midrow; i++)
        {
            for (int j = 0; j < width; j++)
            {
                int temp = image->data[(i * width) + j];
                image->data[(i * width) + j] = image->data[((height - 1 - i) * width) + j];
                image->data[((height - 1 - i) * width) + j] = temp;
            }
        }
    }

    return;
}

/**
 * Reverses the columns of an image
 * @param image Image struct containing pixel data
 * Example Usage:
 *     Image my_image = read_image_from_file("my_directory", "test_image.png");
 *     flip(&my_image);
 */
void mirror(Image *image)
{
    int height = (image->height);
    int width = (image->width);

    if (height != 0 && width != 0)
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = 0, midcolumn = width / 2; j < midcolumn; j++)
            {
                int temp = image->data[(i * width) + j];
                image->data[(i * width) + j] = image->data[(i * width) + (width - 1 - j)];
                image->data[(i * width) + (width - 1 - j)] = temp;
            }
        }
    }

    return;
}