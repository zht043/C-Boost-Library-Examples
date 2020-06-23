#include <iostream>
#include <boost/thread/thread.hpp>
#include <vector>

boost::mutex mu;
bool started = false;

std::vector<double> tlog1, tlog2;

boost::mutex mu2;
double micros(void) {
    mu2.lock();
    auto t = boost::chrono::high_resolution_clock::now();
    mu2.unlock();
    return double(t.time_since_epoch().count()) / 1000.0000f;
}
double t0;

void thread1(int cnt) {
    mu.lock();
    started = true;
    mu.unlock();
    boost::this_thread::yield();
    while(cnt >= 0) {
        mu.lock();
        std::cout << "--- " << cnt-- << std::endl;
        mu.unlock();
        tlog2.push_back(micros() - t0);
        boost::this_thread::yield(); // after this thread calls yield(), this thread 
                                     // gives up the remainder of the time slice, and the 
                                     // next time slice might be scheduled to be used by
                                     // this thread again, or another thread, depending on the
                                     // scheduler. In other words, yield() doesn't guarentee the other
                                     // thread will execute after this thread yields 
    } 
}

int main(void) {
    int cnt = 100;
    std::cout << "<=========== Thread ===========>" << std::endl;
    t0 = micros();
    boost::thread thrd( boost::bind(thread1, cnt) );
    while(started == false);
    for(int i = 0; i < cnt; i++) {
        mu.lock();
        std::cout << "+++ " << i << std::endl;
        mu.unlock();
        tlog1.push_back(micros() - t0);
        boost::this_thread::yield();
    }
    thrd.join();
    
    for(auto& t : tlog1) {
        std::cout << int(t) << " ";
    }
    std::cout << std::endl << std::endl;
    
    for(auto& t : tlog2) {
        std::cout << int(t) << " ";
    } 
    std::cout << std::endl;
    return 0;
}