#pragma once
#include <string>
#include <fstream>
#include <unordered_set>

std::unordered_set<std::string> load_expected_barcodes(const std::string &file_path);
