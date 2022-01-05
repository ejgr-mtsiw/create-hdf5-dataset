/*
 ============================================================================
 Name        : create-hdf5-dataset.c
 Author      : Eduardo Ribeiro
 Description : Creates HDF5 dataset from command line arguments.
 ============================================================================
 */

#include "bit_utils.h"
#include "clargs.h"
#include "dataset.h"
#include <stdio.h>
#include <stdlib.h>
#include "hdf5.h"
#include <math.h>
#include <time.h>

/**
 * Fills the buffer with a random line of 0 and 1
 */
void fill_buffer(hsize_t n_cols, unsigned long n_attributes, int n_classes,
		int probability_attribute_set, unsigned long *buffer);

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
	 * Store the result of operations
	 */
	herr_t status = 0;

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
	unsigned long *buffer = NULL;

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
	file_id = H5Fcreate(args.filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
	if (file_id < 1) {
		// Error creating file
		fprintf(stdout, "Error creating %s\n", args.filename);
		return EXIT_FAILURE;
	}
	fprintf(stdout, " - Empty file created.\n");

	if (calculate_dataset_dimensions(args.n_observations, args.n_attributes,
			args.n_classes, dataset_dimensions) == DATASET_INVALID_DIMENSIONS) {
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
	dataset_id = H5Dcreate2(file_id, args.datasetname, H5T_STD_U64BE,
			dataset_space_id, H5P_DEFAULT, property_list_id,
			H5P_DEFAULT);
	fprintf(stdout, " - Dataset created.\n");
	fprintf(stdout, " - Starting filling in dataset.\n");

	// Close resources
	H5Pclose(property_list_id);

	// Create a memory dataspace to indicate the size of our buffer/chunk
	memory_space_id = H5Screate_simple(2, chunk_dimensions, NULL);

	// Alocate buffer
	buffer = (unsigned long*) malloc(
			sizeof(unsigned long) * chunk_dimensions[1]);

	// We will write one line at a time
	hsize_t count[2] = { 1, chunk_dimensions[1] };
	hsize_t offset[2] = { 0, 0 };

	for (unsigned long line = 0; line < args.n_observations; line++) {

		// Update offset
		offset[0] = line;

		// Select hyperslab on file dataset
		H5Sselect_hyperslab(dataset_space_id, H5S_SELECT_SET, offset, NULL,
				count, NULL);

		// Create a data line
		fill_buffer(chunk_dimensions[1], args.n_attributes, args.n_classes,
				args.probability_attribute_set, buffer);

		// Write buffer to dataset
		// mem_space and file_space should now have the same number of elements selected
		H5Dwrite(dataset_id, H5T_NATIVE_ULONG, memory_space_id,
				dataset_space_id, H5P_DEFAULT, buffer);

		if (line % 100 == 0) {
			fprintf(stdout, " - Writing [%lu/%lu]\n", line,
					args.n_observations);
		}
	}

	free(buffer);
	H5Sclose(memory_space_id);
	H5Sclose(dataset_space_id);

	// Set dataset properties

	status = write_attribute(dataset_id, "n_classes", H5T_NATIVE_INT,
			&args.n_classes);
	if (status < 0) {
		return EXIT_FAILURE;
	}

	status = write_attribute(dataset_id, "n_attributes", H5T_NATIVE_ULONG,
			&args.n_attributes);
	if (status < 0) {
		return EXIT_FAILURE;
	}

	status = write_attribute(dataset_id, "n_observations", H5T_NATIVE_ULONG,
			&args.n_observations);
	if (status < 0) {
		return EXIT_FAILURE;
	}

	H5Dclose(dataset_id);
	H5Fclose(file_id);

	fprintf(stdout, "All done!\n");

	return EXIT_SUCCESS;
}

/**
 * Fills the buffer with a random line of 0 and 1
 */
void fill_buffer(hsize_t n_cols, unsigned long n_attributes, int n_classes,
		int probability_attribute_set, unsigned long *buffer) {
	/**
	 * Probability of getting '1'
	 * TODO: replace placeholder code
	 */
	int probability = RAND_MAX / 100 * probability_attribute_set;

	// What class will this line be?
	int line_class = rand() % n_classes;

	// How many bits are needed to store the class?
	int class_bits_to_set = (int) ceil(log2(n_classes));

	unsigned long column = 0;

	for (unsigned long i = 0; i < n_cols; i++) {

		buffer[i] = 0;

		for (size_t j = 0; j < LONG_BITS; j++) {

			buffer[i] <<= 1;

			if (column < n_attributes) {
				// Filling in attributes
				if (rand() < probability) {
					buffer[i] |= 1;
				}
			} else {
				// Filling in the class
				if (class_bits_to_set > 0) {
					class_bits_to_set--;

					if (CHECK_BIT(line_class, class_bits_to_set) == 1) {
						buffer[i] |= 1;
					}
				} else {
					// We're done here: fast forward!
					buffer[i] <<= (LONG_BITS - 1 - j);
					break;
				}
			}

			column++;
		}
	}
}
