/*
    This example demonstrates how SQLite behaves
    when memory is low.
    It shows how heap defragmentation causes
    out of memory and how to avoid it.
    At first it asks how much memory to occupy
    so as not be available to SQLite.  Then
    tries to insert huge number of records
    and shows how much free memory available
    after each insert.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "SD_MMC.h"

char *dat = NULL;
void block_heap(int times) {

  while (times--) {
     dat = (char *) malloc(4096);
  }

}

const char* data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   Serial.printf("%s: ", (const char*)data);
   for (i = 0; i<argc; i++){
       Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   Serial.printf("\n");
   return 0;
}

int openDb(const char *filename, sqlite3 **db) {
   int rc = sqlite3_open(filename, db);
   if (rc) {
       Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
       return rc;
   } else {
       Serial.printf("Opened database successfully\n");
   }
   return rc;
}

char *zErrMsg = 0;
int db_exec(sqlite3 *db, const char *sql) {
   Serial.println(sql);
   long start = micros();
   int rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if (rc != SQLITE_OK) {
       Serial.printf("SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
   } else {
       Serial.printf("Operation done successfully\n");
   }
   Serial.print(F("Time taken:"));
   Serial.println(micros()-start);
   return rc;
}

int input_string(char *str, int max_len) {
  max_len--;
  int ctr = 0;
  str[ctr] = 0;
  while (str[ctr] != '\n') {
    if (Serial.available()) {
        str[ctr] = Serial.read();
        if (str[ctr] >= ' ' && str[ctr] <= '~')
          ctr++;
        if (ctr >= max_len)
          break;
    }
  }
  str[ctr] = 0;
  Serial.println(str);
}

int input_num() {
  char in[20];
  int ctr = 0;
  in[ctr] = 0;
  while (in[ctr] != '\n') {
    if (Serial.available()) {
        in[ctr] = Serial.read();
        if (in[ctr] >= '0' && in[ctr] <= '9')
            ctr++;
        if (ctr >= sizeof(in))
          break;
    }
  }
  in[ctr] = 0;
  int ret = atoi(in);
  Serial.println(ret);
  return ret;
}

void displayPrompt(const char *title) {
  Serial.print(F("Enter "));
  Serial.println(title);
}

void displayFreeHeap() {
   Serial.printf("\nHeap size: %d\n", ESP.getHeapSize());
   Serial.printf("Free Heap: %d\n", esp_get_free_heap_size());
   Serial.printf("Min Free Heap: %d\n", esp_get_minimum_free_heap_size());
   Serial.printf("Max Alloc Heap: %d\n", ESP.getMaxAllocHeap());
}

char *random_strings[] = {"Hello world", "Have a nice day", "Testing memory problems", "This should work", "ESP32 has 512k RAM", "ESP8266 has only 36k user RAM", 
     "A stitch in time saves nine", "Needle in a haystack", "Too many strings", "I am done"};
char sql[1024];
sqlite3 *db1;
sqlite3_stmt *res;
const char *tail;
int rc;

void setup() {
   Serial.begin(115200);

   randomSeed(analogRead(0));

   SPI.begin();
   SD_MMC.begin();

   displayFreeHeap();
   displayPrompt("No. of 4k heap to block:");
   block_heap(input_num());
   displayFreeHeap();

   sqlite3_initialize();

}

void loop() {

   // Open database 1
   if (openDb("/sdcard/test_bulk_insert.db", &db1))
     return;

   displayFreeHeap();
  
   rc = db_exec(db1, "CREATE TABLE IF NOT EXISTS test (c1 INTEGER, c2, c3, c4, c5 INTEGER, c6 INTEGER, c7, c8, c9 DATETIME, c10 DATETIME, c11 INTEGER )");
   if (rc != SQLITE_OK) {
     sqlite3_close(db1);
     return;
   }

   displayFreeHeap();

   int rec_count;
   displayPrompt("No. of records to insert:");
   rec_count = input_num();
   char *sql = "INSERT INTO test VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
   rc = sqlite3_prepare_v2(db1, sql, strlen(sql), &res, &tail);
   if (rc != SQLITE_OK) {
     Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(db1));
     sqlite3_close(db1);
     return;
   }
   char *value;
   while (rec_count--) {
      sqlite3_bind_int(res, 1, random(65535));
      value = random_strings[random(10)];
      sqlite3_bind_text(res, 2, value, strlen(value), SQLITE_STATIC);
      value = random_strings[random(10)];
      sqlite3_bind_text(res, 3, value, strlen(value), SQLITE_STATIC);
      value = random_strings[random(10)];
      sqlite3_bind_text(res, 4, value, strlen(value), SQLITE_STATIC);
      sqlite3_bind_int(res, 5, random(65535));
      sqlite3_bind_int(res, 6, random(65535));
      value = random_strings[random(10)];
      sqlite3_bind_text(res, 7, value, strlen(value), SQLITE_STATIC);
      value = random_strings[random(10)];
      sqlite3_bind_text(res, 8, value, strlen(value), SQLITE_STATIC);
      sqlite3_bind_int(res, 9, random(100000000L));
      sqlite3_bind_int(res, 10, random(100000000L));
      sqlite3_bind_int(res, 11, random(65535));
      if (sqlite3_step(res) != SQLITE_DONE) {
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(db1));
        sqlite3_close(db1);
        return;
      }
      sqlite3_clear_bindings(res);
      rc = sqlite3_reset(res);
      if (rc != SQLITE_OK) {
        sqlite3_close(db1);
        return;
      }
      displayFreeHeap();
   }
   sqlite3_finalize(res);
   Serial.write("\n");

   rc = db_exec(db1, "Select count(*) from test");
   if (rc != SQLITE_OK) {
     sqlite3_close(db1);
     return;
   }

   sqlite3_close(db1);
   displayFreeHeap();

   displayPrompt("Press enter to continue:");
   input_num();

}

