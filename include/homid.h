#ifndef HOMID_H
#define HOMID_H

struct homid {
  struct homid_ipc_connection *conn;
  struct homid_dev *dev;	///< Pointer to array of 'struct homid_dev'
  unsigned int ndevs;		///< Number of devices
};

#endif /* HOMID_H */
