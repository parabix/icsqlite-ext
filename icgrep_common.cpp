#include "icgrep_common.h"

using namespace llvm;

void icgrep_grep(const char * regex, const char * strToSearch, bool * matchFound) {

    const size_t sizeStr = strlen((char const *)strToSearch) + 1;
    re::RE * reAST = re::RE_Parser::parse(std::string(regex));
    bool foundMatch = grep::matchOnlyGrep(reAST, strToSearch, sizeStr);

    *matchFound = foundMatch;
}

std::vector<uint64_t> icgrep_greplines(const char * regex, const char * strToSearch, const size_t length) {
  re::RE * reAST = re::RE_Parser::parse(std::string(regex));
  return grep::lineNumGrep(reAST, strToSearch, length);
}
