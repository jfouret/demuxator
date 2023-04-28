#include "GzReader.h"

GzReader::GzReader(const std::string& in_path) : file_path(boost::filesystem::absolute(in_path).string()), file(in_path) {
    in.push(boost::iostreams::gzip_decompressor());
    in.push(file);
}

GzReader::~GzReader() {
    close();
}

std::string GzReader::read_line() {
    std::string line;
    std::getline(in, line);
    return line;
}

void GzReader::skip_line() {
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void GzReader::close() {
    if (in.is_complete()) {
        in.pop();
        in.pop();
    }
    if (file.is_open()) {
        file.close();
    }
}

void GzReader::rewind() {
    close();
    file.open(file_path);
    in.push(boost::iostreams::gzip_decompressor());
    in.push(file);
}
