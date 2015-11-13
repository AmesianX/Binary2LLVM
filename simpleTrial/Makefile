CXX=g++

LIBS=-lcommon -ldyninstAPI -linstructionAPI
#CFLAGS=-L/home/chertio/research/DynInst/dyninst_install/lib
#CFLAGS=-L$(LD_LIBRARY_PATH)
# just ask to set the LIBRARY_PATH
OBJ=readBinary.o

%.o: %.cpp 
	$(CXX) -c $^ 

readBinary.out: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ *.out  

