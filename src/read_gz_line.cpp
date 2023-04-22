#include "read_gz_line.h"

std::string read_gz_line(gzFile &file) {
    std::string result;
    int ch = gzgetc(file);
    while (ch != -1 && ch != '\n') {
        result += static_cast<char>(ch);
        ch = gzgetc(file);
    }
    return result;
}

