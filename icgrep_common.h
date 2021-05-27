#ifndef ICGREP_GREP_H
#define ICGREP_GREP_H

#include <cstdio>
#include <vector>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/raw_ostream.h>
#include <re/adt/re_alt.h>
#include <re/adt/re_seq.h>
#include <re/adt/re_start.h>
#include <re/adt/re_end.h>
#include <re/adt/re_utility.h>
#include <re/parse/parser.h>
#include <re/toolchain/toolchain.h>
#include <grep/grep_engine.h>
#include <fstream>
#include <string>
#include <toolchain/toolchain.h>
#include <toolchain/pablo_toolchain.h>
#include <boost/filesystem.hpp>
#include <fileselect/file_select.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <llvm/ADT/STLExtras.h> // for make_unique
#include <kernel/pipeline/driver/cpudriver.h>

void icgrep_grep(const char * regex, const char * strToSearch, bool * matchFound);

#endif // ICGREP_GREP_H
