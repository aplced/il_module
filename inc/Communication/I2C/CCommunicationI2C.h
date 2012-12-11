#ifndef __Communication_I2C_H__
#define __Communication_I2C_H__

#include "inc/lib_comm.h"

#include "../../../../../../../external/kernel-headers/original/linux/i2c.h"

/* This is the structure as used in the I2C_RDWR ioctl call */
struct i2c_rdwr_ioctl_data
{
    struct i2c_msg *msgs;   /* pointers to i2c_msgs */
    int nmsgs;      /* number of i2c_msgs */
};

#define I2C_2_DEVICE		"/dev/i2c-1"

int set_slave_addr(int file, int address,  int SubAddr);
int i2c_transfer(int fd,  struct i2c_msg* msg);

#endif