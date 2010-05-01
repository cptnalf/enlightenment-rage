
CC=gcc
CFLAGS=-g `pkg-config evas ecore ecore-file --cflags` -I../../ -I../bin/
LIBS=`pkg-config evas ecore ecore-file --libs` -lsqlite3

database: core iterator queries

core: test-dbcore
	./test-dbcore

create_db:
	rm -f test_media.db
	
	sqlite3 -bail test_media.db < load.sql 

iterator: test-dbiterator create_db
	./test-dbiterator

queries: test-dbqueries create_db
	./test-queries

test-dbcore: test_dbcore.o ../bin/database.o ../bin/volume.o
	$(CC) -o $@ -g $(LIBS) $^

test-dbiterator: test_dbiterator.o ../bin/database.o ../bin/volume.o
	$(CC) -o $@ -g $(LIBS) $^

test-dbqueries: test_dbqueries.o ../bin/database.o ../bin/volume.o
	$(CC) -o $@ -g $(LIBS) $^
