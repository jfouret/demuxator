#pragma once
#include <iostream>
#include <string>

std::string extract_barcode(const std::string& read2_sequence, int bc_start, int bc_length);
