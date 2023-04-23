#include "select_barcodes_above_threshold.h"

std::unordered_set<std::string> select_barcodes_above_threshold(gzFile &read2_file, int bc_start, int bc_length, double threshold, int sample_size) {
    std::unordered_map<std::string, int> barcode_counts;
    int total_count = 0;

    for (int i = 0; i < sample_size; ++i) {
        std::string read2_identifier = read_gz_line(read2_file);
        std::string read2_sequence = read_gz_line(read2_file);
        std::string read2_plus_line = read_gz_line(read2_file);
        std::string read2_quality = read_gz_line(read2_file);

        if (read2_identifier.empty()) {
            break;
        }

        std::string barcode = read2_sequence.substr(bc_start - 1, bc_length);
        barcode_counts[barcode]++;
        total_count++;
    }

    int threshold_count = static_cast<int>(threshold * total_count);
    std::unordered_set<std::string> selected_barcodes;
    for (const auto &entry : barcode_counts) {
        if (entry.second >= threshold_count) {
            selected_barcodes.insert(entry.first);
        }
    }

    gzrewind(read2_file);
    return selected_barcodes;
}