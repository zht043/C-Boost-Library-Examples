default: signal_slots.exe

boostlib = -lboost_system -lboost_thread -lpthread -lboost_chrono

std = -std=c++14

cppflags = $(std) $(boostlib)
 
%.o: %.cpp
	g++ -c $< 

%.exe: %.o
	g++ -o $@ $< $(cppflags) 
	@rm *.o
	@echo compilation completed

clean:
	@rm *.exe