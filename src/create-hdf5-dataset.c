/*
 ============================================================================
 Name        : create-hdf5-dataset.c
 Author      : Eduardo Ribeiro
 Description : Creates HDF5 dataset from command line arguments.
 ============================================================================
 */

#include "dataset.h"
#include "dataset_hdf5.h"
#include "types/word_t.h"
#include "utils/bit.h"
#include "utils/clargs.h"

#include "hdf5.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 *
 */
int main(int argc, char** argv)
{

	/**
	 * Command line arguments set by the user
	 */
	clargs_t args;

	dataset_hdf5_t hdf5_dataset;

	dataset_t dataset;

	/**
	 * Buffer to store one line/chunk of data
	 */
	word_t* buffer = NULL;

	// Seed the random number generator
	// TODO: we could read the seed from the command line?
	srand((unsigned) time(NULL));

	/**
	 * Parse command line arguments
	 */
	if (read_args(argc, argv, &args) == READ_CL_ARGS_NOK) {
		return EXIT_FAILURE;
	}

	/**
	 * Create the data file
	 */
	hdf5_dataset.file_id
		= H5Fcreate(args.filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
	if (hdf5_dataset.file_id < 1) {
		// Error creating file
		fprintf(stdout, "Error creating %s\n", args.filename);
		return EXIT_FAILURE;
	}
	fprintf(stdout, " - Empty file created.\n");

	// Store data
	dataset.n_classes = args.n_classes;
	dataset.n_attributes = args.n_attributes;
	dataset.n_observations = args.n_observations;
	dataset.n_bits_for_class = (uint8_t) ceil(log2(dataset.n_classes));
	dataset.n_bits_for_jnsqs = 0;

	uint32_t total_bits = dataset.n_attributes + dataset.n_bits_for_class;
	uint32_t n_words = total_bits / WORD_BITS + (total_bits % WORD_BITS != 0);

	dataset.n_words = n_words;

	hdf5_dataset.dataset_id = hdf5_create_dataset(
		hdf5_dataset.file_id, args.datasetname, dataset.n_observations,
		dataset.n_words, H5T_NATIVE_UINT64);
	hdf5_dataset.dimensions[0] = dataset.n_observations;
	hdf5_dataset.dimensions[1] = dataset.n_words;

	// Set dataset properties

	hdf5_write_attribute(hdf5_dataset.dataset_id, "n_classes",
						 H5T_NATIVE_UINT64, &args.n_classes);
	hdf5_write_attribute(hdf5_dataset.dataset_id, "n_attributes",
						 H5T_NATIVE_UINT64, &args.n_attributes);
	hdf5_write_attribute(hdf5_dataset.dataset_id, "n_observations",
						 H5T_NATIVE_UINT64, &args.n_observations);

	// Fill data

	fprintf(stdout, " - Starting filling in dataset.\n");

	// Alocate buffer
	buffer = (unsigned long*) malloc(sizeof(unsigned long) * dataset.n_words);

	for (unsigned long line = 0; line < args.n_observations; line++) {

		fill_buffer(&dataset, args.probability_attribute_set, buffer);
		hdf5_write_n_lines(hdf5_dataset.dataset_id, line, 1, dataset.n_words,
						   H5T_NATIVE_UINT64, buffer);

		if (line % 100 == 0) {
			fprintf(stdout, " - Writing [%lu/%lu]\n", line,
					args.n_observations);
		}
	}

	// Add inconsistencies
	for (unsigned long i = 0; i < args.n_inconsistencies; i++) {
		// Pick a random line
		unsigned long from = rand() % args.n_observations;

		hdf5_read_line(&hdf5_dataset, from, dataset.n_words, buffer);

		// Get line class
		unsigned long line_class
			= get_class(buffer, dataset.n_attributes, dataset.n_words,
						dataset.n_bits_for_class);

		// Change its class
		unsigned long new_class = 0;
		do {
			new_class = rand() % dataset.n_classes;
		} while (new_class == line_class);

		set_class_bits(buffer, new_class, dataset.n_attributes, dataset.n_words,
					   dataset.n_bits_for_class);

		// Put it back somewhere else
		unsigned long to = rand() % args.n_observations;

		hdf5_write_n_lines(hdf5_dataset.dataset_id, to, 1, dataset.n_words,
						   H5T_NATIVE_UINT64, buffer);
	}

	// Add duplicates
	for (unsigned long i = 0; i < args.n_duplicates; i++) {
		// Pick a random line
		unsigned long from = rand() % args.n_observations;

		hdf5_read_line(&hdf5_dataset, from, dataset.n_words, buffer);

		// Put it back somewhere else
		unsigned long to = rand() % args.n_observations;

		hdf5_write_n_lines(hdf5_dataset.dataset_id, to, 1, dataset.n_words,
						   H5T_NATIVE_UINT64, buffer);
	}

	free(buffer);

	hdf5_close_dataset(&hdf5_dataset);

	fprintf(stdout, "All done!\n");

	return EXIT_SUCCESS;
}
