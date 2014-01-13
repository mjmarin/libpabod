#############################################
######## PArt Based Object Detection ########
########         LibPaBOD            ########
#############################################

CXX 		= g++
CC 			= $(CXX)
CPPFLAGS 	= -Wall -O3
CXXFLAGS 	= $(INCLUDE) `pkg-config --libs --cflags matio opencv`
SOURCES 	= ./src/
HEADERS 	= ./include/
OBJS		= ./obj/
TESTS 		= ./tests/
LIBS 		= ./libs/
BIN 		= ./bin/
DOC			= ./doc/
INCLUDE 	= -I$(HEADERS)
LIBRARY		= -L$(LIBS)
CPP_OBJS 	= $(OBJS)makeDetection.o $(OBJS)pabod.o $(OBJS)model.o $(OBJS)cell.o $(OBJS)readTypes.o $(OBJS)imgdetect.o $(OBJS)featPyramid.o $(OBJS)handlerOpenCVStructs.o $(OBJS)gdetect.o $(OBJS)fconvThreads.o $(OBJS)modelSort.o $(OBJS)dt.o $(OBJS)getDetections.o $(OBJS)nms.o
CPP_TEST 	= $(TESTS)mainTest.cpp
CPP_TEST2 	= $(TESTS)detectobj2.cpp
CPP_TEST3 	= $(TESTS)detectvid.cpp
LIBA		= $(LIBS)libpabod.a
DOXYFILE	= $(DOC)Doxyfile

# Create all files
all: $(LIBA) alltests

# Create the lib using object files and header files
$(LIBA): $(CPP_OBJS)
	@echo Building library...
	@ar rs $(LIBA) $(CPP_OBJS)

# Generate object files (*.o) of each source file
$(OBJS)%.o : $(SOURCES)%.cpp $(HEADERS)%.h
	@echo Building $@...
	@$(CC) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

# Create the executable test file using lib
alltests: test test2 test3

test: $(CPP_TEST) $(LIBA)
	@echo Building mainTest executable file...
	@$(CC) $(CPP_TEST) -o $(BIN)mainTest $(LIBRARY) -lpabod $(CPPFLAGS) $(CXXFLAGS)

test2: $(CPP_TEST2) $(LIBA)
	@echo Building detectobj executable file...
	@$(CC) $(CPP_TEST2) -o $(BIN)detectobj $(LIBRARY) -lpabod $(CPPFLAGS) $(CXXFLAGS)

test3: $(CPP_TEST3) $(LIBA)
	@echo Building detectvid executable file...
	@$(CC) $(CPP_TEST3) -o $(BIN)detectvid $(LIBRARY) -lpabod $(CPPFLAGS) $(CXXFLAGS)

# Create documentation in html and latex
docum:
	@doxygen $(DOXYFILE)

# Remove .o files
cleanobj:
	rm $(OBJS)*.o

