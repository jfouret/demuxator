#pragma once
#include <string>
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
    boost::iostreams::filtering_ostream out;
    boost::iostreams::file_sink file;
};
