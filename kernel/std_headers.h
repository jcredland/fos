#pragma once

/** Standard headers. 
 *
 * Headers required by the majority of kernel cpp files, and potentially test systems.
 */

#include "std_types.h"

#include <klibrary/klibrary.h>

#ifdef FOS_MOCK
/* Fake drivers. */
#include "_test/vga_mock.h"
#else
/* Real Drivers. */
#include <display/vga.h>
#include "hw/timer.h"
#endif


