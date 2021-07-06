#include <sqlite3ext.h> /* Do not use <sqlite3.h>! */
#include "icgrep_common.h"
SQLITE_EXTENSION_INIT1

#define SIZE_BUFFER (1024 * 1024 * 100)

typedef struct {
  char *buffer;
  char *regex;
  size_t length;
  bool hasGrep;
  std::vector<uint64_t> lines;
} IcgrepInfo;

static void icgrepCreateBuffer(IcgrepInfo *info, const char *regex) {
  if (info && !info->buffer) {
    info->buffer = (char *) malloc(sizeof(char) * SIZE_BUFFER);
    info->regex = (char *) malloc(sizeof(char) * (strlen(regex) + 1));
    strcpy(info->regex, regex);
  }
}

static void icgrepDeleteBuffer(IcgrepInfo *info) {
  if (info && info->buffer) {
    free(info->buffer);
    free(info->regex);
    info->regex = NULL;
    info->buffer = NULL;
  }
}

static void icgrepDoGrep(IcgrepInfo *info) {
  if (info && !info->hasGrep) {
    info->hasGrep = true;
    info->lines = icgrep_greplines(info->regex, info->buffer, info->length);
  }
}

static void icgrepXStep(sqlite3_context *context, int argc, sqlite3_value **argv) {
  IcgrepInfo *info = (IcgrepInfo *)sqlite3_aggregate_context(context, sizeof(IcgrepInfo));

  const char * regex = (const char *)sqlite3_value_text(argv[0]);
  const char * strToSearch = (const char *)sqlite3_value_text(argv[1]);
  const size_t sizeStr = strlen((char const *)strToSearch);
  icgrepCreateBuffer(info, regex);

  if (info) {
    memcpy(info->buffer + info->length, strToSearch, sizeStr);
    info->buffer[info->length + sizeStr] = '\n';
    info->length += sizeStr + 1;
  }
}

static void icgrepXInverse(sqlite3_context *context, int argc, sqlite3_value **argv) {
  IcgrepInfo *info = (IcgrepInfo *)sqlite3_aggregate_context(context, sizeof(IcgrepInfo));

  const char * regex = (const char *)sqlite3_value_text(argv[0]);
  const char * strToSearch = (const char *)sqlite3_value_text(argv[1]);
  const size_t sizeStr = strlen((char const *)strToSearch);
  icgrepCreateBuffer(info, regex);

  if (info) {
    info->length -= sizeStr + 1;
  }
}

static void icgrepXFinal(sqlite3_context *context) {
  IcgrepInfo *info = (IcgrepInfo *)sqlite3_aggregate_context(context, 0);
  if (info) {
    icgrepDoGrep(info);
  }
  icgrepDeleteBuffer(info);
  std::ostringstream vts;
  if (!info->lines.empty())
  {
    auto vec = info->lines;
    std::copy(vec.begin(), vec.end()-1,  std::ostream_iterator<int>(vts, ", "));
    vts << vec.back();
    sqlite3_result_text(context, vts.str().c_str(), strlen(vts.str().c_str()), NULL);
  } else {
    sqlite3_result_int(context, 0);
  }
}

static void icgrepXValue(sqlite3_context *context) {
  IcgrepInfo *info = (IcgrepInfo *)sqlite3_aggregate_context(context, 0);
  if (info) {
    icgrepDoGrep(info);
  }
  icgrepDeleteBuffer(info);

  std::ostringstream vts;
  if (!info->lines.empty())
  {
    auto vec = info->lines;
    std::copy(vec.begin(), vec.end()-1,  std::ostream_iterator<int>(vts, ", "));
    vts << vec.back();
    sqlite3_result_text(context, vts.str().c_str(), strlen(vts.str().c_str()), NULL);
  } else {
    sqlite3_result_int(context, 0);
  }
}

extern "C" {

int sqlite3IcgrepwInit(sqlite3 *db) {
  return sqlite3_create_window_function(db, "icgrepw", 2, SQLITE_UTF8|SQLITE_INNOCUOUS, 0,
         icgrepXStep, icgrepXFinal, icgrepXValue, icgrepXInverse, 0);
}

#ifndef SQLITE_CORE
#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_icgrepwindowext_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi) {
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg;  /* Unused parameter */
  return sqlite3IcgrepwInit(db);
}
#endif // SQLITE_CORE

} // extern "C"
