#include "inc/Communication/I2C/CCommunicationI2C.h"

int set_slave_addr(int fd, int address, int SubAddr)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg msg;
    TIMM_OSAL_U8 addr_data[2], add_data_8bit;
    int rc;
    msg.addr = address;
    msg.flags = 0;//write
    msg.len = 2;
    msg.buf = addr_data;
    addr_data[0] = (TIMM_OSAL_U8)(SubAddr >> 8) & 0xFF;
    addr_data[1] = (TIMM_OSAL_U8) SubAddr & 0xFF;
    if (addr_data[0] == 0)
    {
         add_data_8bit = addr_data[1];
         msg.len = 1;
         msg.buf = &add_data_8bit;
    }

    data.msgs = &msg;
    data.nmsgs = 1;
   // MMS_IL_PRINT("Subb Addres to set - 0x%02X%02X\n", data.msgs->buf[0], data.msgs->buf[1]);
    rc = ioctl(fd, I2C_RDWR, &data);
    if (rc == -1)
    {
        MMS_IL_PRINT("%s: I2C_RDWR fail\n", __func__);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    return TIMM_OSAL_ERR_NONE;
}

int i2c_transfer(int fd, struct i2c_msg *msg)
{
    struct i2c_rdwr_ioctl_data data;

    int rc;

    data.msgs = msg;
    data.nmsgs = 1;

    rc = ioctl(fd, I2C_RDWR, &data);
    if (rc == -1)
    {
        MMS_IL_PRINT("%s: I2C_RDWR fail\n", __func__);
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    return TIMM_OSAL_ERR_NONE;
}