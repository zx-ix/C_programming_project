// Function for creating a .tsv data file of known images
// Created By : Puah Zi Xu
// Created Date: 26th April 2023
// version ='1.0'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "2071_files.h"
#include "2071_hash.h"
#include "2071_image.h"
#include "stats.h"
#include "transformations.h"

#define NUMBER_OF_TRANSFORMATION_TYPES 8

int main(int argc, char *argv[])
{
	// Remove false memory leak count by 1 due to calling printf() function
	setvbuf(stdout, NULL, _IONBF, 0);

	if (argc != 2)
	{
		printf("Please include the location of the “known” image directory as a command-line argument.\n");
		return 1;
	}

	// Retrieve an array of strings containing the names of known image files from the directory storing the image flies
	int numberOfFiles;
	char **filenames = query_filenames(argv[1], &numberOfFiles);
	if (filenames == NULL)
	{
		printf("Error using query_filenames function.\n");
		return 2;
	}

	// Print the label of the database into the .tsv file
	printf("file\tmean\tvariance\thash\thash(rotate_90_anticlockwise)\thash(rotate_90_clockwise)\thash(rotate_180)\thash(flip)\thash(mirror)\thash(rotate_90_anticlockwise+mirror)\thash(rotate_90_clockwise+mirror)\n");

	// Loop through the array of strings containing the names of known image files
	for (int fileIndex = 0; fileIndex < numberOfFiles; fileIndex++)
	{
		if (filenames[fileIndex] == NULL)
		{
			printf("Error getting image filename.\n");
			return 3;
		}

		// For each known image filename, retrieve the image data from the directory storing the image files
		Image myImage = read_image_from_file(argv[1], filenames[fileIndex]);
		int dataLength = myImage.height * myImage.width;

		float mean = compute_mean(myImage.data, dataLength);
		float variance = compute_variance(myImage.data, dataLength);

		// For each image file, print the filename, the mean and variance of the pixel values into the .tsv file
		printf("%s\t%f\t%f\t", filenames[fileIndex], mean, variance);

		// Loop through all the possible transformation types that can be applied to an image
		// There are only 8 possible transformation types (including no transformation)
		// This is because other combined transformations can be resolved to one of the 8 transformation types
		for (int i = 0; i < NUMBER_OF_TRANSFORMATION_TYPES; i++)
		{
			// Copy the image data into a temporary struct to which each transformation is applied
			Image tempImage = copy_image(&myImage);

			// Apply one possible transformation type for each loop iteration
			switch (i)
			{
				case 0:
				{
					// No transformation is applied
					break;
				}
				case 1:
				{
					rotate_90_anticlockwise(&tempImage);
					break;
				}
				case 2:
				{
					rotate_90_clockwise(&tempImage);
					break;
				}
				case 3:
				{
					rotate_180(&tempImage);
					break;
				}
				case 4:
				{
					flip(&tempImage);
					break;
				}
				case 5:
				{
					mirror(&tempImage);
					break;
				}
				case 6:
				{
					// This is the same as rotating the image by 90 degrees clockwise and then flip the image
					rotate_90_anticlockwise(&tempImage);
					mirror(&tempImage);
					break;
				}
				case 7:
				{
					// This is the same as rotating the image by 90 degrees anticlockwise and then mirror the image
					// This is also the same as transposing the image
					rotate_90_clockwise(&tempImage);
					mirror(&tempImage);
					break;
				}
			}

			// For each possible transformation types on an image file, create a unique hash string
			char *hash = hash_sha_256_image(&tempImage);
			if (hash == NULL)
			{
				printf("Error using hash_sha_256_image function.\n");
				return 4;
			}

			// For each possible transformation types on an image file, print the hash string into the .tsv file
			printf("%s", hash);

			if (i == NUMBER_OF_TRANSFORMATION_TYPES - 1)
			{
				printf("\n");
			}
			else
			{
				printf("\t");
			}

			free(hash);
			free(tempImage.data);
		}

		free(myImage.data);
		free(filenames[fileIndex]);
	}

	free(filenames);

	return 0;
}