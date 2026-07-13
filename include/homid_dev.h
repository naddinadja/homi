#ifndef HOMID_DEV_H
#define HOMID_DEV_H

#include <stdbool.h>

#include <homi_proto.h>
#include <homid_opts.h>

struct homid;

struct homid_device {
	struct xnvme_dev *dev;
	struct xal *xal;
	bool watching;
	char uri[HOMID_DEVURI_MAXLEN];
	char shm_name[64];
};

/**
 * Cleans up array of homid_device
 *
 * For the given number of devices, clean each homid_device's xal and xvnme setups.
 * Checks before freeing.
 *
 * @param ndevs		Number of devices to clean.
 * @param devices	Array of homid_device to clean.
 */
void
homid_dev_close(unsigned int ndevs, struct homid_device *devices);

/**
 * Setup array of homid_device
 *
 * Allocates array of homid_device and initializes xnvme and xal for each of them.
 * Uses device uri, ndevs, and xal_opts from user config options.
 *
 * @param opts		config options parsed from user config file.
 * @param devices	Output: array of homid_device to setup.
 * @return			0 on success, negative errno on failure.
 */
int
homid_dev_open(struct homid_opts *opts, struct homid_device **devices);

/**
 * Get a pointer to the homid_device from a given device URI
 *
 * @param homid   Daemon state
 * @param uri     Device URI
 * @return        A pointer to the first homid_device with a matching URI, NULL if
 *                none is found.
 */
struct homid_device *
homid_dev_get(struct homid *homid, char *uri);

#endif /* HOMID_DEV_H */
