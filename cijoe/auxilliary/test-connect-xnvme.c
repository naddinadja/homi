/**
 * Test for the HOMI xNVMe secondary-process IPC.
 *
 * Compile and test with
 * gcc test-connect-xnvme.c -o test-connect-xnvme -lhomic -lxnvme; ./test-connect-xnvme 0000:01:00.0 upcie [lba]
 */
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <homic.h>
#include <libxnvme.h>
#include <libxnvme_nvm.h>

static int
test_xnvme(const char *dev_uri, const char *be, uint64_t lba, int do_read)
{
	const struct xnvme_geo *geo = NULL;
	struct xnvme_cmd_ctx ctx;
	struct xnvme_dev *dev = NULL;
	void *buf = NULL;
	int err;

	err = homic_connect_xnvme((char *)dev_uri, (char *)be, &dev);
	if (err) {
		printf("FAIL homic_connect_xnvme(%s, %s): %d\n", dev_uri, be, err);
		goto exit;
	}
	printf("OK homic_connect_xnvme(%s, %s)\n", dev_uri, be);

	geo = xnvme_dev_get_geo(dev);
	if (!geo) {
		printf("FAIL xnvme_dev_get_geo(): NULL\n");
		err = -1;
		goto exit;
	}
	printf("OK xnvme_dev_get_geo(): lba_nbytes=%" PRIu32 " tbytes=%" PRIu64 " mdts_nbytes=%" PRIu32 "\n",
	       geo->lba_nbytes, geo->tbytes, geo->mdts_nbytes);

	if (!do_read) {
		goto exit;
	}

	buf = xnvme_buf_alloc(dev, geo->lba_nbytes);
	if (!buf) {
		printf("FAIL xnvme_buf_alloc(%zu): %d\n", (size_t)geo->lba_nbytes, -errno);
		err = -errno;
		goto exit;
	}

	ctx = xnvme_cmd_ctx_from_dev(dev);
	err = xnvme_nvm_read(&ctx, xnvme_dev_get_nsid(dev), lba, 0, buf, NULL);
	if (err || xnvme_cmd_ctx_cpl_status(&ctx)) {
		printf("FAIL xnvme_nvm_read(lba=%" PRIu64 "): err=%d\n", lba, err);
		err = err ? err : -EIO;
		goto exit;
	}
	printf("OK xnvme_nvm_read(lba=%" PRIu64 "): first bytes: %02x %02x %02x %02x\n",
	       lba,
	       ((unsigned char *)buf)[0],
	       ((unsigned char *)buf)[1],
	       ((unsigned char *)buf)[2],
	       ((unsigned char *)buf)[3]);

exit:
	xnvme_buf_free(dev, buf);
	return err;
}

int
main(int argc, char **argv)
{
	uint64_t lba = 0;
	int do_read = 0;
	int err = 0;

	if (argc < 3) {
		printf("Usage: %s <dev_uri> <be> [lba]\n", argv[0]);
		return 1;
	}

	if (argc > 3) {
		lba = strtoull(argv[3], NULL, 0);
		do_read = 1;
	}

	err = homic_connect("/run/homi/homi.sock");
	if (err) {
		printf("Failed connecting\n");
		goto exit;
	}
	printf("Connection success!\n");

	err = test_xnvme(argv[1], argv[2], lba, do_read);

exit:
	printf("Closing connection and exiting ...\n");
	homic_disconnect();
	return err;
}
