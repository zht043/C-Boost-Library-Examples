
default: async_timer.exe backgnd_async.exe ip_query.exe basic_socket.exe \
         sync_tcp_server.exe async_tcp_server.exe sync_tcp_client.exe \
		 saturated_tcp_test.exe sync_udp_server.exe sync_udp_client.exe async_udp_server.exe

compiler = clang++
#compiler = g++

boostlib = -lboost_system -lboost_thread -lpthread 

std = -std=c++17

cppflags = $(std) $(boostlib)
 
%.o: %.cpp
	$(compiler) -c $< 

%.exe: %.o
	$(compiler) -o $@ $< $(cppflags) 
	@rm *.o
	@echo compilation completed

clean:
	@rm *.exe