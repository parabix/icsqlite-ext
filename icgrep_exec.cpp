// For tests only
//

#include <stdio.h>
#include "icgrep_common.h"
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>

namespace fs = boost::filesystem;
int main (int argc, char **argv) {
    bool res;
    icgrep_grep(argv[1], argv[2], &res);
    printf("%d\n", res);

    std::ifstream t(argv[3]);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    auto v = icgrep_greplines(argv[1], str.c_str(), str.length() + 1);
    printf("size: %ld\n", v.size());
}

