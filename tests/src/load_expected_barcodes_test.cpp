#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <unordered_set>
#include "../include/load_expected_barcodes.h"

TEST(LoadExpectedBarcodes, TestFileLoading) {
    // Prepare a test file
    std::string test_file = "test_barcodes.txt";
    std::ofstream test_barcodes_file(test_file);
    test_barcodes_file << "AGCT\n";
    test_barcodes_file << "CGTA\n";
    test_barcodes_file.close();

    std::unordered_set<std::string> expected_barcodes = load_expected_barcodes(test_file);

    EXPECT_EQ(expected_barcodes.size(), 2);
    EXPECT_TRUE(expected_barcodes.find("AGCT") != expected_barcodes.end());
    EXPECT_TRUE(expected_barcodes.find("CGTA") != expected_barcodes.end());
    EXPECT_FALSE(expected_barcodes.find("CATA") != expected_barcodes.end());

    // Cleanup
    std::remove(test_file.c_str());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}