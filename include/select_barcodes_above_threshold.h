#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <zlib.h>
#include "GzReader.h"

std::unordered_set<std::string> select_barcodes_above_threshold(GzReader &read2_file, int bc_start, int bc_end, double threshold, int sample_size);
