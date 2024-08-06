// Functions for searching unknown images using a dataset of known images
// Created By : Puah Zi Xu
// Created Date: 27th April 2023
// version ='1.0'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "2071_files.h"
#include "2071_hash.h"
#include "2071_image.h"
#include "stats.h"
#include "transformations.h"

#define HASH_STRING_LENGTH sha256_desc.hashsize * 2
#define NUMBER_OF_FILENAME_CHARACTERS 100
#define NUMBER_OF_TRANSFORMATION_TYPES 8

// A binary search tree node
typedef struct fileImage
{
	char *filename;
	float mean;
	float variance;
	char *hashes[NUMBER_OF_TRANSFORMATION_TYPES];
	struct fileImage *leftPtr;
	struct fileImage *rightPtr;
} fileImage;

void insert_node(fileImage **treePtr, char *tempFilename, float tempMean, float tempVariance, char *tempHashes[]);
void inorder_search(fileImage *treePtr, char *unknownHash, char *unknownFilename);
void postorder_delete_node(fileImage **treePtr);

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Please include the location of the “unknown” image directory, and the location of the database file as command-line arguments.\n");
		return 1;
	}

	// Create a rootPtr to a binary search tree
	fileImage *rootPtr = NULL;

	// Temporary variables for storing the values scanned from each line of the .tsv file using fscanf
	char tempFilename[NUMBER_OF_FILENAME_CHARACTERS];
	float tempMean = 0.0f;
	float tempVariance = 0.0f;
	char *tempHashes[NUMBER_OF_TRANSFORMATION_TYPES];
	for (int i = 0; i < NUMBER_OF_TRANSFORMATION_TYPES; i++)
	{
		tempHashes[i] = (char *)malloc((HASH_STRING_LENGTH + 1) * sizeof(char));
	}

	// Open the .tsv file and read each line
	FILE *filePtr = fopen(argv[2], "r");
	if (filePtr == NULL)
	{
		printf("Error opening %s file.\n", argv[2]);
		return 2;
	}

	// Scan and discard the label on the first line
	fscanf(filePtr, "%*[^\n]");

	// Scan the remaining lines
	while (fscanf(filePtr, "%s	%f	%f	%s	%s	%s	%s	%s	%s	%s	%s", tempFilename, &tempMean, &tempVariance, tempHashes[0], tempHashes[1], tempHashes[2], tempHashes[3], tempHashes[4], tempHashes[5], tempHashes[6], tempHashes[7]) != EOF)
	{
		// Create a node for the binary search tree and insert the values into the node
		insert_node(&rootPtr, tempFilename, tempMean, tempVariance, tempHashes);
	}

	for (int i = 0; i < NUMBER_OF_TRANSFORMATION_TYPES; i++)
	{
		free(tempHashes[i]);
	}

	// Close the .tsv file
	fclose(filePtr);

	// Retrieve an array of strings containing the names of unknown image files from the directory storing the image flies
	int numberOfFiles;
	char **filenames = query_filenames(argv[1], &numberOfFiles);
	if (filenames == NULL)
	{
		printf("Error using query_filenames function.\n");
		return 3;
	}

	// Print the label onto the console window
	printf("unknown\tknown\n");

	// Loop through the array of strings containing the names of unknown image files
	for (int fileIndex = 0; fileIndex < numberOfFiles; fileIndex++)
	{
		if (filenames[fileIndex] == NULL)
		{
			printf("Error getting image filename.\n");
			return 4;
		}

		// For each unknown image filename, retrieve the image data from the directory storing the image files
		Image myImage = read_image_from_file(argv[1], filenames[fileIndex]);

		// For each unknown image filename, create a unique hash string
		char *myHash = hash_sha_256_image(&myImage);
		if (myHash == NULL)
		{
			printf("Error using hash_sha_256_image function.\n");
			return 5;
		}

		// For each hash string of the unknown image filename, search to find a matching hash string from the known images
		inorder_search(rootPtr, myHash, filenames[fileIndex]);

		free(myHash);
		free(myImage.data);
		free(filenames[fileIndex]);
	}
	free(filenames);

	// Destroy the binary search tree once the search is done to prevent memory leak
	postorder_delete_node(&rootPtr);

	return 0;
}

/**
 * Creates the root node of a binary search tree (if there is none) or insert a child node into the binary search tree (if there is one)
 * Each node contains the information associated to each image, read from the dataset of known images
 * @param treePtr A pointer to a pointer to the root node (double pointer)
 * @param tempFilename A string containing the image filename, to be added to a node
 * @param tempMean A float containing the mean of the pixel values of the image, to be added to a node
 * @param tempVariance A float containing the variance of the pixel values of the image, to be added to a node
 * @param tempHashes An array of string containing the hashes for all the 8 possible transformation types, to be added to a node
 * Example Usage:
 *     const int HASH_STRING_LENGTH = 64;
 *     const int NUMBER_OF_TRANSFORMATION_TYPES = 8;
 *     fileImage *rootPtr = NULL;
 *     const char *temp_filename = "test_image.png";
 *     Image my_image = read_image_from_file("my_directory", temp_filename);
 *     int data_length = my_image.height * my_image.width;
 *     float temp_mean = compute_mean(my_image.data, data_length);
 *     float temp_variance = compute_variance(my_image.data, data_length);
 *     char *hash = hash_sha_256_image(&my_image);
 *     char *temp_hashes[NUMBER_OF_TRANSFORMATION_TYPES];
 *     for (int i = 0; i < NUMBER_OF_TRANSFORMATION_TYPES; i++)
 *     {
 * 			temp_hashes[i] = (char *)malloc((HASH_STRING_LENGTH + 1) * sizeof(char));
 *			strcpy(temp_hashes[i], hash);
 *     }
 *     insert_node(&rootPtr, temp_filename, temp_mean, temp_variance, temp_hashes);
 */
void insert_node(fileImage **treePtr, char *tempFilename, float tempMean, float tempVariance, char *tempHashes[])
{
	if ((*treePtr) == NULL)
	{
		// Create a node
		*treePtr = (fileImage *)malloc(sizeof(fileImage));
		if ((*treePtr) == NULL)
		{
			printf("Error allocating memory for the first node in the binary search tree.\n");
			return;
		}

		// Store all the information associated to each image into the node
		(*treePtr)->filename = (char *)malloc(strlen(tempFilename + 1));
		if ((*treePtr)->filename == NULL)
		{
			printf("Error allocating memory for ImageFile filename.\n");
			return;
		}
		strcpy((*treePtr)->filename, tempFilename);

		(*treePtr)->mean = tempMean;
		(*treePtr)->variance = tempVariance;

		for (int i = 0; i < NUMBER_OF_TRANSFORMATION_TYPES; i++)
		{
			(*treePtr)->hashes[i] = (char *)malloc(strlen(tempHashes[i] + 1));
			if ((*treePtr)->hashes[i] == NULL)
			{
				printf("Error allocating memory for ImageFile hash.\n");
				return;
			}
			strcpy((*treePtr)->hashes[i], tempHashes[i]);
		}

		(*treePtr)->leftPtr = NULL;
		(*treePtr)->rightPtr = NULL;
	}
	else
	{
		int i = 0;
		while (i < HASH_STRING_LENGTH)
		{
			// If the ASCII value of the i-th character of the first hash (no transformation applied) of the incoming image is smaller than that of the current node
			if (tempHashes[0][i] < (*treePtr)->hashes[0][i])
			{
				insert_node(&((*treePtr)->leftPtr), tempFilename, tempMean, tempVariance, tempHashes);
				break;
			}
			// Else if the ASCII value of the i-th character of the first hash (no transformation applied) of the incoming image is greater than that of the current node
			else if (tempHashes[0][i] > (*treePtr)->hashes[0][i])
			{
				insert_node(&((*treePtr)->rightPtr), tempFilename, tempMean, tempVariance, tempHashes);
				break;
			}
			// Else if the ASCII value of the i-th character of the first hash (no transformation applied) of the incoming image is equal to that of the current node
			else
			{
				// Compare the (i + 1)-th character instead
				i++;
			}
		}
	}

	return;
}

/**
 * Traverses the binary search tree in an in-order fashion to look for a matching hash
 * @param treePtr A pointer to the root node
 * @param unknownHash A string containing the hash of the unknown image for matching
 * @param unknownFilename A string containing the name of the unknown image, to be printed onto the console if there is a match
 * Example Usage:
 *     fileImage *rootPtr; // Assume that it has pointed to a binary search tree already
 *     const char *search_filename = "test_image.png";
 *     Image search_image = read_image_from_file("my_directory", search_filename);
 *     char *search_hash = hash_sha_256_image(&search_image);
 *     inorder_search(rootPtr, search_hash, search_filename);
 */
void inorder_search(fileImage *treePtr, char *unknownHash, char *unknownFilename)
{
	if (treePtr != NULL)
	{
		inorder_search(treePtr->leftPtr, unknownHash, unknownFilename);

		// Loop through all the possible transformation types that can be applied to an image
		for (int i = 0; i < NUMBER_OF_TRANSFORMATION_TYPES; i++)
		{
			if (strcmp(treePtr->hashes[i], unknownHash) == 0)
			{
				printf("%s\t%s", unknownFilename, treePtr->filename);

				switch (i)
				{
					case 0:
					{
						printf("\n");
						break;
					}
					case 1:
					{
						printf("(rotate_90_anticlockwise)\n");
						break;
					}
					case 2:
					{
						printf("(rotate_90_clockwise)\n");
						break;
					}
					case 3:
					{
						printf("(rotate_180)\n");
						break;
					}
					case 4:
					{
						printf("(flip)\n");
						break;
					}
					case 5:
					{
						printf("(mirror)\n");
						break;
					}
					case 6:
					{
						printf("(rotate_90_anticlockwise+mirror)\n");
						break;
					}
					case 7:
					{
						printf("(rotate_90_clockwise+mirror)\n");
						break;
					}
				}
			}
		}

		inorder_search(treePtr->rightPtr, unknownHash, unknownFilename);
	}

	return;
}

/**
 * Traverses the binary search tree in an post-order fashion to look delete each node in the binary search tree
 * @param treePtr A pointer to a pointer to the root node (double pointer)
 * Example Usage:
 *     fileImage *rootPtr; // Assume that it has pointed to a binary search tree already
 *     postorder_delete_node(&rootPtr);
 */
void postorder_delete_node(fileImage **treePtr)
{
	if ((*treePtr) != NULL)
	{
		postorder_delete_node(&((*treePtr)->leftPtr));
		postorder_delete_node(&((*treePtr)->rightPtr));

		free((*treePtr)->filename);
		for (int i = 0; i < NUMBER_OF_TRANSFORMATION_TYPES; i++)
		{
			free((*treePtr)->hashes[i]);
		}
		free((*treePtr));
	}

	return;
}