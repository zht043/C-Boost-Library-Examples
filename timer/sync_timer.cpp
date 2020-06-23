#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;
int main(int argc, char* argv[]) {
    //https://stackoverflow.com/questions/3708706/how-to-determine-the-boost-version-on-a-system
    cout << "Using Boost "     
          << BOOST_VERSION / 100000     << "."  // major version
          << BOOST_VERSION / 100 % 1000 << "."  // minor version
          << BOOST_VERSION % 100                // patch level
          << endl;

    cout << "===<boost library synchronous(blocking mode) timer>===" << endl;

    /* Example 1: get current time & time duration in millisec */
    boost::posix_time::ptime t1, t2; 
    /* clock choice is system dependent, for millisec time resolution, 
       you need clock that is at least millisec resolution or below */
    t1 = boost::posix_time::microsec_clock::local_time(); //1st measure 
    cout << "Total elapsed time of today till 1st measurement in millisec: " <<t1.time_of_day().total_milliseconds() << endl;
    sleep(2);
    t2 = boost::posix_time::microsec_clock::local_time(); //2nd measure
    cout << "Total elapsed time of today till 2nd measurement in millisec: " << t2.time_of_day().total_milliseconds() << endl;

    boost::posix_time::time_duration dt;
    dt = t2 - t1;
    cout << "Time duration during the delay(sleep) in s: " << dt.total_seconds() << endl;
    cout << "Time duration during the delay(sleep) in ms: " << dt.total_milliseconds() << endl;
    cout << "Time duration during the delay(sleep) in us: " << dt.total_microseconds() << endl;
    cout << "Time duration during the delay(sleep) in ns: " << dt.total_nanoseconds() << endl;
    
    cout << endl;

    /* Example 2: sync delay/wait (deadline timer vs steady timer: essentially 
                                    the same thing with minor differences) */
    boost::asio::io_service ios;

    boost::asio::deadline_timer timer(ios);

    t1 = boost::posix_time::microsec_clock::local_time(); //1st measure 
    timer.expires_from_now(boost::posix_time::milliseconds(500)); //config time length to wait
    timer.wait(); // blocking wait : during wait time, current process is blocked from doing anything else
    t2 = boost::posix_time::microsec_clock::local_time(); //2nd measure
    cout << "Deadline timer just waited 500 milliseconds" << endl;
    cout << "Measured duration: " << boost::posix_time::time_duration(t2-t1).total_milliseconds() 
                                  << "ms" << endl;

    t1 = boost::posix_time::microsec_clock::local_time(); //1st measure 
    timer.expires_from_now(boost::posix_time::seconds(3)); //config time length to wait
    timer.wait(); // blocking wait : during wait time, current process is blocked from doing anything else
    t2 = boost::posix_time::microsec_clock::local_time(); //2nd measure
    cout << "Deadline timer just waited 3 seconds" << endl;
    cout << "Measured duration: " << boost::posix_time::time_duration(t2-t1).total_seconds() 
                                  << "sec" << endl;
    return 0;
}