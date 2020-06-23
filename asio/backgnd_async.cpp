#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;
using namespace boost::posix_time;
using namespace boost::asio;


void timer_expire_callback(deadline_timer& timer) {
    cout << "<=============================>" << endl
         << "Timer expired!" << endl
         << "<=============================>"
         << endl;

    timer.expires_from_now(seconds(1));
    timer.async_wait(boost::bind(&timer_expire_callback, boost::ref(timer))); // register the next round
}

void background_task(io_service& service, ptime t0) {
    static auto dt = (microsec_clock::local_time() - t0).total_milliseconds();
    if((microsec_clock::local_time() - t0).total_milliseconds() - dt >= 100) { // print at every 100ms
        dt = (microsec_clock::local_time() - t0).total_milliseconds();
        cout << "Total timer elapsed: "
             << dt <<  " ms" << endl; // print current elapsed time every 100ms
    }
    service.post(boost::bind(&background_task, boost::ref(service), t0)); // this should create a loop
}

void terminate_service(io_service &service) {
    service.stop();
}

int main(int argc, char* argv[]) {

    io_service service;
    deadline_timer timer(service), stop_timer(service);

    ptime t0 = microsec_clock::local_time(); // record initial time for elasped time calculation

    timer.expires_from_now(seconds(1)); //set wait time to be 1 seconds
    timer.async_wait(boost::bind(&timer_expire_callback, boost::ref(timer))); // use bind to for param of the function passed in
    
    service.post(boost::bind(&background_task, boost::ref(service), t0)); //boost::ref is just pass by reference within bind
    
    stop_timer.expires_from_now(seconds(10)); //run untill 10 seconds from now 
    stop_timer.async_wait(boost::bind(&terminate_service, boost::ref(service)));
    
    service.run(); //remark: io_service::run() is a blocking method

    return 0;
}
