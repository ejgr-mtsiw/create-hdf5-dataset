/*
 ============================================================================
 Name        : dataset.h
 Author      : Eduardo Ribeiro
 Description : Structures and functions to manage datasets
 ============================================================================
 */
#ifndef DATASET_H
#define DATASET_H

#include "bit_utils.h"
#include "hdf5.h"
#include <math.h>


#define DATASET_VALID_DIMENSIONS 0
#define DATASET_INVALID_DIMENSIONS 1

/**
 * Calculates the dataset dimensions based on the number
 * of observations and attributes
 */
int calculate_dataset_dimensions(unsigned long n_observations, unsigned long n_attributes, int n_classes,
		hsize_t *dataset_dimensions);

/**
 * Writes an attribute to the dataset
 */
herr_t write_attribute(hid_t dataset_id, const char *attribute, hid_t datatype, const void *value);

#endif
