#pragma once
#include <string>
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
    std::string file_path; // Store the file path as a member variable
    boost::iostreams::filtering_istream in;
    boost::iostreams::file_source file;
};

