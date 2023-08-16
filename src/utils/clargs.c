/*
 ============================================================================
 Name        : utils/clargs.c
 Author      : Eduardo Ribeiro
 Description : Structures and functions to manage command line arguments
 ============================================================================
 */

#include "utils/clargs.h"

#include "utils/cargs.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int read_args(int argc, char** argv, clargs_t* args)
{
	char identifier;
	const char* value = NULL;
	char* end = NULL;
	cag_option_context context;

	// Set defaults
	args->filename = NULL;
	args->datasetname = NULL;
	args->n_classes = N_CLASSES_DEFAULT;
	args->n_attributes = N_ATTRIBUTES_DEFAULT;
	args->n_observations = N_OBSERVATIONS_DEFAULT;
	args->probability_attribute_set = PROBABILITY_ATTRIBUTE_SET;
	args->n_inconsistencies = N_INCONSISTENCIES_DEFAULT;
	args->n_duplicates = N_DUPLICATES_DEFAULT;
	args->compress_dataset = COMPRESS_DATASET;
	args->compression_level = ZLIB_COMPRESSION_LEVEL;

	/**
	 * This is the main configuration of all options available.
	 */
	cag_option options[]
		= { { .identifier = 'f',
			  .access_letters = "f",
			  .access_name = NULL,
			  .value_name = "filename",
			  .description = "HDF5 dataset filename" },

			{ .identifier = 'd',
			  .access_letters = "d",
			  .access_name = NULL,
			  .value_name = "dataset",
			  .description = "Dataset identifier" },

			{ .identifier = 'c',
			  .access_letters = "c",
			  .access_name = NULL,
			  .value_name = "classes",
			  .description = "Number of classes" },

			{ .identifier = 'a',
			  .access_letters = "a",
			  .access_name = NULL,
			  .value_name = "attributes",
			  .description = "Number of attributes" },

			{ .identifier = 'o',
			  .access_letters = "o",
			  .access_name = NULL,
			  .value_name = "observations",
			  .description = "Number of observations" },

			{ .identifier = 'p',
			  .access_letters = "p",
			  .access_name = NULL,
			  .value_name = "probability",
			  .description = "Probability of a bit get set (0...100)" },

			{ .identifier = 'i',
			  .access_letters = "i",
			  .access_name = NULL,
			  .value_name = "inconsistencies",
			  .description = "Number of inconsistencies" },

			{ .identifier = 'u',
			  .access_letters = "u",
			  .access_name = NULL,
			  .value_name = "duplicates",
			  .description = "Number of duplicates" },

			{ .identifier = 'z',
			  .access_letters = "z",
			  .access_name = NULL,
			  .value_name = "compression",
			  .description = "Compression level (0...9)" },

			{ .identifier = 'h',
			  .access_letters = "h",
			  .access_name = "help",
			  .description = "Shows the command help"

			} };

	/**
	 * Now we just prepare the context and iterate over all options. Simple!
	 */
	cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
	while (cag_option_fetch(&context)) {
		identifier = cag_option_get(&context);
		switch (identifier) {
		case 'f':
			value = cag_option_get_value(&context);
			args->filename = value;
			break;
		case 'd':
			value = cag_option_get_value(&context);
			args->datasetname = value;
			break;
		case 'c':
			value = cag_option_get_value(&context);
			args->n_classes = strtol(value, &end, 10);
			break;
		case 'a':
			value = cag_option_get_value(&context);
			args->n_attributes = strtol(value, &end, 10);
			break;
		case 'o':
			value = cag_option_get_value(&context);
			args->n_observations = strtol(value, &end, 10);
			break;
		case 'p':
			value = cag_option_get_value(&context);
			args->probability_attribute_set = strtol(value, &end, 10);
			break;
		case 'i':
			value = cag_option_get_value(&context);
			args->n_inconsistencies = strtol(value, &end, 10);
			break;
		case 'u':
			value = cag_option_get_value(&context);
			args->n_duplicates = strtol(value, &end, 10);
			break;
		case 'z':
			value = cag_option_get_value(&context);
			args->compress_dataset = USE_COMPRESSION;
			args->compression_level = strtol(value, &end, 10);
			break;
		case 'h':
			printf("Usage: %s [OPTION]...\n", argv[0]);
			cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
			return READ_CL_ARGS_NOK;
		}
	}

	if (args->filename == NULL || args->datasetname == NULL
		|| args->n_attributes < 2 || args->n_observations < 2
		|| args->n_classes < 2) {
		printf("Usage: %s [OPTION]...\n", argv[0]);
		cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
		return READ_CL_ARGS_NOK;
	}

	return READ_CL_ARGS_OK;
}
