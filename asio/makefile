
default: async_timer.exe backgnd_async.exe ip_query.exe basic_socket.exe \
         sync_tcp_server.exe async_tcp_server.exe
#multithreading_strand.exe

boostlib = -lboost_system -lboost_thread -lpthread 

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