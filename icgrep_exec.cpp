// For tests only
//

#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include "icgrep_common.h"

using namespace buffer;

namespace fs = boost::filesystem;
int main (int argc, char **argv) {
    bool res;
    char * inputBool = argv[2];
    const size_t lengthInputBool = strlen(inputBool);
    buffer::AlignedBuffer<char> alignedBufferBool(lengthInputBool + 1);
    alignedBufferBool.writeData(0, inputBool, lengthInputBool);
    icgrep_grep(argv[1], &alignedBufferBool, &res);
    printf("%d\n", res);

    std::ifstream t(argv[3]);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    char * inputLines = (char *) str.c_str();
    const size_t lengthInputLines = str.length();
    buffer::AlignedBuffer<char> alignedBufferLines(lengthInputLines + 1);
    alignedBufferLines.writeData(0, inputLines, lengthInputLines);
    std::vector<uint64_t> v = icgrep_greplines(argv[1], &alignedBufferLines, lengthInputLines + 1);
    printf("size: %ld\n", v.size()); 
}

