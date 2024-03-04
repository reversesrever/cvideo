CC = gcc
AR = ar
TARGET = libcvideo.a
OBJ = cvideo.o
SOURCES = cvideo.c
HEADERS = cvideo.h

$(TARGET): $(OBJ)
	@ $(AR) -rcs $(TARGET) $(OBJ)

$(OBJ): $(SOURCES) $(HEADERS)
	@ $(CC) $(SOURCES) -c -o $(OBJ)