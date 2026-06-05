#ifndef HOMID_XAL_H
#define HOMID_XAL_H

#include <stdatomic.h>
#include <stdbool.h>

#include <homi_proto.h>
#include <homid_opts.h>

struct homid;

enum homid_xal_watchstate {
	HOMID_XAL_WATCHSTATE_NONE = 0, ///< Will not listen for file changes
	HOMID_XAL_WATCHSTATE_IDLE = 1, ///< Will listen for file changes, but is not started
	HOMID_XAL_WATCHSTATE_WATCHING = 2, ///< Is actively listening for file changes.
};

struct homid_device {
	struct xnvme_dev *dev;
	struct xal *xal;
	enum homid_xal_watchstate watchstate;
	_Atomic bool indexed;
	char shm_name[64];
};

/**
 * Setup xal for the homid_device
 *
 * For the given homid_device, open xal and retrieve extents through a full scan.
 * xnvme device must be initialized first.
 *
 * @param opts		xal_opts parsed from config file.
 * @param device	Output: device to setup.
 * @return			0 on success, negative errno on failure.
 */
int
homid_xal_setup(struct xal_opts *opts, struct homid_device *device);

/**
 * Setup xnvme for the homid_device
 *
 * For the given homid_device, initialize xnvme as the primary process.
 * Calls xnvme_dev_get_geo() after opening to publish identify data for secondaries.
 *
 * @param uri		URI of the device.
 * @param be		xNVMe backend name (e.g. "linux", "upcie").
 * @param device	Output: device to setup.
 * @return			0 on success, negative errno on failure.
 */
int
homid_xnvme_setup(char *uri, const char *be, struct xnvme_dev **device);

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
homid_device_close(unsigned int ndevs, struct homid_device *devices);

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
homid_device_setup(struct homid_opts *opts, struct homid_device **devices);

/**
 * Get a pointer to the homid_device from a given device URI
 *
 * Returns the first device whose URI matches. Use when backend does not matter
 * (e.g. xal lookups where the same shared memory is exported for all backends).
 *
 * @param homid   Daemon state
 * @param uri     Device URI
 * @return        A pointer to the first homid_device with a matching URI, NULL if
 *                none is found.
 */
struct homid_device *
homid_device_get(struct homid *homid, char *uri);

/**
 * Get a pointer to the homid_device matching both URI and backend
 *
 * Use when the caller needs a specific backend instance, e.g. for xNVMe
 * secondary attach where the secondary must use the same backend as the primary.
 *
 * @param homid   Daemon state
 * @param uri     Device URI
 * @param be      xNVMe backend name
 * @return        A pointer to the homid_device matching both uri and be, NULL if
 *                none is found.
 */
struct homid_device *
homid_device_get_be(struct homid *homid, char *uri, char *be);

void
_on_xal_dirty(struct xal *xal, void *cb_args);

#endif /* HOMID_XAL_H */
