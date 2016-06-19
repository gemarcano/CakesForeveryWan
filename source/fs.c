#include "fs.h"

#include <stddef.h>
#include "draw.h"
#include <ctr9/io.h>
#include <ctr9/io/ctr_fatfs.h>


static FATFS fs;
static ctr_sd_interface sd;

int mount_sd()
{
	ctr_fatfs_initialize(NULL, NULL, NULL, &sd);
    if (f_mount(&fs, "SD:", 1) != FR_OK) {
        print("Failed to mount SD card!");
        return 1;
    }
    return 0;
}

int unmount_sd()
{
    if (f_mount(NULL, "SD:", 1) != FR_OK) {
        print("Failed to mount SD card!");
        return 1;
    }
    print("Unmounted SD card");
    return 0;
}

int read_file_offset(void *dest, const char *path, unsigned int size, unsigned int offset)
{
    FRESULT fr;
    FIL handle;
    unsigned int bytes_read = 0;

    fr = f_open(&handle, path, FA_READ);
    if (fr != FR_OK) goto error;

    if (!size) {
        size = f_size(&handle);
    }

    if (offset) {
        fr = f_lseek(&handle, offset);
        if (fr != FR_OK) goto error;
    }

    fr = f_read(&handle, dest, size, &bytes_read);
    if (fr != FR_OK) goto error;

    fr = f_close(&handle);
    if (fr != FR_OK) goto error;

    return 0;

error:
    f_close(&handle);
    return fr;
}

int write_file(const void *buffer, const char *path, unsigned int size)
{
    FRESULT fr;
    FIL handle;
    unsigned int bytes_written = 0;

    fr = f_open(&handle, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr != FR_OK) goto error;

    fr = f_write(&handle, buffer, size, &bytes_written);
    if (fr != FR_OK || bytes_written != size) goto error;

    // For some reason this always returns 1
    f_close(&handle);

    return 0;

error:
    f_close(&handle);
    return fr;
}
