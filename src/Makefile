#opt: OPTFLAGS = -O2
#export OPTFLAGS

CC		= gcc
CCC		= g++
CFLAGS		= -Wall -g -fPIC
CFLAGS		+= $(OPTFLAGS)
LDFLAGS		= $(GCOV_FLAGS)
LIBDIR = ../lib

H_DIR			= -I../inc
TARGET_LIB = libstreamfuzzyhash.a
TARGET_SO = libstreamfuzzyhash.so 

OBJS = interval_index.o stream_fuzzy_hash.o rbtree.o sfh_compare.o
.c.o:
	$(CC) -c $(CFLAGS) -I. $(H_DIR) $<

.cpp.o:
	$(CCC) -c $(CFLAGS) -I. $(H_DIR) $<

all: $(TARGET_LIB) $(TARGET_SO)

$(TARGET_SO): $(OBJS)
	$(CCC) -o $(TARGET_SO) -shared $(OBJS)
	cp $(TARGET_SO) ../lib/
$(TARGET_LIB): $(OBJS)
	echo  making dynamic lib ...
	ar cqs $(TARGET_LIB) $(OBJS)
	cp $(TARGET_LIB) ../lib/
clean:
	rm -rf *.o *.gcov *.gcno $(TARGET_SO) $(TARGET_LIB) *~

opt:
	$(MAKE) all
