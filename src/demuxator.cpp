#include "demuxator.h"

void demuxator(gzFile &read1_file, gzFile &read2_file,
    std::unordered_set<std::string> allowed_barcodes,
    int bc_start, int bc_length, bool remove_barcodes) {

    // Key: barcode, Value: a pair of output files for R1 and R2
    std::unordered_map<std::string, std::pair<gzFile, gzFile>> output_files;

    while (true) {
        // Read R1 and R2 lines from the input files

        std::string read2_identifier = read_gz_line(read2_file);
        std::string read2_sequence = read_gz_line(read2_file);
        std::string read2_plus_line = read_gz_line(read2_file);
        std::string read2_quality = read_gz_line(read2_file);

        // Break the loop if either R1 or R2 reaches the end of the file
        if (read2_identifier.empty()) {
            break;
        }

        // Extract the barcode from the R2 read
        std::string barcode = read2_sequence.substr(bc_start - 1, bc_length);

        // Skip the read if the barcode is not in the allowed_barcodes set
        if (allowed_barcodes.find(barcode) == allowed_barcodes.end()) {
            for (int i = 0 ; i < 4; i++) {
                read_gz_line(read1_file);
            }
            continue;
        }

        if (remove_barcodes) {
            int read2_size = read2_sequence.size();
            int read2_start = bc_start - 1 + bc_length;
            read2_sequence = read2_sequence.substr(read2_start, read2_size);
            read2_quality = read2_quality.substr(read2_start, read2_size);
        }

        std::string read1_identifier = read_gz_line(read1_file);
        std::string read1_sequence = read_gz_line(read1_file);
        std::string read1_plus_line = read_gz_line(read1_file);
        std::string read1_quality = read_gz_line(read1_file);

        // Check if the output files for this barcode have been created
        auto it = output_files.find(barcode);
        if (it == output_files.end()) {
            // If not, create the output files and add them to the output_files map
            std::string output_file_name_r1 = barcode + "_R1.fastq.gz";
            std::string output_file_name_r2 = barcode + "_R2.fastq.gz";
            gzFile output_file_r1 = gzopen(output_file_name_r1.c_str(), "wt");
            gzFile output_file_r2 = gzopen(output_file_name_r2.c_str(), "wt");
            assert(output_file_r1);
            assert(output_file_r2);
            output_files[barcode] = {output_file_r1, output_file_r2};
            it = output_files.find(barcode);
        }

        // Write the R1 and R2 reads to their respective output files
        gzFile output_file_r1 = it->second.first;
        gzFile output_file_r2 = it->second.second;
        gzputs(output_file_r1, read1_identifier.c_str());
        gzputc(output_file_r1, '\n');
        gzputs(output_file_r1, read1_sequence.c_str());
        gzputc(output_file_r1, '\n');
        gzputs(output_file_r1, read1_plus_line.c_str());
        gzputc(output_file_r1, '\n');
        gzputs(output_file_r1, read1_quality.c_str());
        gzputc(output_file_r1, '\n');

        gzputs(output_file_r2, read2_identifier.c_str());
        gzputc(output_file_r2, '\n');
        gzputs(output_file_r2, read2_sequence.c_str());
        gzputc(output_file_r2, '\n');
        gzputs(output_file_r2, read2_plus_line.c_str());
        gzputc(output_file_r2, '\n');
        gzputs(output_file_r2, read2_quality.c_str());
        gzputc(output_file_r2, '\n');
    }

    // Close all output files
    for (auto &entry : output_files) {
        gzclose(entry.second.first);
        gzclose(entry.second.second);
    }

    // Close input files
    gzclose(read1_file);
    gzclose(read2_file);
}