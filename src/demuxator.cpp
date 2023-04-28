#include "demuxator.h"

void demuxator(GzReader &read1_file, GzReader &read2_file,
    std::unordered_set<std::string> allowed_barcodes,
    int bc_start, int bc_length, bool remove_barcodes) {

    // Key: barcode, Value: a pair of output files for R1 and R2
    std::unordered_map<std::string, std::pair<std::shared_ptr<GzWriter>, std::shared_ptr<GzWriter>>> output_files;

    while (true) {
        // Read R1 and R2 lines from the input files

        std::string read2_identifier = read2_file.read_line();
        std::string read2_sequence = read2_file.read_line();

        // Break the loop if either R1 or R2 reaches the end of the file
        if (read2_identifier.empty()) {
            break;
        }

        // Extract the barcode from the R2 read
        std::string barcode = extract_barcode(read2_sequence, bc_start, bc_length);
        if (barcode.empty()) {
            continue;
        } 

        // Skip the read if the barcode is not in the allowed_barcodes set
        if (allowed_barcodes.find(barcode) == allowed_barcodes.end()) {
            for (int i = 0 ; i < 4; i++) {
                read1_file.skip_line();
            }
            read2_file.skip_line();
            read2_file.skip_line();
            continue;
        }

        std::string read2_plus_line = read2_file.read_line();
        std::string read2_quality = read2_file.read_line();

        if (remove_barcodes) {
            int read2_size = read2_sequence.size();
            int read2_start = bc_start - 1 + bc_length;
            read2_sequence = read2_sequence.substr(read2_start, read2_size);
            read2_quality = read2_quality.substr(read2_start, read2_size);
        }


        // Check if the output files for this barcode have been created
        auto it = output_files.find(barcode);
        if (it == output_files.end()) {
            // If not, create the output files and add them to the output_files map
            std::string output_file_name_r1 = barcode + "_R1.fastq.gz";
            std::string output_file_name_r2 = barcode + "_R2.fastq.gz";
            output_files.emplace(barcode, std::make_pair(std::make_shared<GzWriter>(output_file_name_r1), std::make_shared<GzWriter>(output_file_name_r2)));
            it = output_files.find(barcode);
        }

        // Write the R1 and R2 reads to their respective output files
        std::shared_ptr<GzWriter> output_file_r1 = it->second.first;
        std::shared_ptr<GzWriter> output_file_r2 = it->second.second;

        output_file_r2->write_line(read2_identifier);
        output_file_r2->write_line(read2_sequence);
        output_file_r2->write_line("+");
        output_file_r2->write_line(read2_quality);
        for (int i = 0 ; i < 4; i++) {
            output_file_r1->write_line(read1_file.read_line());
        }
    }

    // Close all output files
    for (auto &entry : output_files) {
        entry.second.first->close();
        entry.second.second->close();
    }

    // Close input files
    read1_file.close();
    read1_file.close();
}