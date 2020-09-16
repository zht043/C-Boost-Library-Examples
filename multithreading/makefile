default: thread_and_mutex.exe misc_thread_sync_tests.exe condition_variable.exe \
thread_pool.exe

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