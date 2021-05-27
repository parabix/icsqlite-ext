#include <sqlite3ext.h> /* Do not use <sqlite3.h>! */
#include "icgrep_common.h"
SQLITE_EXTENSION_INIT1

static void icgrep_func(sqlite3_context *context, int argc, sqlite3_value **argv) {
    const char * regex = (const char *)sqlite3_value_text(argv[0]);
    const char * strToSearch = (const char *)sqlite3_value_text(argv[1]);
    bool matchFound;
    icgrep_grep(regex, strToSearch, &matchFound);
    sqlite3_result_int(context, static_cast<int>(matchFound));
}

extern "C" {

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_icgrepext_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi) {
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  rc = sqlite3_create_function(db, "icgrep", 2, SQLITE_UTF8|SQLITE_INNOCUOUS,
                               0, icgrep_func, 0, 0);
  return rc;
}

} // extern "C"
