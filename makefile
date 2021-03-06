CXX=g++
CXXFLAGS=-std=c++14 -Wall -MMD
EXEC=vm
OBJECTS=main.o ncurseView.o model.o event.o file.o line.o vm.o keyboard.o 
DEPENDS=${OBJECTS:.o=.d}

${EXEC}:${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC} -lncurses
	
-include ${DEPENDS}

.PHONY:clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}


