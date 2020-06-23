
default: async_timer.exe sync_timer.exe chrono.exe


boostlib = -lboost_system -lboost_thread -lpthread -lboost_chrono
#-L/usr/lib/x86_64-linux-gnu

std = -std=c++17

cppflags = $(std) $(boostlib)
 
%.o: %.cpp
	g++ -c $< 

%.exe: %.o
	g++ -o $@ $< $(cppflags) 
	@rm *.o
	@echo compilation completed

clean:
	@rm *.exe