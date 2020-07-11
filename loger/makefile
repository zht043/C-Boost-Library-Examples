
default: trivial_logger.exe logger_with_severity.exe logger_with_tags_format.exe

compiler = g++


boostlib = -DBOOST_LOG_DYN_LINK -lboost_log -lboost_system -lboost_thread -lpthread -lboost_chrono
std = -std=c++17

cppflags = $(std) $(boostlib)
 
%.o: %.cpp
	$(compiler) -c $< $(cppflags)

%.exe: %.o
	$(compiler) -o $@ $< $(cppflags)
	@rm *.o
	@echo compilation completed

clean:
	@rm *.exe