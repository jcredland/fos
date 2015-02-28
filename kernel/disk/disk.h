#pragma once
#include "../std_headers.h"

enum class DiskResultCode
{
    SUCCESS = 0,
    END_OF_FILE = -1,
    DISK_ERROR = -2
};

#include "storage_driver.h" 
#include "ata_pio_driver.h" 

#include "fat16.h"
