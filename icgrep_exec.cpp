// For tests only
//

#include <stdio.h>
#include "icgrep_common.h"

namespace fs = boost::filesystem;
int main (int argc, char **argv) {
    bool res;
    icgrep_grep(argv[1], argv[2], &res);
    printf("%d", res);
}

