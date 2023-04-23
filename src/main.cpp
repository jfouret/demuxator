
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
#include "demuxator.h"

constexpr int SUBSET_SIZE = 10000;
constexpr int BC_START = 10;
constexpr int BC_END = 22;

int main(int argc, char *argv[]) {
    int opt;
    std::string expected_barcodes_path;
    double threshold = 0.01;
    
    static struct option long_options[] = {
        {"expected-barcodes", required_argument, 0, 'e'},
        {"threshold", required_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    int long_index = 0;
    while ((opt = getopt_long(argc, argv, "e:t:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'e':
                expected_barcodes_path = optarg;
                break;
            case 't':
                threshold = std::stod(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-e|--expected-barcodes expected_barcodes.txt] [-t|--threshold threshold] read1.fastq.gz read2.fastq.gz" << std::endl;
                return 1;
        }
    }

    if (optind + 2 > argc) {
        std::cerr << "Usage: " << argv[0] << " [-e|--expected-barcodes expected_barcodes.txt] [-t|--threshold threshold] read1.fastq.gz read2.fastq.gz" << std::endl;
        return 1;
    }

    const char *read1_path = argv[optind];
    const char *read2_path = argv[optind + 1];

    gzFile read1_file = gzopen(read1_path, "rt");
    gzFile read2_file = gzopen(read2_path, "rt");
    if (!read1_file || !read2_file) {
        std::cerr << "Error: Cannot open input files." << std::endl;
        return 1;
    }

    std::unordered_set<std::string> allowed_barcodes;
    if (!expected_barcodes_path.empty()) {
        allowed_barcodes = load_expected_barcodes(expected_barcodes_path);
    }
    
    std::unordered_set<std::string> barcodes_above_threshold = select_barcodes_above_threshold(read2_file, BC_START, BC_END, threshold, SUBSET_SIZE);
    // If both expected barcodes and barcodes above threshold are provided, take the union of both sets
    if (!allowed_barcodes.empty()) {
        for (const auto &barcode : barcodes_above_threshold) {
            allowed_barcodes.insert(barcode);
        }
    } else {
        allowed_barcodes = barcodes_above_threshold;
    }

    demuxator(read1_file, read2_file, allowed_barcodes, BC_START, BC_END);

    return 0;
}

