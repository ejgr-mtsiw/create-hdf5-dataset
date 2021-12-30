/*
 ============================================================================
 Name        : clargs.h
 Author      : Eduardo Ribeiro
 Version     :
 Description : Structures and functions to manage command line arguments
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef __CL_ARGS_INCLUDE__
#define __CL_ARGS_INCLUDE__

/**
 * Number of classes to generate by default
 */
#define N_CLASSES_DEFAULT 2

/**
 * Number of attributes to generate by default
 */
#define N_ATTRIBUTES_DEFAULT 1000000
/**
 * Number of observations to generate by default
 */
#define N_OBSERVATIONS_DEFAULT 2000

/**
 * Probability that an attribute is set to '1' [0 ~ 100]
 */
#define PROBABILITY_ATTRIBUTE_SET 26//%

/**
 * Compress the dataset?
 */
#define COMPRESS_DATASET 0

/**
 * Dataset compression level
 */
#define ZLIB_COMPRESSION_LEVEL 6

/**
 * Do not edit
 */
#define READ_CL_ARGS_OK 0
#define READ_CL_ARGS_NOK 1

#define DONT_USE_COMPRESSION 0
#define USE_COMPRESSION 1

/**
 * Structure to store command line options
 */
typedef struct {
	/**
	 * The name of the file to store the dataset
	 */
	char *filename;

	/**
	 * The dataset identifier
	 */
	char *datasetname;

	/**
	 * Number of classes to generate
	 */
	int n_classes;

	/**
	 * Number of attributes (columns) to generate.
	 */
	unsigned long n_attributes;

	/**
	 * Number of observations (lines) to generate.
	 */
	unsigned long n_observations;

	/**
	 * Probability that an attribute is set to '1'
	 */
	unsigned char probability_attribute_set;

	/**
	 * Compress the dataset?
	 */
	unsigned char compress_dataset;

	/**
	 * Dataset compression level
	 */
	unsigned char compression_level;
} clargs;

/**
 * Reads the arguments from the command line and stores
 * them in the args structure
 */
int read_args(int argc, char **argv, clargs *args);

#endif
