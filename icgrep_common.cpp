#include "icgrep_common.h"
#include <util/aligned_allocator.h>

using namespace llvm;
using namespace buffer;

void icgrep_grep(const char * regex, AlignedBuffer<char> * buffer, bool * matchFound) {

    char * strBuffer = buffer->aligned_ptr();
    const size_t sizeStr = strlen(strBuffer) + 1;
    re::RE * reAST = re::RE_Parser::parse(std::string(regex));
    bool foundMatch = grep::matchOnlyGrep(reAST, strBuffer, sizeStr);

    *matchFound = foundMatch;
}

std::vector<uint64_t> icgrep_greplines(const char * regex, AlignedBuffer<char> * buffer, const size_t length) {
  re::RE * reAST = re::RE_Parser::parse(std::string(regex));
  return grep::lineNumGrep(reAST, buffer->aligned_ptr(), length);
}

