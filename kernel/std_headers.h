#pragma once

/** Standard headers. 
 *
 * Headers required by the majority of kernel cpp files, and potentially test systems.
 */

#include "std_types.h"

#define F_NO_COPY(Class)  Class (const Class &) = delete; Class & operator=(const Class &) = delete;

#include <klibrary/klibrary.h>
#include <mem/mem_layout.h>

#ifdef FOS_MOCK
/* Fake drivers. */
#include "_test/vga_mock.h"
#else
/* Real Drivers. */
#include <display/vga.h>
#endif


