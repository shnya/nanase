# Makefile
PROGRAM = nanase
OBJS = $(PROGRAM).o

CC = gcc
CFLAGS = -g -Wall -I/opt/local/include
CXX = g++
CXXFLAGS = -g -Wall -I/opt/local/include
LDLIBS = -L/opt/local/lib -ltokyocabinet
CHK_SOURCES = tcmanager.cc

.SUFFIXES: .cc .o
.SUFFIXES: .cpp .o

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $(PROGRAM) $^	

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<


.PHONY: clean
clean:
	$(RM) $(PROGRAM) $(OBJS)
	$(RM) *.idxdb *.docdb


.PHONY: check-syntax
check-syntax:
	$(CXX) $(CXXFLAGS) -Wall -fsyntax-only $(CHK_SOURCES)
