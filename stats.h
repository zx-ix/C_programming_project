// Functions for computing statistics on arrays of data
// Created By : Puah Zi Xu
// Created Date: 24th April 2023
// version ='1.0'

float compute_mean(int *data, int dataLength);
float compute_variance(int *data, int dataLength);

/**
 * Calculates the mean of the pixel values of an image
 * @param data An array of pixel data
 * @param dataLength The length of the array of pixel data
 * @return A float containing the mean of the pixel values of an image
 * Example Usage:
 *     int pixels = [1, 2, 3, 4, 5];
 *     int number_of_pixels = 5;
 *     float mean = compute_mean(pixels, number_of_pixels);
 */
float compute_mean(int *data, int dataLength)
{
    int sum = 0;
    float mean = 0.0f;

    if (dataLength != 0)
    {
        for (int i = 0; i < dataLength; i++)
        {
            sum += data[i];
        }

        mean = (float)sum / dataLength;
    }

    return mean;
}

/**
 * Calculates the variance of the pixel values of an image
 * @param data An array of pixel data
 * @param dataLength The length of the array of pixel data
 * @return A float containing the variance of the pixel values of an image
 * Example Usage:
 *     int pixels = [1, 2, 3, 4, 5];
 *     int number_of_pixels = 5;
 *     float variance = compute_variance(pixels, number_of_pixels);
 */
float compute_variance(int *data, int dataLength)
{
    int sum = 0;
    float mean = 0.0f;
    float deviationSquaredSum = 0.0f; // the sum of the square of the deviation of each pixel value from the mean
    float variance = 0.0f;

    if (dataLength != 0)
    {
        // getting mean
        mean = compute_mean(data, dataLength);

        // calculating variance
        for (int i = 0; i < dataLength; i++)
        {
            deviationSquaredSum += ((float)data[i] - mean) * ((float)data[i] - mean);
        }

        variance = deviationSquaredSum / dataLength;
    }

    return variance;
}