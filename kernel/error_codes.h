#pragma once

/** @file contains result codes used by various kernel functions and system calls. 
 *
 * In general 0, or in some cases a positive number should mean success and negative
 * numbers should mean failure. 
 *
 */

enum class DiskResultCode
{
    SUCCESS = 0,
    END_OF_FILE = -1,
    DISK_ERROR = -2,
    FILE_NOT_FOUND = -3,
    BAD_HANDLE = -4,
    ALIGNMENT_ERROR = -5  

};
