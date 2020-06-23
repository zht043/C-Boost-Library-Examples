#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;
using namespace boost::posix_time;
using namespace boost::asio;


void timer_expire_callback(ptime t0, bool* expiration_flag) {
    cout << "Total timer elapsed: "
         << (second_clock::local_time() - t0).total_seconds() << endl; // print current elapsed time
    *expiration_flag = true;
}

int main(int argc, char* argv[]) {
    cout << "===<boost library asynchronous(non-blocking mode) timer>===" << endl;

    io_service service;
    deadline_timer timer1(service), timer2(service);
    bool timer1_expired = false, timer2_expired = false;
    ptime t0 = second_clock::local_time(); // record initial time for elasped time calculation

    timer1.expires_from_now(seconds(3)); //set wait time to be 3 seconds
    timer2.expires_from_now(seconds(7)); /*two timer should run asynchornously, 
    so timer2 should expire exactly at 7sec from now, not 7+3=10sec which is what it would have been if synchronously instead*/ 
    timer1.async_wait(boost::bind(&timer_expire_callback, t0, &timer1_expired)); // use bind to for param of the function passed in
    timer2.async_wait(boost::bind(&timer_expire_callback, t0, &timer2_expired));
    service.run(); //remark: io_service::run() is a blocking method

    cout << "timer1 expired? : " << (timer1_expired?"True":"False") << endl;
    cout << "timer2 expired? : " << (timer2_expired?"True":"False") << endl;
    return 0;
}
