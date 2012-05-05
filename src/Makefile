INCLUDE_DIR=../include
CC=g++
CFLAGS=-std=c++0x
LIBS=-lGL -lglfw -lGLEW

all: teste

teste: main.o Split.o SceneManager.o LuckWindow.o EventDispatcher.o Entity.o AssetManager.o
	$(CC) main.o Split.o SceneManager.o LuckWindow.o EventDispatcher.o Entity.o AssetManager.o -o teste $(CFLAGS) $(LIBS)

main.o: main.cpp
	$(CC) -c main.cpp $(CFLAGS) -I$(INCLUDE_DIR)

Split.o: Split.cpp
	$(CC) -c Split.cpp $(CFLAGS) -I$(INCLUDE_DIR)

SceneManager.o: SceneManager.cpp
	$(CC) -c SceneManager.cpp $(CFLAGS) -I$(INCLUDE_DIR)

LuckWindow.o: LuckWindow.cpp
	$(CC) -c LuckWindow.cpp $(CFLAGS) -I$(INCLUDE_DIR)

EventDispatcher.o: EventDispatcher.cpp
	$(CC) -c EventDispatcher.cpp $(CFLAGS) -I$(INCLUDE_DIR)

Entity.o: Entity.cpp
	$(CC) -c Entity.cpp $(CFLAGS) -I$(INCLUDE_DIR)

AssetManager.o: AssetManager.cpp
	$(CC) -c AssetManager.cpp $(CFLAGS) -I$(INCLUDE_DIR)

clean:
	rm -rf *o teste
