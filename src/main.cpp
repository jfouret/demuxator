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

int main(int argc, char *argv[]) {
    int opt;
    std::string expected_barcodes_path;
    double threshold = 0.01;
    int subset_size = 10000;
    int bc_start = 10;
    int bc_length = 12;
    bool remove_barcode = false;

    static struct option long_options[] = {
        {"expected-barcodes", required_argument, 0, 'e'},
        {"threshold", required_argument, 0, 't'},
        {"subset-size", optional_argument, 0, 's'},
        {"bc-start", optional_argument, 0, 'b'},
        {"bc-length", optional_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int long_index = 0;
    while ((opt = getopt_long(argc, argv, "e:t:s:b:l:h", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'e':
                expected_barcodes_path = optarg;
                break;
            case 't':
                threshold = std::stod(optarg);
                break;
            case 's':
                subset_size = std::stoi(optarg);
                break;
            case 'b':
                bc_start = std::stoi(optarg);
                break;
            case 'l':
                bc_length = std::stoi(optarg);
                break;
            case 'r':
                remove_barcode = true;
                break;
            case 'h':
            default:
                std::cerr << "Usage: " << argv[0] << " [-e|--expected-barcodes expected_barcodes.txt] [-t|--threshold threshold] [--subset-size subset_size] [--bc-start bc_start] [--bc-length bc_length] [--remove-barcode] read1.fastq.gz read2.fastq.gz" << std::endl;
                std::cerr << "  -e, --expected-barcodes: Path to the expected barcodes file (default: none)" << std::endl;
                std::cerr << "  -t, --threshold: Threshold for selecting barcodes based on their relative occurrence in the subset (default: 0.01)" << std::endl;
                std::cerr << "  --subset-size: Subset size for selecting barcodes (default: 10000)" << std::endl;
                std::cerr << "  --bc-start: Start position of barcodes in read 2 (default: 10)" << std::endl;
                std::cerr << "  --bc-length: Length of barcodes in read 2 (default: 12)" << std::endl;
                std::cerr << "  --remove-barcode: Remove barcode from output reads (default: false)" << std::endl;
                std::cerr << "  -h, --help: Show this help message" << std::endl;
                std::cerr << "Output files are written in the current directory using the barcode sequence as prefix in filenames." << std::endl;
                return (opt == 'h') ? 0 : 1;
        }
    }

    if (optind + 2 > argc) {
        std::cerr << "Usage: " << argv[0] << " [-e|--expected-barcodes expected_barcodes.txt] [-t|--threshold threshold] [--subset-size subset_size] [--bc-start bc_start] [--bc-length bc_length] [--remove-barcode] read1.fastq.gz read2.fastq.gz" << std::endl;
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

    std::unordered_set<std::string> barcodes_above_threshold = select_barcodes_above_threshold(read2_file, bc_start, bc_length, threshold, subset_size);
    // If both expected barcodes and barcodes above threshold are provided, take the union of both sets
    if (!allowed_barcodes.empty()) {
        for (const auto &barcode : barcodes_above_threshold) {
            allowed_barcodes.insert(barcode);
        }
    } else {
        allowed_barcodes = barcodes_above_threshold;
    }

    demuxator(read1_file, read2_file, allowed_barcodes, bc_start, bc_length, remove_barcode);

    return 0;

}