# -----------------------------------------------------------
CXXFLAGS= -Wall -Wextra -Werror -pedantic -std=gnu++2a

PREFIX=/usr/local
LIBDIR=${PREFIX}/lib
INCLUDEDIR=${PREFIX}/include

# -----------------------------------------------------------
#  jobs
all: install cxx_examples

install: includes images library

# -----------------------------------------------------------
# include files
INCLUDES=$(wildcard include/*.h)
INCLUDES_DIST=$(addprefix ${INCLUDEDIR}/, $(notdir ${INCLUDES}))

includes: $(INCLUDES_DIST)

${INCLUDEDIR}/%: %
	sudo cp $^ ${INCLUDEDIR}/

vpath %.h include

# -----------------------------------------------------------
# image files
IMAGES=$(wildcard src/sense_hat_text.*)
IMAGES_DIST=$(addprefix ${LIBDIR}/, $(notdir ${IMAGES}))

images: $(IMAGES_DIST)

${IMAGES_DIST}: ${IMAGES}
	sudo cp $? ${LIBDIR}

# -----------------------------------------------------------
# library files
SOURCE_NAME=sensehat
LIBRARY_NAME=lib${SOURCE_NAME}-c++.so
LIBRARY_SOURCE=src/${SOURCE_NAME}.cpp
OBJECT=$(LIBRARY_SOURCE:.cpp=.o)
LIBRARY=lib/${LIBRARY_NAME}
LIBRARY_DIST=${LIBDIR}/$(notdir ${LIBRARY})

library: ${LIBRARY_DIST}.0

${OBJECT}: ${LIBRARY_SOURCE} ${INCLUDES}
	${CXX} ${CXXFLAGS} -c -fPIC -o $@ $< -lpng

${LIBRARY}.0: ${OBJECT}
	${CXX} ${CXXFLAGS} -g -shared -Wl,-soname,${LIBRARY_NAME} -o $@ $<

${LIBRARY_DIST}.0: ${LIBRARY}.0
	sudo cp $< $@
	sudo sh -c "cd ${LIBDIR} && ln -sf ${LIBRARY_NAME}.0 ${LIBRARY_NAME}"
	sudo ldconfig

# -----------------------------------------------------------
#  examples
cxx_examples:
	cd examples && $(MAKE)

# -----------------------------------------------------------
clean:
	rm -f *o src/*.o lib/*.so*

