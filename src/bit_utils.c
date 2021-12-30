/*
 ============================================================================
 Name        : bit_utils.c
 Author      : Eduardo Ribeiro
 Description : Utils
 ============================================================================
 */
#include "bit_utils.h"

/**
 * How many bits in a long?
 */
unsigned int get_number_of_bits_in_a_long() {
	return sizeof(unsigned long) * CHAR_BIT;
}

/**
 *
 */
int get_nth_bit(int x, int n) {
	return (x >> n) & 1;
}
