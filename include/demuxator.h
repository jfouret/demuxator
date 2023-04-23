
#include <iostream>
#include <string>
#include <cassert>
#include <unordered_set>
#include <unistd.h>
#include <getopt.h>
#include <zlib.h>
#include "read_gz_line.h"
#include "load_expected_barcodes.h"
#include "select_barcodes_above_threshold.h"

void demuxator(gzFile &read1_file, gzFile &read2_file, std::unordered_set<std::string> expected_barcodes, int bc_start, int bc_end);