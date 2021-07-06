#include <sqlite3ext.h> /* Do not use <sqlite3.h>! */
#include "icgrep_common.h"
SQLITE_EXTENSION_INIT1

typedef struct {
  char *buffer;
  size_t length;
} IcgrepInfo;

static void icgrepXStep(sqlite3_context *context,int argc,sqlite3_value **argv) {
  sqlite3_int64 *pInt;

  if (sqlite3_value_type(argv[0])!=SQLITE_INTEGER) {
    sqlite3_result_error(context, "invalid argument", -1);
    return;
  }

  pInt = (sqlite3_int64*)sqlite3_aggregate_context(context, sizeof(sqlite3_int64));
  if (pInt) {
    *pInt += sqlite3_value_int64(argv[0]);
  }
}

static void icgrepXInverse(sqlite3_context *context, int argc, sqlite3_value **argv) {
  sqlite3_int64 *pInt;
  pInt = (sqlite3_int64*)sqlite3_aggregate_context(context, sizeof(sqlite3_int64));
  *pInt -= sqlite3_value_int64(argv[0]);
}

static void icgrepXFinal(sqlite3_context *context) {
  sqlite3_int64 res = 0;
  sqlite3_int64 *pInt;
  pInt = (sqlite3_int64*)sqlite3_aggregate_context(context, 0);
  if (pInt) {
    res = *pInt;
  }
  sqlite3_result_int64(context, res);
}

static void icgrepXValue(sqlite3_context *context) {
  sqlite3_int64 res = 0;
  sqlite3_int64 *pInt;
  pInt = (sqlite3_int64*)sqlite3_aggregate_context(context, 0);
  if (pInt) {
     res = *pInt;
  }
  sqlite3_result_int64(context, res);
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
