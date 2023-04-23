#include "gtest/gtest.h"
#include "../include/demuxator.h"
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <vector>
#include <zlib.h>
#include <random>

void write_test_files(const std::vector<std::string>& barcodes) {
    std::ofstream read1_file("test_R1.fastq");
    std::ofstream read2_file("test_R2.fastq");

    for (const auto& barcode : barcodes) {
        read1_file << "@" << barcode << "_1\n";
        read1_file << "NNN" << barcode << "NNN\n";
        read1_file << "+\n";
        read1_file << "IIIIIIIIIIII\n";

        read2_file << "@" << barcode << "_2\n";
        read2_file << "NNNNNN" << barcode << "NN" << barcode << "NN\n";
        read2_file << "+\n";
        read2_file << "IIIIIIIIIIIIIIIIIIIIII\n";
    }

    read1_file.close();
    read2_file.close();

    // Compress the test files
    std::string command = "gzip test_R1.fastq";
    std::system(command.c_str());
    command = "gzip test_R2.fastq";
    std::system(command.c_str());
}

bool file_exists(const std::string& file_path) {
    struct stat buffer;
    return (stat(file_path.c_str(), &buffer) == 0);
}

void check_output_file_content(const std::string& barcode) {
    std::string output_file_name_r1 = barcode + "_R1.fastq.gz";
    std::string output_file_name_r2 = barcode + "_R2.fastq.gz";

    gzFile output_file_r1 = gzopen(output_file_name_r1.c_str(), "rb");
    gzFile output_file_r2 = gzopen(output_file_name_r2.c_str(), "rb");

    std::string expected_read1_seq = "NNN" + barcode + "NNN";
    std::string expected_read2_seq = "NN" + barcode + "NN";

    skip_gz_line(output_file_r1);  // Skip line
    std::string read1_sequence = read_gz_line(output_file_r1);
    read_gz_line(output_file_r1);  // Skip line
    std::string read1_quality = read_gz_line(output_file_r1);

    skip_gz_line(output_file_r2);  // Skip line
    std::string read2_sequence = read_gz_line(output_file_r2);
    read_gz_line(output_file_r2);  // Skip line
    std::string read2_quality = read_gz_line(output_file_r2);

    gzclose(output_file_r1);
    gzclose(output_file_r2);

    EXPECT_EQ(expected_read1_seq, read1_sequence);
    EXPECT_EQ(expected_read2_seq, read2_sequence);
    EXPECT_EQ(read1_quality.length(), expected_read1_seq.length());
    EXPECT_EQ(read2_quality.length(), expected_read2_seq.length());
}


TEST(DemuxatorTest, SplitInputFilesBasedOnAllowedBarcodes) {
    // Prepare input files (R1 and R2)
    write_test_files({"ATCGAA", "ATCGTT", "ATCGCC"});

    gzFile read1_file = gzopen("test_R1.fastq.gz", "rb");
    gzFile read2_file = gzopen("test_R2.fastq.gz", "rb");
    std::unordered_set<std::string> allowed_barcodes = {"ATCGAA", "ATCGTT"};
    demuxator(read1_file, read2_file, allowed_barcodes, 7, 6, true);

    // Check output file content 
    check_output_file_content("ATCGAA");
    check_output_file_content("ATCGTT");
    ASSERT_FALSE(file_exists("ATCGCC_R1.fastq.gz"));
    ASSERT_FALSE(file_exists("ATCGCC_R2.fastq.gz"));

    // Clean up the test files
    std::string command = "rm test_R1.fastq.gz test_R2.fastq.gz ATCGAA_R1.fastq.gz ATCGAA_R2.fastq.gz ATCGTT_R1.fastq.gz ATCGTT_R2.fastq.gz";
    std::system(command.c_str());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}