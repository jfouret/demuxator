#include "extract_barcode.h"

std::string extract_barcode(const std::string& read2_sequence, int bc_start, int bc_length) {
    if (read2_sequence.size() < bc_start + bc_length - 1) {
        std::cerr << "Warning: Read sequence is too short to extract the barcode." << std::endl;
        return "";
    }
    return read2_sequence.substr(bc_start - 1, bc_length);
}
