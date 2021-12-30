/*
 ============================================================================
 Name        : dataset.c
 Author      : Eduardo Ribeiro
 Description : Structures and functions to manage datasets
 ============================================================================
 */

#include "dataset.h"

/**
 * Calculates the dataset dimensions based on the number
 * of observations and attributes
 */
int calculate_dataset_dimensions(unsigned long n_observations, unsigned long n_attributes, /*int n_classes,*/
hsize_t *dataset_dimensions) {

	// check for invalid dimensions
	if (n_observations < 1 || n_attributes < 1) {
		return DATASET_INVALID_DIMENSIONS;
	}

	/**
	 * Create a 2D dataspace
	 * Set dataspace dimension
	 * Number of lines = number of observations
	 * Number of columns = number of attributes + 1 (for class)
	 * TODO: should be n more depending on the number of classes
	 */
	// https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
	unsigned long n_cols = (n_attributes + 1) / BITS_IN_A_LONG + ((n_attributes + 1) % BITS_IN_A_LONG != 0);

	dataset_dimensions[0] = n_observations;
	dataset_dimensions[1] = n_cols;

	return DATASET_VALID_DIMENSIONS;
}

/**
 * Writes an attribute to the dataset
 */
herr_t write_attribute(hid_t dataset_id, const char *attribute, hid_t datatype, const void *value) {
	hid_t attr_dataspace = H5Screate(H5S_SCALAR);
	hid_t attr = H5Acreate2(dataset_id, attribute, datatype, attr_dataspace, H5P_DEFAULT, H5P_DEFAULT);

	// Write the attribute to the dataset
	herr_t status = H5Awrite(attr, datatype, value);
	if (status < 0) {
		fprintf(stderr, "Error writing attribute %s.\n", attribute);
		return status;
	}

	// Close the attribute.
	status = H5Aclose(attr);
	if (status < 0) {
		fprintf(stderr, "Error closing attribute %s.\n", attribute);
		return status;
	}

	// Close the dataspace.
	status = H5Sclose(attr_dataspace);
	if (status < 0) {
		fprintf(stderr, "Error closing attribute %s datatspace.\n", attribute);
	}

	return status;
}
