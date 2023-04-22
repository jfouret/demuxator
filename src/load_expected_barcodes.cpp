#include "load_expected_barcodes.h"

std::unordered_set<std::string> load_expected_barcodes(const std::string &file_path) {
    std::unordered_set<std::string> expected_barcodes;
    std::ifstream file(file_path);
    std::string line;
    while (std::getline(file, line)) {
        expected_barcodes.insert(line);
    }
    return expected_barcodes;
}