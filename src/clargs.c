#include "clargs.h"

/**
 * Reads the arguments from the command line and stores
 * them in the args structure
 */
int read_args(int argc, char **argv, clargs *args) {
	int c = 0, idata = 0;
	unsigned long uidata = 0;

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
			} else {
				fprintf(stderr, "The number of classes must be >= 2.\n");
				return READ_CL_ARGS_NOK;
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
			} else {
				fprintf(stderr, "The probability must be between 0 and 100.\n");
				return READ_CL_ARGS_NOK;
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
		fprintf(stdout,
				"-z compress_dataset_level Default: 6, 0 disables compression\n");
		return READ_CL_ARGS_NOK;
	}

	return READ_CL_ARGS_OK;
}
