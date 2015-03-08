#pragma once

/** Standard headers. 
 *
 * Headers required by the majority of kernel cpp files, and potentially test systems.
 */
#include <float.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <iso646.h>
#include <stdbool.h> 
#include <stdint.h>

#include <klibrary/klibrary.h>

#ifdef FOS_MOCK
/* Fake drivers. */
#include "_test/vga_mock.h"
#include "kernel_log.h"

#else
/* Real Drivers. */
#include <display/vga.h>
#include "hw/timer.h"
#endif


