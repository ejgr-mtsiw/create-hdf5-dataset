/*
 ============================================================================
 Name        : create-hdf5-dataset.c
 Author      : Eduardo Ribeiro
 Description : Creates HDF5 dataset from command line arguments.
 ============================================================================
 */

#include <clargs.h>
#include <stdio.h>
#include <stdlib.h>
#include "hdf5.h"
#include <limits.h>

#define VALID_DIMENSIONS 0
#define INVALID_DIMENSIONS 1

/**
 * How many bits in a long?
 */
size_t get_number_of_bits_in_a_long() {
	return sizeof(unsigned long int) * CHAR_BIT;
}

/**
 * Calculates the dataset dimensions based on the number
 * of observations and attributes
 */
int calculate_dataset_dimensions(clargs *args, hsize_t *dataset_dimensions) {

	// How many bits in a long?
	size_t nbits = get_number_of_bits_in_a_long();

	// check for invalid dimensions
	if (args->n_observations < 1 || args->n_attributes < 1) {
		return INVALID_DIMENSIONS;
	}

	/**
	 * Create a 2D dataspace
	 * Set dataspace dimension
	 * Number of lines = number of observations
	 * Number of columns = number of attributes + 1 (for class)
	 * TODO: should be n more depending on the number of classes
	 */
	// https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
	unsigned long int ncols = (args->n_attributes + 1) / nbits + ((args->n_attributes + 1) % nbits != 0);

	dataset_dimensions[0] = args->n_observations;
	dataset_dimensions[1] = ncols;

	return VALID_DIMENSIONS;
}

/**
 * Fills the buffer with a random line of 0 and 1
 */
void fill_buffer(clargs *args, hsize_t ncols, unsigned long int *buffer) {
	/**
	 * Probability of getting '1'
	 * TODO: replace placeholder code
	 */
	int probability = RAND_MAX / 100 * args->probability_attribute_set;
	size_t nbits = get_number_of_bits_in_a_long();

	unsigned long int column = 0;
	char class_set = 0;

	for (unsigned long int i = 0; i < ncols; i++) {

		buffer[i] = 0;
		class_set = 0;
		column = 0;

		for (size_t j = 0; j < nbits; j++) {

			buffer[i] <<= 1;

			if (column < args->n_attributes) {
				// Filling attributes
				if (random() < probability) {
					buffer[i] += 1;
				}
			} else {
				// first time here? Fill in the class
				if (class_set == 0) {
					class_set = 1;

					// TODO: support more than 2 classes
					if (random() < probability) {
						buffer[i] += 1;
					}
				}

			}
		}
	}
}


/**
 *
 */
int main(int argc, char **argv) {

	/**
	 * Command line arguments set by the user
	 */
	clargs args;

	/**
	 * File identifier
	 */
	hid_t file_id = 0;

	/**
	 * Dataset identifier
	 */
	hid_t dataset_id = 0;

	/**
	 * Dataset dataspace identifier
	 */
	hid_t dataset_space_id = 0;

	/**
	 * Dataset creation property list identifier
	 */
	hid_t property_list_id = 0;

	/**
	 * In memory dataspace identifier
	 */
	hid_t memory_space_id = 0;

	/**
	 * Dataset dimensions
	 */
	hsize_t dataset_dimensions[2] = { 0, 0 };

	/**
	 * Chunk dimensions
	 */
	hsize_t chunk_dimensions[2] = { 0, 0 };

	/**
	 * Buffer to store one line/chunk of data
	 */
	unsigned long int *buffer = NULL;

	/**
	 * Parse command line arguments
	 */
	if (read_args(argc, argv, &args) == READ_CL_ARGS_NOK) {
		return EXIT_FAILURE;
	}

	/**
	 * Create the data file
	 */
	file_id = H5Fcreate(args.filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
	if (file_id < 1) {
		// Error creating file
		fprintf(stdout, "Error creating %s\n", args.filename);
		return EXIT_FAILURE;
	}
	fprintf(stdout, " - Empty file created.\n");

	if (calculate_dataset_dimensions(&args, dataset_dimensions) == INVALID_DIMENSIONS) {
		// Invalid dimensions
		fprintf(stdout, " - Invalid dataset dimensions!\n");
		return EXIT_FAILURE;
	}

	dataset_space_id = H5Screate_simple(2, dataset_dimensions, NULL);
	fprintf(stdout, " - Dataspace created.\n");

	// Create a dataset creation property list
	property_list_id = H5Pcreate(H5P_DATASET_CREATE);
	H5Pset_layout(property_list_id, H5D_CHUNKED);

	// The choice of the chunk size affects performance!
	// for now we will choose one line
	chunk_dimensions[0] = 1;
	chunk_dimensions[1] = dataset_dimensions[1];

	H5Pset_chunk(property_list_id, 2, chunk_dimensions);
	fprintf(stdout, " - Property list created.\n");

	if (args.compress_dataset == USE_COMPRESSION) {
		// Set ZLIB / DEFLATE Compression.
		H5Pset_deflate(property_list_id, args.compression_level);
	}

	// Create the dataset
	dataset_id = H5Dcreate2(file_id, args.datasetname, H5T_NATIVE_ULONG, dataset_space_id, H5P_DEFAULT,
			property_list_id, H5P_DEFAULT);
	fprintf(stdout, " - Dataset created.\n");

	// Close resources
	H5Pclose(property_list_id);

	// Create a memory dataspace to indicate the size of our buffer/chunk
	memory_space_id = H5Screate_simple(2, chunk_dimensions, NULL);

	// Alocate buffer
	buffer = (unsigned long int*) malloc(sizeof(unsigned long int) * chunk_dimensions[1]);

	// We will write one line at a time
	hsize_t count[2] = { 1, dataset_dimensions[1] };
	hsize_t offset[2] = { 0, 0 };

	for (unsigned long int line = 0; line < args.n_observations; line++) {

		// Update offset
		offset[0] = line;

		// Select hyperslab on file dataset
		H5Sselect_hyperslab(dataset_space_id, H5S_SELECT_SET, offset, NULL, count, NULL);

		// Create a data line
		fill_buffer(&args, chunk_dimensions[1], buffer);

		// Write buffer to dataset
		// mem_space and file_space should now have the same number of elements selected
		H5Dwrite(dataset_id, H5T_NATIVE_ULONG, memory_space_id, dataset_space_id, H5P_DEFAULT, buffer);
		if (line % 100 == 0) {
			fprintf(stdout, " - Writing [%lu/%lu]\n", line, args.n_observations);
		}
	}

	free(buffer);
	H5Sclose(dataset_space_id);
	H5Sclose(memory_space_id);
	H5Dclose(dataset_id);
	H5Fclose(file_id);

	fprintf(stdout, "All done!\n");

	return EXIT_SUCCESS;
}
