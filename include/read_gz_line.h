#pragma once
#include <string>
#include <zlib.h>

void skip_gz_line(gzFile &file);
std::string read_gz_line(gzFile &file);
