#include <iostream>
#include <boost/thread/thread.hpp>
#include <vector>

boost::mutex mu;
boost::condition_variable_any cond1, cond2;
bool started = false;

void thread1(int cnt) {
    mu.lock();
    started = true;
    mu.unlock();
    boost::this_thread::yield();
    mu.lock();
    while(cnt > 0) {
        std::cout << "--- " << cnt-- << std::endl;
        cond2.notify_one();
        cond1.wait(mu);
    } 
    cond2.notify_one();
    mu.unlock();
}

int main(void) {
    int cnt = 100;
    std::cout << "<=========== Thread ===========>" << std::endl;
    boost::thread thrd( boost::bind(thread1, cnt) );
    while(started == false);
    mu.lock();
    for(int i = 0; i < cnt; i++) {
        std::cout << "+++ " << i << std::endl;
        cond1.notify_one();
        cond2.wait(mu);
    }
    cond1.notify_one();
    mu.unlock();
    thrd.join();
    return 0;
}