#pragma once
#include <string>
#include <zlib.h>

std::string read_gz_line(gzFile &file);
