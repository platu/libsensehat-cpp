# -----------------------------------------------------------
CXXFLAGS= -Wall -Wextra -Wduplicated-cond -Wduplicated-branches -Wsign-conversion -Wlogical-op -Wuseless-cast -Werror -pedantic -std=gnu++23
LIBLIST= -lsensehat-c++ -lpng -li2c -lm -lRTIMULib -lgpiod
LIBDEP=/usr/local/lib/libsensehat-c++.so.0

# -----------------------------------------------------------
# C++ language test programs
CXX_SOURCES=$(wildcard *.cpp)
CXX_TARGETS=$(CXX_SOURCES:.cpp=) 

all: ${CXX_TARGETS}

%: %.cpp ${LIBDEP}
	${CXX} ${CXXFLAGS} -o $@ $< ${LIBLIST}

# -----------------------------------------------------------
clean:
	rm -f ${CXX_TARGETS}

