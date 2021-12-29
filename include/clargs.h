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
	unsigned long int n_attributes;

	/**
	 * Number of observations (lines) to generate.
	 */
	unsigned long int n_observations;

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

int read_args(int argc, char **argv, clargs *args) {
	int c = 0, idata = 0;
	unsigned long int uidata = 0;

	opterr = 0;

	// Set defaults
	args->filename = NULL;
	args->datasetname = NULL;
	args->n_classes = N_CLASSES_DEFAULT;
	args->n_attributes = N_ATTRIBUTES_DEFAULT;
	args->n_observations = N_OBSERVATIONS_DEFAULT;
	args->probability_attribute_set = PROBABILITY_ATTRIBUTE_SET;
	args->compress_dataset = COMPRESS_DATASET;
	args->compression_level = ZLIB_COMPRESSION_LEVEL;

	while ((c = getopt(argc, argv, "f:d:c:a:o:p:z:")) != -1) {
		switch (c) {
		case 'f':
			args->filename = optarg;
			break;
		case 'd':
			args->datasetname = optarg;
			break;
		case 'c':
			idata = atoi(optarg);
			if (idata > 2) {
				args->n_classes = idata;
			}
			break;
		case 'a':
			uidata = strtoul(optarg, NULL, 10);
			args->n_attributes = uidata;
			break;
		case 'o':
			uidata = strtoul(optarg, NULL, 10);
			args->n_observations = uidata;
			break;
		case 'p':
			idata = atoi(optarg);
			if (idata >= 0 && idata <= 100) {
				args->probability_attribute_set = idata;
			}
			break;
		case 'z':
			idata = atoi(optarg);
			if (idata > 0 && idata < 10) {
				args->compress_dataset = USE_COMPRESSION;
				args->compression_level = idata;
			} else {
				args->compress_dataset = DONT_USE_COMPRESSION;
			}
			break;
		case '?':
			if (optopt == 'f') {
				fprintf(stderr, "Must set output filename (-f).\n");
				return READ_CL_ARGS_NOK;
			}

			if (optopt == 'd') {
				fprintf(stderr, "Must set dataset identifier (-d).\n");
				return READ_CL_ARGS_NOK;
			}

			if (optopt == 'z') {
				// Compress using default compression level
				args->compress_dataset = USE_COMPRESSION;
			}
			break;

		default:
			abort();
		}
	}

	if (args->filename == NULL || args->datasetname == NULL) {
		fprintf(stdout, "Usage:\n %s -f <filename> -d <dataset>\n", argv[0]);
		fprintf(stdout, "Optional parameters:\n");
		fprintf(stdout, "-c n_classes Default: 2\n");
		fprintf(stdout, "-a n_attributes Default: 1000000\n");
		fprintf(stdout, "-o n_observations Default: 2000\n");
		fprintf(stdout, "-p probability_attribute_set Default: 26\n");
		fprintf(stdout, "-z compress_dataset_level Default: 6, 0 disables compression\n");
		return READ_CL_ARGS_NOK;
	}

	return READ_CL_ARGS_OK;
}

#endif
