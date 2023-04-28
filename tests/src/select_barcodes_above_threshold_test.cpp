#include "gtest/gtest.h"
#include "../include/select_barcodes_above_threshold.h"
#include <sstream>
#include <zlib.h>
#include <random>

class SelectBarcodesAboveThresholdTest : public ::testing::Test {
protected:
    void SetUp() override {
        create_test_file("test1_read2.fastq.gz", 10, 50, 10000, {"ATCGAA", "ATCGTT", "ATCGCC"}, {0.5, 0.499, 0.001});
        create_test_file("test2_read2.fastq.gz", 10, 50, 10000, {"ATCGAA", "ATCGTT"}, {0.5, 0.5});
    }

    void create_test_file(const std::string& file_path, const int bc_start,
      const int read_size, const int n_reads,
      const std::vector<std::string>& barcodes,
      const std::vector<double>& barcode_freq){

      std::discrete_distribution<int> bdis(barcode_freq.begin(), barcode_freq.end());
      std::uniform_int_distribution<> udis(0, 3);
      std::vector<char> bases = {'A', 'T', 'C', 'G'};
      int idx;

      // create a random number generator using a random device as a seed
      std::random_device rd;
      std::mt19937 gen(rd());

      int n = 50;
      char c = '!';
      std::string s_quals(n, c);

      std::ostringstream test_data;

      for (int i = 0; i < n_reads; i++) {
        test_data << "@" << i << std::endl;
        for (int j = 1 ; j < bc_start; j++) {
          test_data << bases[udis(gen)];
        }
        idx = bdis(gen);
        test_data << barcodes[idx];
        for (int j = bc_start + barcodes[idx].size() ; j <= read_size; j++) {
          test_data << bases[udis(gen)];
        }
        test_data << std::endl << "+" << std::endl << s_quals << std::endl;
      }

      gzFile test_file = gzopen(file_path.c_str(), "wb");
      std::string test_data_str = test_data.str();
      gzwrite(test_file, test_data_str.c_str(), test_data_str.length());
      gzclose(test_file);
    };

    void TearDown() override {
      // remove("test1_read2.fastq.gz");
      remove("test2_read2.fastq.gz");
    }
};

void test_select_barcodes_above_threshold(const std::string& file_path,
  int bc_start, int bc_length, double threshold,
  std::unordered_set<std::string> expected,
  std::unordered_set<std::string> unexpected) {

  GzReader file (file_path);

  std::unordered_set<std::string> selected_barcodes = select_barcodes_above_threshold(file, bc_start, bc_length, threshold, 5000);

  for (const auto& barcode : expected) {
      EXPECT_TRUE(selected_barcodes.find(barcode) != selected_barcodes.end()) << "Expected barcode " << barcode << " not found.";
  }

  for (const auto& barcode : unexpected) {
      EXPECT_TRUE(selected_barcodes.find(barcode) == selected_barcodes.end()) << "Unexpected barcode " << barcode << " found.";
  }

  std::string first_line = file.read_line();

  EXPECT_TRUE(first_line == "@0") << "File not rewind: " << first_line;

  file.close();

}

TEST_F(SelectBarcodesAboveThresholdTest, SelectBarcodesAboveThresholdTest) {
    test_select_barcodes_above_threshold("test1_read2.fastq.gz", 10, 6, 0.01,
        {"ATCGAA", "ATCGTT"},
        {"ATCGCC"});

    test_select_barcodes_above_threshold("test1_read2.fastq.gz", 10, 6, 0.0001,
        {"ATCGAA", "ATCGTT","ATCGCC"},
        {"ATCGGG"});

    test_select_barcodes_above_threshold("test2_read2.fastq.gz", 10, 6, 0.01,
        {"ATCGAA", "ATCGTT"},
        {"ATCGCC"});
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
