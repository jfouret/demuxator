#include <gtest/gtest.h>
#include <zlib.h>
#include <fstream>
#include <string>
#include "../include/read_gz_line.h"

TEST(ReadGzLine, TestReadGzLine) {
    // Prepare a test file
    std::string test_file = "test_file.txt";
    std::ofstream test_data_file(test_file);
    test_data_file << "Line1\n";
    test_data_file << "Line2\n";
    test_data_file.close();

    // Compress the test file
    std::string gz_test_file = "test_file.txt.gz";
    std::string gzip_command = "gzip -c " + test_file + " > " + gz_test_file;
    system(gzip_command.c_str());

    // Read gzipped test file
    gzFile gz_file = gzopen(gz_test_file.c_str(), "r");
    std::string line1 = read_gz_line(gz_file);
    std::string line2 = read_gz_line(gz_file);
    gzclose(gz_file);

    EXPECT_EQ(line1, "Line1");
    EXPECT_EQ(line2, "Line2");

    // Cleanup
    std::remove(test_file.c_str());
    std::remove(gz_test_file.c_str());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}