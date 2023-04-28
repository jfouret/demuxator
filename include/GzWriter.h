#pragma once
#include <string>
#include <vector>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>

class GzWriter {
public:
    explicit GzWriter(const std::string& file_path);
    ~GzWriter();

    void write_line(const std::string& line);
    void close();

private:
    static const size_t BUFFER_SIZE = 100 * 1024 * 1024; // 100M buffer size
    static const int COMPRESSION_LEVEL = 4;
    boost::iostreams::file_sink file;
    std::vector<char> buffer; // Create a buffer of size 100M
    boost::iostreams::filtering_ostream out;
};
