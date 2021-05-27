#include "icgrep_common.h"

using namespace llvm;

void icgrep_grep(const char * regex, const char * strToSearch, bool * matchFound) {

    const size_t sizeStr = strlen((char const *)strToSearch) + 1;
    re::RE * reAST = re::RE_Parser::parse(std::string(regex));
    bool foundMatch = grep::matchOnlyGrep(reAST, strToSearch, sizeStr);

    *matchFound = foundMatch;
}

