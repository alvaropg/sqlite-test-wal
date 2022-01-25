#include <sqlite3.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

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

  srand(time(NULL));
  do {
    int storage_id = rand();
    char query[1500];
    snprintf(query, 1499, "INSERT INTO storage (storage_id, storage_content) VALUES (%d, 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent sit amet nisl a nisl mattis venenatis vel ac mauris. Nunc ipsum velit, consequat et ex ac, consectetur cursus ex. Nam vel dolor placerat, efficitur nisl a, efficitur urna. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Curabitur lobortis, nunc eget scelerisque efficitur, tortor ligula pulvinar justo, et aliquet eros urna ut neque. Sed pharetra accumsan interdum. Nunc facilisis maximus eros, et consectetur quam ultricies a. Duis eleifend risus id magna tristique lacinia. Integer pharetra enim sollicitudin magna convallis volutpat. Mauris eu dui non quam vehicula lacinia non non massa. Curabitur id ultricies eros. Quisque malesuada venenatis elit rhoncus pretium. Duis lacinia libero velit, ac pellentesque augue facilisis et. Sed at fringilla leo. Donec iaculis risus quam, at efficitur lectus tincidunt pretium.');", storage_id);
    rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
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
