#pragma once
#include <string>
#include <vector>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem.hpp>

class GzReader {
public:
    explicit GzReader(const std::string& in_path);
    ~GzReader();

    std::string read_line();
    void skip_line();
    void close();
    void rewind();

private:
    static const size_t BUFFER_SIZE = 100 * 1024 * 1024; // 100M buffer size
    std::string file_path; // Store the file path as a member variable
    boost::iostreams::file_source file;
    std::vector<char> buffer; // Create a buffer of size 100M
    boost::iostreams::filtering_istream in;
};
