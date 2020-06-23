
default: async.exe sync.exe chrono.exe
	@echo compilation completed

boostlib = -lboost_system -lboost_thread -lpthread -lboost_chrono
#-L/usr/lib/x86_64-linux-gnu

std = -std=c++17

cppflags = $(std) $(boostlib)
 
%.o: %.cpp
	g++ -c $< 

async.exe: async_timer.o
	g++ -o $@ $< $(cppflags) 

sync.exe: sync_timer.o
	g++ -o $@ $< $(cppflags) 

chrono.exe: chrono.o
	g++ -o $@ $< $(cppflags)