#include "select_barcodes_above_threshold.h"
#include "extract_barcode.h"
#include "GzReader.h"

std::unordered_set<std::string> select_barcodes_above_threshold(GzReader &read_file, int bc_start, int bc_length, double threshold, int sample_size) {
    std::unordered_map<std::string, int> barcode_counts;
    int total_count = 0;

    for (int i = 0; i < sample_size; ++i) {
        read_file.skip_line();
        std::string read_sequence = read_file.read_line();
        read_file.skip_line();
        read_file.skip_line();

        if (read_sequence.empty()) {
            break;
        }

        std::string barcode = extract_barcode(read_sequence, bc_start, bc_length);
        if (! barcode.empty()) {
            barcode_counts[barcode]++;
            total_count++;
        } 
    }

    int threshold_count = static_cast<int>(threshold * total_count);
    std::unordered_set<std::string> selected_barcodes;
    for (const auto &entry : barcode_counts) {
        if (entry.second >= threshold_count) {
            selected_barcodes.insert(entry.first);
        }
    }

    read_file.rewind();

    return selected_barcodes;
}