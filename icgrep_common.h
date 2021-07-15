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

#include <util/aligned_allocator.h>

namespace buffer {

enum : unsigned {
  MAX_SIMD_WIDTH_SUPPORTED   = 512U
};

template <class T>
using BufferAllocator = AlignedAllocator<T, (MAX_SIMD_WIDTH_SUPPORTED / (sizeof(T) * 8))>;

template <class T>
class AlignedBuffer
{
public:
  AlignedBuffer(const size_t length) { ptr = alloc.allocate(length, 0); }
  ~AlignedBuffer() { alloc.deallocate(ptr, 0); }
  void write(const size_t index, T element) { ptr[index] = element; }
  void writeData(const size_t begin, T * elements, const size_t length) { memcpy(ptr + begin, elements, length); }
  T* aligned_ptr() { return ptr; }

private:
    BufferAllocator<T> alloc;
    T* ptr;
};

} //alignedbuffer

void icgrep_grep(const char * regex, buffer::AlignedBuffer<char> * buffer, bool * matchFound);

std::vector<uint64_t> icgrep_greplines(const char * regex, buffer::AlignedBuffer<char> * buffer, const size_t length);

#endif // ICGREP_GREP_H
