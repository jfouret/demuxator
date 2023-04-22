#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <zlib.h>
#include "read_gz_line.h"

std::unordered_set<std::string> select_barcodes_above_threshold(gzFile &read2_file, double threshold, int sample_size);
