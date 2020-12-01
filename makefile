CXX=g++
CXXFLAGS=-std=c++14 -Wall -MMD -lncurses
EXEC=test
OBJECTS=test.o
DEPENDS=${OBJECTS:.o=.d}

${EXEC}:${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}
	
-include ${DEPENDS}

.PHONY:clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
