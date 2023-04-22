
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

constexpr int SUBSET_SIZE = 10000;

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
    std::unordered_set<std::string> barcodes_above_threshold = select_barcodes_above_threshold(read2_file, threshold, SUBSET_SIZE);
    // If both expected barcodes and barcodes above threshold are provided, take the union of both sets
    if (!allowed_barcodes.empty()) {
        for (const auto &barcode : barcodes_above_threshold) {
            allowed_barcodes.insert(barcode);
        }
    } else {
        allowed_barcodes = barcodes_above_threshold;
    }

    // Key: barcode, Value: a pair of output files for R1 and R2
    std::unordered_map<std::string, std::pair<gzFile, gzFile>> output_files;

    while (true) {
        // Read R1 and R2 lines from the input files
        std::string read1_identifier = read_gz_line(read1_file);
        std::string read1_sequence = read_gz_line(read1_file);
        std::string read1_plus_line = read_gz_line(read1_file);
        std::string read1_quality = read_gz_line(read1_file);

        std::string read2_identifier = read_gz_line(read2_file);
        std::string read2_sequence = read_gz_line(read2_file);
        std::string read2_plus_line = read_gz_line(read2_file);
        std::string read2_quality = read_gz_line(read2_file);

        // Break the loop if either R1 or R2 reaches the end of the file
        if (read1_identifier.empty() || read2_identifier.empty()) {
            break;
        }

        // Extract the barcode from the R2 read
        std::string barcode = read2_sequence.substr(10, 12);

        // Skip the read if the barcode is not in the allowed_barcodes set
        if (allowed_barcodes.find(barcode) == allowed_barcodes.end()) {
            continue;
        }

        // Check if the output files for this barcode have been created
        auto it = output_files.find(barcode);
        if (it == output_files.end()) {
            // If not, create the output files and add them to the output_files map
            std::string output_file_name_r1 = barcode + "_R1.fastq.gz";
            std::string output_file_name_r2 = barcode + "_R2.fastq.gz";
            gzFile output_file_r1 = gzopen(output_file_name_r1.c_str(), "wt");
            gzFile output_file_r2 = gzopen(output_file_name_r2.c_str(), "wt");
            assert(output_file_r1);
            assert(output_file_r2);
            output_files[barcode] = {output_file_r1, output_file_r2};
            it = output_files.find(barcode);
        }

        // Write the R1 and R2 reads to their respective output files
        gzFile output_file_r1 = it->second.first;
        gzFile output_file_r2 = it->second.second;
        gzputs(output_file_r1, read1_identifier.c_str());
        gzputc(output_file_r1, '\n');
        gzputs(output_file_r1, read1_sequence.c_str());
        gzputc(output_file_r1, '\n');
        gzputs(output_file_r1, read1_plus_line.c_str());
        gzputc(output_file_r1, '\n');
        gzputs(output_file_r1, read1_quality.c_str());
        gzputc(output_file_r1, '\n');

        gzputs(output_file_r2, read2_identifier.c_str());
        gzputc(output_file_r2, '\n');
        gzputs(output_file_r2, read2_sequence.c_str());
        gzputc(output_file_r2, '\n');
        gzputs(output_file_r2, read2_plus_line.c_str());
        gzputc(output_file_r2, '\n');
        gzputs(output_file_r2, read2_quality.c_str());
        gzputc(output_file_r2, '\n');
    }

    // Close all output files
    for (auto &entry : output_files) {
        gzclose(entry.second.first);
        gzclose(entry.second.second);
    }

    // Close input files
    gzclose(read1_file);
    gzclose(read2_file);

    return 0;
}

