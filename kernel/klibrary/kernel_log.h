#pragma once

#include <std_types.h>
#include <klibrary/kstring.h>

void kdebug (const KString& log);
void kerror (const KString& log);
void khex_dump (const char* buffer, uint16 bytes_to_show);

void kpanic (const kstring & message);

