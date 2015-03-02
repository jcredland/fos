#pragma once

/** Standard headers. */
#include <float.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <iso646.h>
#include <stdbool.h> 
#include <stdint.h>


/** Local headers. */
#include "kernel_type.h"
#include "error_codes.h"

#include "clib.h"
#include "util.h"
#include "kernel_string.h"
#include "random.h"

#ifdef FOS_MOCK
/* Fake drivers. */
#include "_test/vga_mock.h"
#include "kernel_log.h"

#else
/* Real Drivers. */
#include "vga.h"
#include "kernel_log.h"
#include "timer.h"
#endif


