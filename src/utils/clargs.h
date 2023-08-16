/*
 ============================================================================
 Name        : clargs.h
 Author      : Eduardo Ribeiro
 Description : Structures and functions to manage command line arguments
 ============================================================================
 */

#ifndef CL_ARGS_H
#define CL_ARGS_H

/**
 * Do not edit
 */
#define READ_CL_ARGS_OK 0
#define READ_CL_ARGS_NOK 1

/**
 * Number of classes to generate by default
 */
#define N_CLASSES_DEFAULT 2

/**
 * Number of attributes to generate by default
 */
#define N_ATTRIBUTES_DEFAULT 10
/**
 * Number of observations to generate by default
 */
#define N_OBSERVATIONS_DEFAULT 20

/**
 * Probability that an attribute is set to '1' [0 ~ 100]
 */
#define PROBABILITY_ATTRIBUTE_SET 26 //%

/**
 * Number of inconsistencies to add by default
 */
#define N_INCONSISTENCIES_DEFAULT 2

/**
 * Number of duplicates to add by default
 */
#define N_DUPLICATES_DEFAULT 2

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
typedef struct clargs_t {
	/**
	 * The name of the file to store the dataset
	 */
	const char* filename;

	/**
	 * The dataset identifier
	 */
	const char* datasetname;

	/**
	 * Number of classes to generate
	 */
	unsigned long n_classes;

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
	 * Number of duplicates to add
	 */
	unsigned long n_duplicates;

	/**
	 * Number of inconsistencies to add
	 */
	unsigned long n_inconsistencies;

	/**
	 * Compress the dataset?
	 */
	unsigned char compress_dataset;

	/**
	 * Dataset compression level
	 */
	unsigned char compression_level;
} clargs_t;

/**
 * Reads the arguments from the command line and stores
 * them in the args structure
 */
int read_args(int argc, char** argv, clargs_t* args);

#endif
