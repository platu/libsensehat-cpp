# -----------------------------------------------------------
CXXFLAGS= -Wall -Wextra -Werror -pedantic -std=gnu++2a
LIBLIST= -lsensehat-c++ -lpng -li2c -lm -lRTIMULib -lgpiod
LIBDEP=/usr/local/lib/libsensehat-c++.so.0

# -----------------------------------------------------------
# C++ language test programs
CXX_SOURCES=$(wildcard *.cpp)
CXX_TARGETS=$(CXX_SOURCES:.cpp=.o) 

all: ${CXX_TARGETS}

%.o : %.cpp ${LIBDEP}
	${CXX} ${CXXFLAGS} -o $@ $< ${LIBLIST}

# -----------------------------------------------------------
clean:
	rm -f *o

