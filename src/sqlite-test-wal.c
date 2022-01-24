#include <sqlite3.h>
#include <stdio.h>
#include <unistd.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(int argc, char *argv[])
{
  sqlite3 *db;
  int rc;
  char *zErrMsg = 0;

  rc = sqlite3_initialize();
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to initialize SQLite: %s", sqlite3_errstr(rc));
    return 1;
  }

  rc = sqlite3_open_v2("/tmp/sqlite-test-wal.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_AUTOPROXY, NULL);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  rc = sqlite3_exec(db, "PRAGMA temp_store = MEMORY;", callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  rc = sqlite3_exec(db, "PRAGMA journal_mode=WAL;", callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS storage (storage_id INTEGER, storage_content TEXT NOT NULL);", callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  /* rc = sqlite3_exec(db, "BEGIN TRANSACTION;", callback, 0, &zErrMsg); */
  /* if (rc != SQLITE_OK) { */
  /*   fprintf(stderr, "SQL error: %s\n", zErrMsg); */
  /*   sqlite3_free(zErrMsg); */
  /* } */

  do {
    rc = sqlite3_exec(db, "INSERT INTO storage (storage_id, storage_content) VALUES (3, 'Other info content');", callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    sleep(2);
  } while(1);

  /* rc = sqlite3_exec(db, "COMMIT;", callback, 0, &zErrMsg); */
  /* if (rc != SQLITE_OK) { */
  /*   fprintf(stderr, "SQL error: %s\n", zErrMsg); */
  /*   sqlite3_free(zErrMsg); */
  /* } */

  /* sqlite3_close(db); */

  return 0;
}
