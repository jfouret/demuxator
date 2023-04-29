#include "GzReader.h"

GzReader::GzReader(const std::string& in_path) : file_path(boost::filesystem::absolute(in_path).string()), file(in_path), buffer(BUFFER_SIZE) {

    in.push(boost::iostreams::gzip_decompressor(15, BUFFER_SIZE));
    in.push(file);
    in.rdbuf()->pubsetbuf(buffer.data(), BUFFER_SIZE); // Set the buffer for the input stream
}

GzReader::~GzReader() {
    close();
}

std::string GzReader::read_line() {
    in.getline(line_buffer, sizeof(line_buffer));
    std::string line(line_buffer);
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
    in.rdbuf()->pubsetbuf(buffer.data(), BUFFER_SIZE); // Set the buffer for the input stream
}
