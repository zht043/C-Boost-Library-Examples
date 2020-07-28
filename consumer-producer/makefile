
default: example.exe

compiler = clang++
#compiler = g++

boostlib = -lboost_system -lboost_thread -lpthread 

std = -std=c++17

cppflags = $(std) $(boostlib)
 
%.o: %.cpp
	$(compiler) -c $< 

example.exe: example.o
	$(compiler) -g -O0 -o  $@ example.o $(cppflags) 
	@rm *.o
	@echo compilation completed

clean:
	@rm -f *.exe
	@rm -f *.o