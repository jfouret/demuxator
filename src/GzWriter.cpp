#include "GzWriter.h"

GzWriter::GzWriter(const std::string& file_path) : file(file_path), buffer(BUFFER_SIZE) {
    boost::iostreams::gzip_params gzip_params;
    gzip_params.level = COMPRESSION_LEVEL;

    out.push(boost::iostreams::gzip_compressor(gzip_params, BUFFER_SIZE));
    out.push(file);
    out.rdbuf()->pubsetbuf(buffer.data(), BUFFER_SIZE); // Set the buffer for the output stream
}

GzWriter::~GzWriter() {
    close();
}

void GzWriter::write_line(const std::string& line) {
    out << line << std::endl;
}

void GzWriter::close() {
    if (out.is_complete()) {
        out.pop();
    }
}
